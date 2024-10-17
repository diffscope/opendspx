#include "converters/midi.h"

#include <cmath>
#include <functional>
#include <iostream>

#include <QFile>
#include <QPoint>
#include <QString>
#include <QTextCodec>

#include <QMidiFile.h>

namespace QDspx
{
    // Use `https://github.com/waddlesplash/QMidi` to read and write MIDI file.

    // Import MIDI file to QDspxModel.

    // Convert note number to note name.
    QString ToneNumToToneName(int num) {
        static const QString tones[] = {
            "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B",
        };

        int step = num % 12;
        int octave = num / 12 - 1;

        if (num < 0) {
            octave -= 1;
            step = (step + 12) % 12;
        }

        // Verify the tone and octave
        if (octave < -1 || step < 0 || step >= 12) {
            return "Invalid tone or octave";
        }

        return tones[step] + QString::number(octave);
    }

    // Load MIDI file to QDspxModel.
    Result MidiConverter::load(const QString& path, Model* out, const QVariantMap& args) {
        QMidiFile midi;
        if (!midi.load(path)) {
            return {Result::File, QFileDevice::ReadError}; // Ignore concrete error type
        }

        // Convert
        // Midi type, quaver diaeresis ticks number, track number, time type
        int midiFormat = midi.fileFormat();
        int resolution = midi.resolution();
        auto tracksCount = midi.tracks().size();
        int divType = midi.divisionType();

        // Verify the number of tracks and the type of midi
        if (tracksCount == 0) {
            return {Result::Empty};
        } else if (midiFormat == 0 && tracksCount > 1) {
            return {Result::InvalidFormat};
        } else if (midiFormat == 2 || divType != QMidiFile::PPQ) {
            return {UnsupportedType};
        }

        // Parsing Tempo Map
        QMap<int, double> tempos;
        tempos.insert(0, 120);

        // Parsing Marker
        QVector<QPair<int, QByteArray>> markers;

        // Parsing Time Signature
        QMap<int, QPoint> timeSign;
        timeSign.insert(0, QPoint(4, 4));

        // Parsing Meta Event of Track 0(Track 0 is the tempo map)
        QList<QMidiEvent*> tempMap = midi.eventsForTrack(0);

        // Parsing Meta Event
        for (auto e : std::as_const(tempMap)) {
            const auto& data = e->data();
            if (e->type() == QMidiEvent::Meta) {
                switch (e->number()) {
                case QMidiEvent::Tempo:
                    tempos.insert(e->tick(), e->tempo());
                    break;
                case QMidiEvent::Marker:
                    markers.append(qMakePair(e->tick(), data));
                    break;
                case QMidiEvent::TimeSignature:
                    {
                        auto sig(QPoint(data[0], 2 ^ data[1]));
                        if (sig.x() == 0 || sig.y() == 0)
                            break;
                        timeSign.insert(e->tick(), sig);
                        break;
                    }
                default:
                    break;
                }
            }
        }

        // Physical track is midi track.
        // Logical track is to subdivide the tracks of MIDI files into channels.
        // Pitch index is to subdivide the pitch of the Logical track into the pitches.
        // Encapsulate data for indexes using track, channel, and note.
#pragma pack(1)
        struct LogicIndex {
            quint8 key; // 0~16, note pitch
            quint8 channel; // 16~24, note channel
            quint16 track; // 24~32, note track

            LogicIndex() : LogicIndex(0, 0, 0) {};

            LogicIndex(const quint16& track, const quint8& channel, const quint8& key) {
                this->track = track;
                this->channel = channel;
                this->key = key;
            };

            [[nodiscard]] qint32 toInt() const {
                return *reinterpret_cast<const qint32*>(this);
            }

            static LogicIndex fromInt(qint32 n) {
                return *reinterpret_cast<LogicIndex*>(&n);
            }
        };
#pragma pack()

        Q_STATIC_ASSERT(sizeof(LogicIndex) == sizeof(qint32));

        // struct to store track name, lyrics and track end tick.
        struct TrackNameAndLyrics {
            QByteArray name;
            qint32 trackEnd;
            QMap<qint32, QByteArray> lyrics; // key: pos; value: lyric;
            TrackNameAndLyrics() : trackEnd(0) {};
        };

        // Logical track index set.
        // key: pack(track, channel, 0); ordered
        std::set<qint32> logicIndexSet;

        // key: track index; value: track name;
        QHash<qint32, TrackNameAndLyrics> physicNameAndLyrics;

        // key: pack(track, channel, key);  value: noteOn and noteOff pos;
        QHash<qint32, QPair<QList<qint32>, QList<qint32>>> pitchNoteMap;

        // Parsing Midi Event.
        for (int i = midiFormat; i < tracksCount; ++i) {
            QList<QMidiEvent*> list = midi.eventsForTrack(i);
            qint32 trackIndex = i - midiFormat + 1;

            TrackNameAndLyrics cur;
            cur.trackEnd = midi.trackEndTick(i);

            // Package data using track, channel, and note as indexes.
            for (auto e : list) {
                // Parsing Meta Event
                switch (e->type()) {
                case QMidiEvent::Meta:
                    {
                        switch (e->number()) {
                        case QMidiEvent::TrackName:
                            cur.name = e->data();
                            break;
                        case QMidiEvent::Lyric:
                            cur.lyrics.insert(e->tick(), e->data());
                            break;
                        default:
                            break;
                        }
                        break;
                    }
                // Parsing Note Event
                case QMidiEvent::NoteOn:
                    {
                        // Add packed(track, channel, 0)
                        logicIndexSet.insert(LogicIndex(trackIndex, e->voice(), 0).toInt());
                        // Add packed(track, channel, key), noteOn pos.
                        pitchNoteMap[LogicIndex(trackIndex, e->voice(), e->note()).toInt()]
                            .first.append(e->tick());
                        break;
                    }
                case QMidiEvent::NoteOff:
                    {
                        // Add packed(track, channel, key), noteOff pos.
                        pitchNoteMap[LogicIndex(trackIndex, e->voice(), e->note()).toInt()]
                            .second.append(e->tick());
                        break;
                    }
                default:
                    break;
                }
            }
            // Add physic track name and lyrics.
            physicNameAndLyrics.insert(trackIndex, cur);
        }

        // Logical Track Information, used to select the track to be imported.
        struct LogicTrackInfo {
            TrackInfo option;
        };

        // Logical Track is to subdivide the tracks of MIDI files into channels.
        // key: pack(track, channel, 0); value: TrackInfo;
        QMap<qint32, LogicTrackInfo> logicTrackInfoList;

        // Logical Note
        struct LogicNote {
            int pos;
            int len;
            int key;
            QByteArray lyric;

            LogicNote() : LogicNote(0, 0, 0) {};

            LogicNote(int pos, int len, int key) : pos(pos), len(len), key(key) {};

            bool operator<(const LogicNote& other) const {
                if (pos == other.pos) {
                    return key < other.key;
                }
                return pos < other.pos;
            }
        };

        // key: pack(track, channel, 0); value: set<LogicNote>;
        QMap<qint32, std::set<LogicNote>> logicTrackNotes;

        // Traverse the logical index set to encapsulate the logical track.
        for (const auto& packedLogicIndex : logicIndexSet) {
            LogicIndex logicIndex = LogicIndex::fromInt(packedLogicIndex);

            qint32 noteCount = 0;
            std::set<qint32> staticKeyNum;

            // Encapsulate the logical track.
            auto& currentNoteSet = logicTrackNotes[packedLogicIndex];

            // Traverse the pitchNoteMap to encapsulate the logical note.
            for (int key = 0; key < 128; ++key) {
                LogicIndex tempIndex(logicIndex.track, logicIndex.channel, key);

                // Using track, channel, and note as indexes, retrieve a pitchNoteMap.
                auto it = pitchNoteMap.find(tempIndex.toInt());
                if (it == pitchNoteMap.end()) {
                    // If the note does not exist, continue.
                    continue;
                }

                // NoteOn and NoteOff events for each keyNum.
                const auto& noteListPair = it.value();

                // Verify the number of NoteOn and NoteOff events.
                if (noteListPair.first.size() != noteListPair.second.size()) {
                    return {NoteEventInconsistent};
                }

                // Store the key that appears, used to obtain the pitch range.
                staticKeyNum.insert(key);

                // Logical Track Encapsulation Note.
                // Using track, channel, and note as indexes, retrieve the lyrics.
                const auto& lyricsMap = physicNameAndLyrics[logicIndex.track].lyrics;
                for (int i = 0; i < noteListPair.first.size(); ++i) {
                    noteCount++;

                    LogicNote note;
                    note.pos = noteListPair.first[i];
                    note.len = noteListPair.second[i] - note.pos;
                    note.key = key;

                    {
                        // Using track, channel, and note as indexes, retrieve the lyrics.
                        auto it2 = lyricsMap.find(note.pos);
                        if (it2 != lyricsMap.end()) {
                            note.lyric = it2.value();
                        }
                    }

                    currentNoteSet.insert(note);
                }
            }


            // Logic Track Name
            TrackInfo info(physicNameAndLyrics[logicIndex.track].name,
                           physicNameAndLyrics[logicIndex.track].lyrics.values());
            info.channelIndex = logicIndex.channel;
            info.noteCount = noteCount;
            info.keyRange = staticKeyNum.empty()
                                ? "-"
                                : ToneNumToToneName(*staticKeyNum.begin()) + "-" +
                                ToneNumToToneName(*staticKeyNum.rbegin());
            logicTrackInfoList.insert(packedLogicIndex, LogicTrackInfo{info});
        }

        // Select
        QList<qint32> selectLogicIndexes;
        QTextCodec* codec = QTextCodec::codecForName("UTF-8");
        {
            using Selector = std::function<bool(const QList<TrackInfo>&, const QList<QByteArray>&,
                                                QList<int>*, QTextCodec**)>;
            auto& selector = *reinterpret_cast<Selector*>(
                args.value(QStringLiteral("selector")).value<quintptr>());

            // Info
            QList<TrackInfo> trackInfoList;
            QList<QByteArray> labelList;

            // Fill in packed index and track information.
            QList<qint32> logicIndexList;
            for (auto it = logicTrackInfoList.begin(); it != logicTrackInfoList.end(); ++it) {
                logicIndexList.append(it.key());
                trackInfoList.append(it.value().option);
            }
            if (logicIndexList.empty()) {
                return {Result::Empty};
            }

            // Result
            QList<int> selectIDs;
            if (!selector(trackInfoList, labelList, &selectIDs, &codec)) {
                return Result::Aborted;
            }
            for (auto id : selectIDs) {
                selectLogicIndexes.append(logicIndexList[id]);
            }
        }

        // Scale factor
        double scaleFactor = 480.0 / resolution;

        // TempoMap Track Data
        QDspx::Timeline timeLine;

        for (auto it = timeSign.begin(); it != timeSign.end(); ++it) {
            QDspx::TimeSignature timeSignature;
            timeSignature.pos = static_cast<int>(it.key() * scaleFactor);
            timeSignature.num = it->x();
            timeSignature.den = it->y();
            timeLine.timeSignatures.append(timeSignature);
        }

        for (auto it = tempos.begin(); it != tempos.end(); ++it) {
            QDspx::Tempo tempo;
            tempo.pos = it.key();
            tempo.value = it.value();
            timeLine.tempos.append(tempo);
        }

        QDspx::Label label;
        for (auto& it : markers) {
            label.pos = static_cast<int>(it.first * scaleFactor);
            label.text = codec->toUnicode(it.second);
            timeLine.labels.append(label);
        }

        QDspxModel model;
        model.content.timeline = timeLine;

        // Track Data
        for (auto& logicID : selectLogicIndexes) {
            LogicIndex tempIndex = LogicIndex::fromInt(logicID);
            QDspx::Track track;
            auto clip = QDspx::SingingClipRef::create();
            auto logicNotes = logicTrackNotes[logicID];

            int clipEnd = static_cast<int>(physicNameAndLyrics[tempIndex.track].trackEnd * scaleFactor);
            QDspx::ClipTime clipTime(0, clipEnd, 0, clipEnd);
            clip->time = clipTime;
            clip->name = codec->toUnicode(logicTrackInfoList[logicID].option.title);

            // Fill note.
            for (const auto& logicNote : logicNotes) {
                QDspx::Note note;

                note.pos = static_cast<int>(logicNote.pos * scaleFactor);
                note.length = static_cast<int>(logicNote.len * scaleFactor);
                note.keyNum = logicNote.key;
                note.lyric = codec->toUnicode(logicNote.lyric);
                clip->notes.append(note);
            }

            // Fill in track information.
            track.name = clip->name;
            track.clips.append(clip);
            model.content.tracks.append(track);
        }

        *out = std::move(model);

        return Result::Success;
    }

