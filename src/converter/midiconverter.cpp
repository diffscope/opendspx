#include "midi/midiconverter.h"

#include <algorithm>
#include <cstdint>
#include <ranges>
#include <deque>
#include <map>
#include <vector>
#include <utility>

#include <wolf-midi/MidiFile.h>

#include <opendspx/model.h>
#include <opendspxconverter/midi/midiintermediatedata.h>

namespace opendspx {

    constexpr int kDspxTicksPerQuarter = 480;

    MidiIntermediateData MidiConverter::convertMidiToIntermediate(std::istream &in, Error &error, ConvertMidiToIntermediateOption option) {
        error = Error::NoError;

        Midi::MidiFile midiFile;
        if (!midiFile.load(in)) {
            error = Error::InvalidMidiData;
            return {};
        }

        // Only MIDI format 0/1 with PPQ timing is supported.
        if ((midiFile.fileFormat() != 0 && midiFile.fileFormat() != 1) || midiFile.divisionType() != Midi::MidiFile::PPQ) {
            error = Error::UnsupportedFormat;
            return {};
        }

        const int srcTpqn = midiFile.resolution();
        if (srcTpqn <= 0) {
            error = Error::UnsupportedFormat;
            return {};
        }

        midiFile.sort();

        std::vector<MidiIntermediateData::Tempo> tempos;
        std::vector<MidiIntermediateData::TimeSignature> timeSignatures;
        std::vector<MidiIntermediateData::Marker> markers;
        std::vector<MidiIntermediateData::Track> tracks;

        const bool separateChannels = option.separateChannels;

        for (int trackId : midiFile.tracks()) {
            const auto events = midiFile.eventsForTrack(trackId);
            std::string trackTitle;

            struct LyricEvent {
                int tick;
                std::string text;
            };
            std::vector<LyricEvent> lyricEvents;

            // Gather meta events and lyrics first to reference them while building notes.
            for (const auto *event : events) {
                if (!event || event->type() != Midi::MidiEvent::Meta) {
                    continue;
                }

                const int scaledTick = event->tick();
                switch (event->number()) {
                    case Midi::MidiEvent::TrackName: {
                        if (trackTitle.empty()) {
                            const auto data = event->data();
                            trackTitle = std::string(data.data(), static_cast<int>(data.size()));
                        }
                        break;
                    }
                    case Midi::MidiEvent::Tempo: {
                        tempos.emplace_back(scaledTick, static_cast<double>(event->tempo()));
                        break;
                    }
                    case Midi::MidiEvent::TimeSignature: {
                        const auto data = event->data();
                        if (data.size() >= 2) {
                            const int numerator = static_cast<unsigned char>(data[0]);
                            const int denominator = 1 << static_cast<unsigned char>(data[1]);
                            timeSignatures.emplace_back(scaledTick, numerator, denominator);
                        }
                        break;
                    }
                    case Midi::MidiEvent::Marker: {
                        const auto data = event->data();
                        markers.emplace_back(scaledTick, std::string(data.data(), static_cast<int>(data.size())));
                        break;
                    }
                    case Midi::MidiEvent::Lyric: {
                        const auto data = event->data();
                        lyricEvents.emplace_back(scaledTick, std::string(data.data(), static_cast<int>(data.size())));
                        break;
                    }
                    default:
                        break;
                }
            }

            struct PendingNote {
                int startTick;
                std::string lyric;
            };

            // channel -> key -> FIFO notes (to support overlapping notes on same key)
            std::map<int, std::map<int, std::deque<PendingNote>>> pendingNotes;
            std::vector<MidiIntermediateData::Note> combinedNotes;
            std::map<int, std::vector<MidiIntermediateData::Note>> channelNotes; // channel -> notes

            std::size_t lyricIndex = 0;

            // Walk note events, pairing NoteOn/NoteOff and attaching the next lyric at or before NoteOn.
            for (const auto *event : events) {
                if (!event) {
                    continue;
                }

                const auto type = event->type();
                if (type != Midi::MidiEvent::NoteOn && type != Midi::MidiEvent::NoteOff) {
                    continue;
                }

                const int channel = event->voice();
                const int key = event->note();
                if (channel < 0 || key < 0) {
                    continue;
                }

                const int scaledTick = event->tick();
                const bool isNoteOff = type == Midi::MidiEvent::NoteOff || event->velocity() == 0;

                if (isNoteOff) {
                    auto channelIt = pendingNotes.find(channel);
                    if (channelIt == pendingNotes.end()) {
                        continue;
                    }

                    auto &keyMap = channelIt->second;
                    auto keyIt = keyMap.find(key);
                    if (keyIt == keyMap.end() || keyIt->second.empty()) {
                        continue;
                    }

                    const PendingNote pending = keyIt->second.front();
                    keyIt->second.pop_front();
                    const int length = scaledTick - pending.startTick;
                    if (length >= 0) {
                        MidiIntermediateData::Note note{pending.startTick, length, key, pending.lyric};
                        if (separateChannels) {
                            channelNotes[channel].push_back(note);
                        } else {
                            combinedNotes.push_back(note);
                        }
                    }

                    if (keyIt->second.empty()) {
                        keyMap.erase(keyIt);
                        if (keyMap.empty()) {
                            pendingNotes.erase(channelIt);
                        }
                    }
                    continue;
                }

                std::string lyric;
                if (lyricIndex < lyricEvents.size() && lyricEvents[lyricIndex].tick <= scaledTick) {
                    lyric = lyricEvents[lyricIndex].text;
                    ++lyricIndex;
                }

                pendingNotes[channel][key].push_back(PendingNote{scaledTick, lyric});
            }

            if (separateChannels) {
                for (auto &channelPair : channelNotes) {
                    tracks.emplace_back(trackTitle, channelPair.second, channelPair.first, midiFile.trackEndTick(trackId));
                }
            } else {
                tracks.emplace_back(trackTitle, combinedNotes, -1, midiFile.trackEndTick(trackId));
            }
        }

        return {srcTpqn, tempos, timeSignatures, markers, tracks};
    }

