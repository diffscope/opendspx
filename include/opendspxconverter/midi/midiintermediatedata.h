#ifndef OPENDSPX_CONVERTER_MIDIINTERMEDIATEDATA_H
#define OPENDSPX_CONVERTER_MIDIINTERMEDIATEDATA_H

#include <algorithm>

#include <QByteArray>
#include <QList>

namespace opendspx {

    class MidiIntermediateData {
    public:

        struct Tempo {
            int tick;
            double tempo;
        };

        struct TimeSignature {
            int tick;
            int numerator;
            int denominator;
        };

        struct Marker {
            int tick;
            QByteArray text;
        };

        struct Note {
            int noteOnTick;
            int length;
            int key;
            QByteArray lyric;
        };

        struct Track {
            QByteArray title;
            QList<Note> notes;
            int channelIndex;
            int endTick;
        };

        MidiIntermediateData() = default;

        MidiIntermediateData(int resolution, QList<Tempo> tempos, QList<TimeSignature> timeSignatures, QList<Marker> markers, QList<Track> tracks)
            : m_resolution(resolution),
              m_tempos(std::move(tempos)),
              m_timeSignatures(std::move(timeSignatures)),
              m_markers(std::move(markers)),
              m_tracks(std::move(tracks)) {
        }

        int resolution() const {
            return m_resolution;
        }

        QList<Tempo> tempos() const {
            return m_tempos;
        }

        QList<TimeSignature> timeSignatures() const {
            return m_timeSignatures;
        }

        QList<Marker> markers() const {
            return m_markers;
        }

        QList<Track> tracks() const {
            return m_tracks;
        }

        bool isValid() const {
            return std::ranges::all_of(std::initializer_list<bool>{
                m_resolution >= 0,
                std::ranges::all_of(m_tempos, [](const Tempo &tempo) {
                    return tempo.tick >= 0 && tempo.tempo >= 10 && tempo.tempo <= 1000;
                }),
                std::ranges::all_of(m_timeSignatures, [](const TimeSignature &timeSignature) {
                    return timeSignature.tick >= 0 && timeSignature.numerator >= 1 && std::ranges::any_of(std::initializer_list<int>{1, 2, 4, 8, 16, 32, 64, 128}, [timeSignature](int v) { return v == timeSignature.denominator; });
                }),
                std::ranges::all_of(m_markers, [](const Marker &marker) {
                    return marker.tick >= 0;
                }),
                std::ranges::all_of(m_tracks, [](const Track &track) {
                    return track.endTick >= 0 && std::ranges::all_of(track.notes, [](const Note &note) {
                        return note.noteOnTick >= 0 && note.length >= 0 && note.key >= 0 && note.key <= 127;
                    });
                })
            }, [](auto v) { return v; });
        }

    private:
        int m_resolution{};
        QList<Tempo> m_tempos;
        QList<TimeSignature> m_timeSignatures;
        QList<Marker> m_markers;
        QList<Track> m_tracks;

    };

}

#endif //OPENDSPX_CONVERTER_MIDIINTERMEDIATEDATA_H