    Result MidiConverter::save(const QString& path, const Model& in, const QVariantMap& args) {
        QMidiFile midi;
        midi.setFileFormat(1);
        midi.setDivisionType(QMidiFile::DivisionType::PPQ);
        midi.setResolution(480);

        // Convert
        QDspx::Timeline timeLine = in.content.timeline;
        const auto trackNum = in.content.tracks.size();

        // Tempo Map
        midi.createTrack();

        const QTextCodec* codec = QTextCodec::codecForName("UTF-8");

        // timeSignature
        for (const auto& timeSignature : timeLine.timeSignatures) {
            QByteArray buf;
            buf.resize(4);
            buf[0] = static_cast<char>(timeSignature.num);
            buf[1] = static_cast<char>(std::log2(timeSignature.den));
            buf[2] = 24;
            buf[3] = 8;
            midi.createMetaEvent(0, timeSignature.pos, QMidiEvent::TimeSignature, buf);
        }

        // tempos
        for (const auto& tempo : timeLine.tempos) {
            midi.createTempoEvent(0, tempo.pos, static_cast<float>(tempo.value));
        }

        // label
        for (const auto& label : timeLine.labels) {
            midi.createMarkerEvent(0, label.pos, codec->fromUnicode(label.text));
        }

        // track
        QList<QPair<int, QDspx::Note>> overlapNotes;
        for (int trackId = 0; trackId < trackNum; trackId++) {
            QDspx::Track track = in.content.tracks[trackId];
            midi.createTrack();
            midi.createMetaEvent(trackId + 1, 0, QMidiEvent::MetaNumbers::TrackName,
                                 codec->fromUnicode(track.name));


            // clip
            for (const auto& clip : track.clips) {
                if (clip->type == QDspx::Clip::Singing) {
                    auto singingClip = clip.dynamicCast<QDspx::SingingClip>();

                    int clipStart = singingClip->time.clipStart;
                    int clipEnd = clipStart + singingClip->time.clipLen;
                    int posCursor = clipStart;
                    for (const auto& note : singingClip->notes) {
                        if (clipStart <= note.pos && note.pos < clipEnd) {
                            if (posCursor > note.pos) {
                                overlapNotes.append(qMakePair(trackId, note));
                            }
                            int noteEnd = note.pos + note.length;
                            midi.createLyricEvent(trackId + 1, note.pos,
                                                  codec->fromUnicode(note.lyric));
                            midi.createNoteOnEvent(trackId + 1, note.pos, 0, note.keyNum, 64);
                            midi.createNoteOffEvent(trackId + 1, std::min(noteEnd, clipEnd), 0,
                                                    note.keyNum, 64);
                            posCursor = std::max(posCursor, note.pos + note.length);
                        }
                    }
                }
            }
        }

        // Overlapped ?
        if (!overlapNotes.empty()) {
            using OverlapHandler = std::function<bool()>;

            auto& overlapHandler = *reinterpret_cast<OverlapHandler*>(
                args.value(QStringLiteral("overlapHandler")).value<quintptr>());
            if (!overlapHandler()) {
                return Result::Aborted;
            }
        }

        if (!midi.save(path)) {
            return {Result::File, QFileDevice::WriteError}; // Ignore concrete error type
        }

        return Result::Success;
    }
}