    Model MidiConverter::convertIntermediateToDspx(const MidiIntermediateData &midiData, bool *ok) {
        return convertIntermediateToDspx(midiData, [](const std::string &data) { return data; }, ok);
    }

    Model MidiConverter::convertIntermediateToDspx(const MidiIntermediateData &midiData, const std::function<std::string(const std::string &)> &decodeText, bool *ok) {
        if (ok) {
            *ok = false;
        }

        if (!midiData.isValid()) {
            return {};
        }

        const int srcResolution = midiData.resolution();
        if (srcResolution <= 0) {
            return {};
        }

        const auto scaleTick = [srcResolution](int tick) -> int {
            return static_cast<int>(static_cast<std::int64_t>(tick) * kDspxTicksPerQuarter / srcResolution);
        };

        Model model;

        std::vector<Tempo> tempos;
        const auto midiTempos = midiData.tempos();
        const bool hasInitialTempo = std::ranges::any_of(midiTempos, [](const auto &tempo) {
            return tempo.tick == 0;
        });
        if (!hasInitialTempo) {
            tempos.emplace_back(0, 120.0);
        }
        for (const auto &tempo : midiTempos) {
            tempos.emplace_back(scaleTick(tempo.tick), tempo.tempo);
        }

        auto midiTimeSignatures = midiData.timeSignatures();
        const bool hasInitialTimeSignature = std::ranges::any_of(midiTimeSignatures, [](const auto &sig) {
            return sig.tick == 0;
        });
        if (!hasInitialTimeSignature) {
            midiTimeSignatures.insert(midiTimeSignatures.begin(), {0, 4, 4});
        }
        std::ranges::stable_sort(midiTimeSignatures, [](const auto &lhs, const auto &rhs) {
            return lhs.tick < rhs.tick;
        });

        std::vector<TimeSignature> timeSignatures;
        if (!midiTimeSignatures.empty()) {
            const auto &firstSignature = midiTimeSignatures.front();
            timeSignatures.emplace_back(0, firstSignature.numerator, firstSignature.denominator);

            int currentNumerator = firstSignature.numerator;
            int currentDenominator = firstSignature.denominator;
            std::int64_t currentMeasureStartTick = 0;
            int currentMeasureIndex = 0;

            const auto ticksPerMeasure = [srcResolution](int numerator, int denominator) -> std::int64_t {
                return static_cast<std::int64_t>(numerator) * srcResolution * 4 / denominator;
            };

            for (int i = 1; i < midiTimeSignatures.size(); ++i) {
                const auto &signature = midiTimeSignatures.at(i);
                const std::int64_t measureTicks = ticksPerMeasure(currentNumerator, currentDenominator);
                if (measureTicks <= 0) {
                    continue;
                }

                std::int64_t deltaTicks = static_cast<std::int64_t>(signature.tick) - currentMeasureStartTick;
                if (deltaTicks < 0) {
                    deltaTicks = 0;
                }

                std::int64_t measuresForward = deltaTicks / measureTicks;
                if (deltaTicks % measureTicks != 0) {
                    ++measuresForward;
                }

                currentMeasureIndex += static_cast<int>(measuresForward);
                currentMeasureStartTick += measuresForward * measureTicks;

                if (!timeSignatures.empty() && timeSignatures.back().index == currentMeasureIndex) {
                    timeSignatures.back().numerator = signature.numerator;
                    timeSignatures.back().denominator = signature.denominator;
                } else {
                    timeSignatures.emplace_back(currentMeasureIndex, signature.numerator, signature.denominator);
                }

                currentNumerator = signature.numerator;
                currentDenominator = signature.denominator;
            }
        }

        std::vector<Label> labels;
        for (const auto &marker : midiData.markers()) {
            labels.emplace_back(scaleTick(marker.tick), decodeText(marker.text));
        }

        model.content.timeline.tempos = std::move(tempos);
        model.content.timeline.timeSignatures = std::move(timeSignatures);
        model.content.timeline.labels = std::move(labels);

        for (const auto &track : midiData.tracks()) {
            Track dspxTrack;
            dspxTrack.name = decodeText(track.title);

            std::vector<Note> notes;
            notes.reserve(track.notes.size());
            for (const auto &note : track.notes) {
                Note dspxNote;
                dspxNote.pos = scaleTick(note.noteOnTick);
                dspxNote.length = scaleTick(note.length);
                dspxNote.keyNum = note.key;
                dspxNote.lyric = decodeText(note.lyric);
                notes.push_back(std::move(dspxNote));
            }

            auto clip = std::make_shared<SingingClip>();
            clip->name = dspxTrack.name;
            clip->notes = std::move(notes);
            clip->time.clipLen = scaleTick(track.endTick);

            dspxTrack.clips.push_back(clip);
            model.content.tracks.push_back(dspxTrack);
        }

        if (ok) {
            *ok = true;
        }
        return model;
    }
    MidiIntermediateData MidiConverter::convertDspxToIntermediate(const Model &model, ConvertDspxToIntermediateOption option) {
        return convertDspxToIntermediate(model, [](const std::string &text) { return text; }, option);
    }

