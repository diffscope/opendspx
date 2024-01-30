#ifndef OPENDSPX_MIDI_H
#define OPENDSPX_MIDI_H

#include <opendspx/converter.h>

namespace QDspx {

    class OPENDSPX_EXPORT MidiConverter : public Converter {
    public:
        ReturnCode load(const QString &path, Model *out, const QVariantMap &args) override;
        ReturnCode save(const QString &path, const Model &in, const QVariantMap &args) override;
    };

}

#endif // OPENDSPX_MIDI_H
