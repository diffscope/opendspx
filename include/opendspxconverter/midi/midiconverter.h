#ifndef OPENDSPX_CONVERTER_MIDICONVERTER_H
#define OPENDSPX_CONVERTER_MIDICONVERTER_H

#include <iosfwd>
#include <functional>
#include <string>

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
        static MidiIntermediateData convertMidiToIntermediate(std::istream &in, Error &error, ConvertMidiToIntermediateOption option = {true});

        static Model convertIntermediateToDspx(const MidiIntermediateData &midiData, bool *ok = nullptr);

        static Model convertIntermediateToDspx(const MidiIntermediateData &midiData, const std::function<std::string(const std::string &)> &decodeText, bool *ok = nullptr);

        struct ConvertDspxToIntermediateOption {
            int resolution = 480;
            bool separateClips = false;
        };
        static MidiIntermediateData convertDspxToIntermediate(const Model &model, ConvertDspxToIntermediateOption option = {480, false});
        static MidiIntermediateData convertDspxToIntermediate(const Model &model, const std::function<std::string(const std::string &)> &encodeText, ConvertDspxToIntermediateOption option = {480, false});

        static void convertIntermediateToMidi(std::ostream &out, const MidiIntermediateData &midiData);

    };

}

#endif //OPENDSPX_CONVERTER_MIDICONVERTER_H