    MidiIntermediateData MidiConverter::convertDspxToIntermediate(const Model &model, const std::function<std::string(const std::string &)> &encodeText, ConvertDspxToIntermediateOption option) {
        if (option.resolution <= 0) {
            return {};
        }

        const auto scaleTick = [resolution = option.resolution](std::int64_t tick) -> int {
            return static_cast<int>(tick * resolution / kDspxTicksPerQuarter);
        };

        std::vector<MidiIntermediateData::Tempo> tempos;
        const auto &timeline = model.content.timeline;
        const bool hasInitialTempo = std::ranges::any_of(timeline.tempos, [](const auto &tempo) {
            return tempo.pos == 0;
        });
        if (!hasInitialTempo) {
            tempos.emplace_back(0, 120.0);
        }
        for (const auto &tempo : timeline.tempos) {
            tempos.emplace_back(scaleTick(tempo.pos), tempo.value);
        }

        auto timeSignaturesSrc = timeline.timeSignatures;
        const bool hasInitialTimeSignature = std::ranges::any_of(timeSignaturesSrc, [](const auto &signature) {
            return signature.index == 0;
        });
        if (!hasInitialTimeSignature) {
            timeSignaturesSrc.insert(timeSignaturesSrc.begin(), {0, 4, 4});
        }
        std::ranges::stable_sort(timeSignaturesSrc, [](const auto &lhs, const auto &rhs) {
            return lhs.index < rhs.index;
        });

        std::vector<MidiIntermediateData::TimeSignature> timeSignatures;
        if (!timeSignaturesSrc.empty()) {
            int currentNumerator = 4;
            int currentDenominator = 4;
            int currentIndex = 0;
            std::int64_t currentTick = 0;

            const auto ticksPerMeasure = [resolution = option.resolution](int numerator, int denominator) -> std::int64_t {
                return static_cast<std::int64_t>(numerator) * resolution * 4 / denominator;
            };

            for (const auto &signature : timeSignaturesSrc) {
                const std::int64_t measureTicks = ticksPerMeasure(currentNumerator, currentDenominator);
                if (measureTicks <= 0) {
                    continue;
                }

                std::int64_t measuresForward = static_cast<std::int64_t>(signature.index) - currentIndex;
                if (measuresForward < 0) {
                    measuresForward = 0;
                }

                const std::int64_t signatureTick = currentTick + measuresForward * measureTicks;
                timeSignatures.emplace_back(scaleTick(signatureTick), signature.numerator, signature.denominator);

                currentIndex = signature.index;
                currentTick = signatureTick;
                currentNumerator = signature.numerator;
                currentDenominator = signature.denominator;
            }
        }

        std::vector<MidiIntermediateData::Marker> markers;
        for (const auto &label : timeline.labels) {
            markers.emplace_back(scaleTick(label.pos), encodeText(label.text));
        }

        std::vector<MidiIntermediateData::Track> tracks;
        for (const auto &track : model.content.tracks) {
            std::vector<SingingClip> singingClips;
            for (const auto &clipRef : track.clips) {
                if (!clipRef || clipRef->type != Clip::Type::Singing) {
                    continue;
                }
                const auto singingClip = std::static_pointer_cast<SingingClip>(clipRef);
                if (!singingClip) {
                    continue;
                }
                singingClips.push_back(*singingClip);
            }

            if (option.separateClips) {
                for (const auto &clip : singingClips) {
                    std::vector<MidiIntermediateData::Note> notes;
                    std::int64_t trackEndTick = 0;

                    const std::int64_t start = static_cast<std::int64_t>(clip.time.pos) - clip.time.clipStart;

                    for (const auto &note : clip.notes) {
                        const std::int64_t noteStart = start + note.pos;
                        const std::int64_t noteEnd = noteStart + note.length;
                        notes.emplace_back(scaleTick(noteStart), scaleTick(note.length), note.keyNum, encodeText(note.lyric));
                        trackEndTick = std::max(trackEndTick, noteEnd);
                    }

                    trackEndTick = std::max(trackEndTick, start + clip.time.clipLen);

                    std::string name = track.name;
                    if (!clip.name.empty()) {
                        if (!name.empty()) {
                            name.append({' '});
                        }
                        name.append(clip.name);
                    }

                    tracks.emplace_back(encodeText(name), notes, -1, scaleTick(trackEndTick));
                }
            } else {
                std::vector<MidiIntermediateData::Note> notes;
                std::int64_t trackEndTick = 0;

                for (const auto &clip : singingClips) {
                    const std::int64_t start = static_cast<std::int64_t>(clip.time.pos) - clip.time.clipStart;
                    for (const auto &note : clip.notes) {
                        const std::int64_t noteStart = start + note.pos;
                        const std::int64_t noteEnd = noteStart + note.length;
                        notes.emplace_back(scaleTick(noteStart), scaleTick(note.length), note.keyNum, encodeText(note.lyric));
                        trackEndTick = std::max(trackEndTick, noteEnd);
                    }
                    trackEndTick = std::max(trackEndTick, start + clip.time.clipLen);
                }

                tracks.emplace_back(encodeText(track.name), notes, -1, scaleTick(trackEndTick));
            }
        }

        return {option.resolution, tempos, timeSignatures, markers, tracks};
    }

