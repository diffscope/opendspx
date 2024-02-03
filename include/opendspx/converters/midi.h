#ifndef OPENDSPX_MIDI_H
#define OPENDSPX_MIDI_H

#include <opendspx/converter.h>

namespace QDspx {

    class OPENDSPX_EXPORT MidiConverter : public Converter {
    public:
        enum Result {
            NoTrack = ReturnCode::User + 1,
            UnsupportedType,
            NoteEventInconsistent,
        };

        struct TrackInfo {
            QByteArray title;
            QList<QByteArray> lyrics;
            bool selectable;
            int channelIndex;
            int noteCount;
            QPair<int, int> keyRange;
            TrackInfo() : TrackInfo({}, {}){};
            TrackInfo(const QByteArray &title, const QList<QByteArray> &lyrics)
                : title(title), lyrics(lyrics), selectable(true), channelIndex(0), noteCount(0){};
        };

        // selector: bool (const QList<TrackInfo> &, const QList<QByteArray> &, QList<int> *,
        // QTextCodec **);
        ReturnCode load(const QString &path, Model *out, const QVariantMap &args) override;

        // overlapHandler: bool ();
        ReturnCode save(const QString &path, const Model &in, const QVariantMap &args) override;
    };

}

#endif // OPENDSPX_MIDI_H
