#include "converters/midi.h"

#include <functional>

#include <QFile>
#include <QPoint>
#include <QString>
#include <QTextCodec>

#include <QMidiFile.h>

namespace QDspx {

    // Use `https://github.com/waddlesplash/QMidi` to read and write MIDI file.

    QString ToneNumToToneName(int num) {
        const QString tones[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

        int step = num % 12;
        int octave = num / 12;

        if (octave < 0 || step < 0 || step >= 12) {
            return "Invalid tone or octave";
        }

        return tones[step] + QString::number(octave);
    }

    ReturnCode MidiConverter::load(const QString &path, Model *out, const QVariantMap &args) {
        QMidiFile midi;
        if (!midi.load(path)) {
            return {ReturnCode::File, QFileDevice::ReadError}; // Ignore concrete error type
        }

        // Convert
        // Midi type, quaver diaeresis ticks number, track number, time type
        int midiFormat = midi.fileFormat();
        int resolution = midi.resolution();
        auto tracksCount = midi.tracks().size();
        int divType = midi.divisionType();

        // Verify the number of tracks and the type of midi
        if (tracksCount == 0) {
            return {ReturnCode::Empty};
        } else if (midiFormat == 0 && tracksCount > 1) {
            return {ReturnCode::InvalidFormat};
        } else if (midiFormat == 2 || divType != QMidiFile::PPQ) {
            return {UnsupportedType};
        }

        // Parsing Tempo Map
        QMap<int, double> tempos;
        tempos.insert(0, 120);

        QVector<QPair<int, QByteArray>> markers;

        QMap<int, QPoint> timeSign;
        timeSign.insert(0, QPoint(4, 4));

        QList<QMidiEvent *> tempMap = midi.eventsForTrack(0);

        for (auto e : std::as_const(tempMap)) {
            const auto &data = e->data();
            if (e->type() == QMidiEvent::Meta) {
                switch (e->number()) {
                    case QMidiEvent::Tempo:
                        tempos.insert(e->tick(), e->tempo());
                        break;
                    case QMidiEvent::Marker:
                        markers.append(qMakePair(e->tick(), data));
                        break;
                    case QMidiEvent::TimeSignature: {
                        QPoint sig(QPoint(data[0], 2 ^ data[1]));
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

#pragma pack(1)
        struct LogicTrack {
            quint8 key;     // 0~16
            quint8 channel; // 16~24
            quint16 track;  // 24~32

            LogicTrack() : LogicTrack(0, 0, 0){};

            LogicTrack(quint16 track, quint8 channel, quint8 key) {
                this->track = track;
                this->channel = channel;
                this->key = key;
            };

            [[nodiscard]] qint32 toInt() const {
                return *reinterpret_cast<const qint32 *>(this);
            }

            static LogicTrack fromInt(qint32 n) {
                return *reinterpret_cast<LogicTrack *>(&n);
            }
        };
#pragma pack()

        Q_STATIC_ASSERT(sizeof(LogicTrack) == sizeof(qint32));

        struct TrackNameAndLyrics {
            QByteArray name;
            qint32 trackEnd;
            QMap<qint32, QByteArray> lyrics; // key: pos; value: lyric;
            TrackNameAndLyrics() : trackEnd(0){};
        };

        // key: pack(track, channel, 0); ordered
        std::set<qint32> trackAndChannelIndexSet;

        // key: track index; value: track name;
        QHash<qint32, TrackNameAndLyrics> trackNameAndLyrics;

        // key: pack(track, channel, key);  value: ...;
        QHash<qint32, QPair<QList<qint32>, QList<qint32>>> noteMap;

        // Parsing metadata
        for (int i = midiFormat; i < tracksCount; ++i) {
            QList<QMidiEvent *> list = midi.eventsForTrack(i);
            qint32 trackIndex = i - midiFormat + 1;

            TrackNameAndLyrics cur;
            cur.trackEnd = midi.trackEndTick(i);

            // Package data using track, channel, and note as indexes
            for (auto e : list) {
                // midi元事件
                switch (e->type()) {
                    case QMidiEvent::Meta: {
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
                    case QMidiEvent::NoteOn: {
                        // Add packed(track, channel, 0)
                        trackAndChannelIndexSet.insert(
                            LogicTrack(trackIndex, e->voice(), 0).toInt());
                        // Add packed(track, channel, key)
                        noteMap[LogicTrack(trackIndex, e->voice(), e->note()).toInt()].first.append(
                            e->tick());
                        break;
                    }
                    case QMidiEvent::NoteOff: {
                        // Add packed(track, channel, key)
                        noteMap[LogicTrack(trackIndex, e->voice(), e->note()).toInt()]
                            .second.append(e->tick());
                        break;
                    }
                    default:
                        break;
                }
            }
            // Add
            trackNameAndLyrics.insert(trackIndex, cur);
        }

        struct LogicTrackInfo {
            TrackInfo option;
        };

        // Analytical Track
        // key: pack(track, channel, 0); value: ...;
        QMap<qint32, LogicTrackInfo> logicTrackInfoList;

        struct LogicNote {
            int pos;
            int len;
            int key;
            QByteArray lyric;

            LogicNote() : LogicNote(0, 0, 0){};
            LogicNote(int pos, int len, int key) : pos(pos), len(len), key(key){};

            bool operator<(const LogicNote &other) const {
                if (pos == other.pos) {
                    return key < other.key;
                }
                return pos < other.pos;
            }
        };

        // key: pack(track, channel, 0); value: (pos, LogicNote)
        QMap<qint32, std::set<LogicNote>> logicTrackNotes;

        // Existing logical trajectory
        for (const auto &packData : trackAndChannelIndexSet) {
            LogicTrack trackAndChannelIndex = LogicTrack::fromInt(packData);

            qint32 noteCount = 0;
            std::set<qint32> staticKeyNum;

            auto &currentNoteSet = logicTrackNotes[packData];

            // Traverse keyNum
            for (int key = 0; key < 128; ++key) {
                LogicTrack tempIndex(trackAndChannelIndex.track, trackAndChannelIndex.channel, key);

                auto it = noteMap.find(tempIndex.toInt());
                if (it == noteMap.end()) {
                    // The current key of the current trackAndChannelIndex has no notes
                    continue;
                }

                // Using track, channel, and note as indexes, retrieve a noteMap
                const auto &noteListPair = it.value();

                // Verify the pairing of NoteOn/Off events for each keyNum
                if (noteListPair.first.size() != noteListPair.second.size()) {
                    return {NoteEventInconsistent};
                }

                // Store the key that appears
                staticKeyNum.insert(key);

                // Logical Track Encapsulation Note
                for (int i = 0; i < noteListPair.first.size(); ++i) {
                    noteCount++;

                    LogicNote note;
                    note.pos = noteListPair.first[i];
                    note.len = noteListPair.second[i] - note.pos;
                    note.key = key;

                    {
                        const auto &lyricsMap =
                            trackNameAndLyrics[trackAndChannelIndex.track].lyrics;
                        auto it2 = lyricsMap.find(note.pos);
                        if (it2 != lyricsMap.end()) {
                            note.lyric = it2.value();
                        }
                    }

                    currentNoteSet.insert(note);
                }
            }

            // Obtain Logical Track Range
            QString logicTrackPitchRange;
            if (!staticKeyNum.empty()) {
                logicTrackPitchRange = ToneNumToToneName(*staticKeyNum.begin()) + "-" +
                                       ToneNumToToneName(*staticKeyNum.rbegin());
            }


            // Logic Track Name
            TrackInfo info(trackNameAndLyrics[trackAndChannelIndex.track].name,
                           trackNameAndLyrics[trackAndChannelIndex.track].lyrics.values());
            info.channelIndex = trackAndChannelIndex.channel;
            info.noteCount = noteCount;
            info.keyRange = staticKeyNum.empty()
                                ? qMakePair(-1, -1)
                                : qMakePair(*staticKeyNum.begin(), *staticKeyNum.rbegin());
            logicTrackInfoList.insert(packData, LogicTrackInfo{info});
        }

        // Select
        QList<qint32> selectLogicIndexs;
        QTextCodec *codec;
        {
            using Selector = std::function<bool(const QList<TrackInfo> &, const QList<QByteArray> &,
                                                QList<int> *, QTextCodec **)>;
            auto &selector = *reinterpret_cast<Selector *>(
                args.value(QStringLiteral("selector")).value<quintptr>());

            // Info
            QList<TrackInfo> trackInfoList;
            QList<QByteArray> labelList;

            QList<qint32> logicIndexList;
            for (auto it = logicTrackInfoList.begin(); it != logicTrackInfoList.end(); ++it) {
                logicIndexList.append(it.key());
                trackInfoList.append(it.value().option);
            }
            if (logicIndexList.empty()) {
                return {ReturnCode::Empty};
            }

            // Result
            QList<int> selectIDs;
            if (!selector(trackInfoList, labelList, &selectIDs, &codec)) {
                return ReturnCode::Aborted;
            }
            for (auto id : selectIDs) {
                selectLogicIndexs.append(logicIndexList[id]);
            }
        }

        // Scale factor
        double scaleFactor = 480.0 / resolution;

        // TempoMap Track Data
        QDspx::Timeline timeLine;

        QDspx::TimeSignature timeSignature;
        for (auto it = timeSign.begin(); it != timeSign.end(); ++it) {
            timeSignature.pos = int(it.key() * scaleFactor);
            timeSignature.num = it->x();
            timeSignature.den = it->y();
            timeLine.timeSignatures.append(timeSignature);
        }

        QDspx::Tempo tempo;
        for (auto it = tempos.begin(); it != tempos.end(); ++it) {
            tempo.pos = it.key();
            tempo.value = it.value();
            timeLine.tempos.append(tempo);
        }

        QDspx::Label label;
        for (auto &it : markers) {
            label.pos = int(it.first * scaleFactor);
            label.text = codec->toUnicode(it.second);
            timeLine.labels.append(label);
        }

        QDspxModel model;
        model.content.timeline = timeLine;

        // Track Data
        for (auto &logicID : selectLogicIndexs) {
            LogicTrack tempIndex = LogicTrack::fromInt(logicID);
            QDspx::Track track;
            auto clip = QDspx::SingingClipRef::create();
            auto logicNotes = logicTrackNotes[logicID];

            int clipEnd = int(trackNameAndLyrics[tempIndex.track].trackEnd * scaleFactor);
            QDspx::ClipTime clipTime(0, clipEnd, 0, clipEnd);
            clip->time = clipTime;
            clip->name = codec->toUnicode(logicTrackInfoList[logicID].option.title);

            // Fill note
            for (const auto &logicNote : logicNotes) {
                QDspx::Note note;

                note.pos = int(logicNote.pos * scaleFactor);
                note.length = int(logicNote.len * scaleFactor);
                note.keyNum = logicNote.key;
                note.lyric = codec->toUnicode(logicNote.lyric);
                clip->notes.append(note);
            }

            // Fill in track information
            track.name = clip->name;
            track.clips.append(clip);
            model.content.tracks.append(track);
        }

        *out = std::move(model);

        return ReturnCode::Success;
    }

    ReturnCode MidiConverter::save(const QString &path, const Model &in, const QVariantMap &args) {
        QMidiFile midi;
        midi.setFileFormat(1);
        midi.setDivisionType(QMidiFile::DivisionType::PPQ);
        midi.setResolution(480);

        // Convert
        QDspx::Timeline timeLine = in.content.timeline;
        auto trackNum = in.content.tracks.size();

        // Tempo Map
        midi.createTrack();

        QTextCodec *codec = QTextCodec::codecForName("UTF-8");

        // timeSignature
        for (const auto &timeSignature : timeLine.timeSignatures) {
            QByteArray buf;
            buf.resize(4);
            buf[0] = int(timeSignature.num);
            buf[1] = int(std::log2(timeSignature.den));
            buf[2] = 24;
            buf[3] = 8;
            midi.createMetaEvent(0, timeSignature.pos, QMidiEvent::TimeSignature, buf);
        }

        // tempos
        for (const auto &tempo : timeLine.tempos) {
            midi.createTempoEvent(0, tempo.pos, float(tempo.value));
        }


        // label
        for (const auto &label : timeLine.labels) {
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
            for (const auto &clip : track.clips) {
                if (clip->type == QDspx::Clip::Singing) {
                    auto singingClip = clip.dynamicCast<QDspx::SingingClip>();

                    int clipStart = singingClip->time.clipStart;
                    int clipEnd = clipStart + singingClip->time.clipLen;
                    int posCursor = clipStart;
                    for (const auto &note : singingClip->notes) {
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

            auto &overlapHandler = *reinterpret_cast<OverlapHandler *>(
                args.value(QStringLiteral("overlapHandler")).value<quintptr>());
            if (!overlapHandler()) {
                return ReturnCode::Aborted;
            }
        }

        if (!midi.save(path)) {
            return {ReturnCode::File, QFileDevice::WriteError}; // Ignore concrete error type
        }

        return ReturnCode::Success;
    }

}