    void MidiConverter::convertIntermediateToMidi(std::ostream &out, const MidiIntermediateData &midiData) {
        if (!midiData.isValid() || midiData.resolution() <= 0) {
            return;
        }

        Midi::MidiFile midiFile;
        midiFile.setFileFormat(1);
        midiFile.setDivisionType(Midi::MidiFile::PPQ);
        midiFile.setResolution(midiData.resolution());

        // Track 0: global meta (tempo, time signature, markers)
        const int metaTrack = midiFile.createTrack();
        for (const auto &tempo : midiData.tempos()) {
            midiFile.createTempoEvent(metaTrack, tempo.tick, static_cast<float>(tempo.tempo));
        }
        for (const auto &sig : midiData.timeSignatures()) {
            midiFile.createTimeSignatureEvent(metaTrack, sig.tick, sig.numerator, sig.denominator);
        }
        for (const auto &marker : midiData.markers()) {
            midiFile.createMarkerEvent(metaTrack, marker.tick, std::vector<char>(marker.text.begin(), marker.text.end()));
        }

        // Tracks with notes/lyrics
        for (const auto &track : midiData.tracks()) {
            const int trackId = midiFile.createTrack();

            if (!track.title.empty()) {
                midiFile.createMetaEvent(trackId, 0, Midi::MidiEvent::TrackName, std::vector<char>(track.title.begin(), track.title.end()));
            }

            constexpr int kDefaultVelocity = 64;

            for (const auto &note : track.notes) {
                const int noteOffTick = note.noteOnTick + note.length;

                if (!note.lyric.empty()) {
                    midiFile.createLyricEvent(trackId, note.noteOnTick, std::vector<char>(note.lyric.begin(), note.lyric.end()));
                }

                midiFile.createNoteOnEvent(trackId, note.noteOnTick, 0, note.key, kDefaultVelocity);
                midiFile.createNoteOffEvent(trackId, noteOffTick, 0, note.key, kDefaultVelocity);
            }
        }

        midiFile.sort();

        midiFile.save(out);
    }

}
