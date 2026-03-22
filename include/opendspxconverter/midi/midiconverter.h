#ifndef OPENDSPX_CONVERTER_MIDICONVERTER_H
#define OPENDSPX_CONVERTER_MIDICONVERTER_H

#include <functional>

#include <opendspxconverter/opendspxconverterglobal.h>

namespace opendspx {

    struct Model;
    class MidiIntermediateData;

    class OPENDSPX_CONVERTER_EXPORT MidiConverter {
    public:

        enum class Error {
            NoError,
            InvalidMidiData,
            UnsupportedFormat,
        };

        struct ConvertMidiToIntermediateOption {
            bool separateChannels = true;
        };
        static MidiIntermediateData convertMidiToIntermediate(const QByteArray &midiData, Error &error, ConvertMidiToIntermediateOption option = {true});

        static Model convertIntermediateToDspx(const MidiIntermediateData &midiData, bool *ok = nullptr);
        static Model convertIntermediateToDspx(const MidiIntermediateData &midiData, const std::function<QString(const QByteArray &)> &decodeText, bool *ok = nullptr);

        struct ConvertDspxToIntermediateOption {
            int resolution = 480;
            bool separateClips = false;
        };
        static MidiIntermediateData convertDspxToIntermediate(const Model &model, ConvertDspxToIntermediateOption option = {480, false});
        static MidiIntermediateData convertDspxToIntermediate(const Model &model, const std::function<QByteArray(const QString &)> &encodeText, ConvertDspxToIntermediateOption option = {480, false});

        static QByteArray convertIntermediateToMidi(const MidiIntermediateData &midiData);

    };

}

#endif //OPENDSPX_CONVERTER_MIDICONVERTER_H
