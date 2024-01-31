#ifndef OPENDSPX_MIDI_H
#define OPENDSPX_MIDI_H

#include <opendspx/converter.h>

namespace QDspx {

    class OPENDSPX_EXPORT MidiConverter : public Converter {
    public:
        enum Result {
            NoTrack = ReturnCode::User + 1,
            UnsupportedType,
        };

        struct TrackInfo {
            QByteArray title;
            QList<QByteArray> lyrics;
            QString format; // Display as format.arg(title)
            bool selectable;
            TrackInfo() : TrackInfo({}, {}){};
            TrackInfo(const QByteArray &title, const QList<QByteArray> &lyrics)
                : title(title), lyrics(lyrics), format(QStringLiteral("%1")), selectable(true){};
        };

        // selector: bool (const QList<TrackInfo> &, const QList<QByteArray> &, QList<int> *, QTextCodec **);
        ReturnCode load(const QString &path, Model *out, const QVariantMap &args) override;

        // overlapHandler: bool ();
        ReturnCode save(const QString &path, const Model &in, const QVariantMap &args) override;
    };

}

#endif // OPENDSPX_MIDI_H
