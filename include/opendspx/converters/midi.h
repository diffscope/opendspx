#ifndef OPENDSPX_MIDI_H
#define OPENDSPX_MIDI_H

#include <opendspx/converter.h>

#include <utility>

namespace QDspx {

    class OPENDSPX_EXPORT MidiConverter : public Converter {
    public:
        enum Error {
            NoTrack = Result::User + 1,
            UnsupportedType,
            NoteEventInconsistent,
        };

        struct TrackInfo {
            QByteArray title;
            QList<QByteArray> lyrics;
            bool selectable;
            int channelIndex;
            int noteCount;
            QString keyRange;
            TrackInfo() : TrackInfo({}, {}){};
            TrackInfo(QByteArray title, const QList<QByteArray> &lyrics)
                : title(std::move(title)), lyrics(lyrics), selectable(true), channelIndex(0),
                  noteCount(0) {
            }
        };

        // selector: bool (const QList<TrackInfo> &, const QList<QByteArray> &, QList<int> *,
        // QTextCodec **);
        Result load(const QString &path, Model *out, const QVariantMap &args) override;

        // overlapHandler: bool ();
        Result save(const QString &path, const Model &in, const QVariantMap &args) override;
    };

}

#endif // OPENDSPX_MIDI_H
