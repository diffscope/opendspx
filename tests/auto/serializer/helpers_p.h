#ifndef OPENDSPX_TEST_SERIALIZER_HELPERS_P_H
#define OPENDSPX_TEST_SERIALIZER_HELPERS_P_H

#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

#include <stdcorelib/support/json.h>

#include <opendspx/model.h>
#include <opendspx/serializer/serializer.h>

#include <boost/test/unit_test.hpp>

namespace opendspx {

    // So that a failing BOOST_CHECK_EQUAL on an error type prints the code rather than refusing
    // to compile. Found by ADL, which is why it lives in opendspx rather than in the test's own
    // namespace.
    inline std::ostream &operator<<(std::ostream &os, SerializationError::Type type) {
        return os << "0x" << std::hex << static_cast<int>(type) << std::dec;
    }

}

namespace opendspx::test {

    constexpr Serializer::Option defaultOptions = Serializer::FailFast | Serializer::CheckError;

    inline std::string serialize(const Model &model, SerializationErrorList &errors,
                                 Serializer::Option options = defaultOptions,
                                 bool compress = false) {
        std::ostringstream out(std::ios::binary);
        Serializer::serialize(out, model, errors, options, compress);
        return out.str();
    }

    inline Model deserialize(const std::string &text, SerializationErrorList &errors,
                             Serializer::Option options = defaultOptions) {
        std::istringstream in(text, std::ios::binary | std::ios::in);
        return Serializer::deserialize(in, errors, options);
    }

    inline stdc::json::Value parse(std::string_view text) {
        stdc::json::ParseError error;
        auto value = stdc::json::Value::fromJson(text, false, &error);
        BOOST_REQUIRE_MESSAGE(!error, "the test's own JSON does not parse: " << error.message());
        return value;
    }

    // SerializationError has no vtable -- it is discriminated by type() -- so a downcast is
    // checked against that rather than by dynamic_cast.
    template <typename T>
    std::shared_ptr<T> errorAs(const SerializationErrorRef &error) {
        return std::static_pointer_cast<T>(error);
    }

    // Clip, Singer and ParamCurve are the same shape: a base holding a type enum and no virtual
    // anything, so which one an object is has to be read off that enum. Returns null when the
    // object is not the kind asked for, which is what the cases check.
    template <typename Derived, typename Base, typename Type>
    std::shared_ptr<Derived> derivedAs(const std::shared_ptr<Base> &value, Type type) {
        if (!value || value->type != type) {
            return nullptr;
        }
        return std::static_pointer_cast<Derived>(value);
    }

    inline std::string replaceFirst(std::string text, std::string_view from, std::string_view to) {
        const auto at = text.find(from);
        BOOST_REQUIRE_MESSAGE(at != std::string::npos,
                              "the document to mutate does not contain " << from);
        return text.replace(at, from.size(), to);
    }

    // A model with something in every corner the mappings have to reach: both clip kinds, both
    // singer kinds, both parameter curve kinds, a workspace at each level.
    inline Model richModel() {
        Model model;

        model.content.global.name = "song";
        model.content.global.author = "author";
        model.content.global.centShift = -3;
        model.content.global.editorId = "editor";
        model.content.global.editorName = "Editor";

        model.content.master.control.gain = -1.5;
        model.content.master.control.pan = 0.25;
        model.content.master.control.mute = true;

        model.content.timeline.tempos = {Tempo{0, 120.0}, Tempo{1920, 87.5}};
        model.content.timeline.timeSignatures = {TimeSignature{0, 3, 4}, TimeSignature{4, 7, 8}};
        model.content.timeline.labels = {Label{0, "intro"}, Label{1920, "verse"}};
        model.content.workspace["app"] = stdc::json::Object{{"zoom", 1.5}, {"page", 2}};

        Note note;
        note.pos = 0;
        note.length = 480;
        note.keyNum = 60;
        note.centShift = 25;
        note.language = "eng";
        note.lyric = "la";
        note.pronunciation = Pronunciation{"l a", "l a"};
        note.phonemes.original = {Phoneme{"eng", "l", 0, true}, Phoneme{"eng", "a", 100, false}};
        note.phonemes.edited = note.phonemes.original;
        note.vibrato.start = 0.2;
        note.vibrato.end = 0.9;
        note.vibrato.amp = 40;
        note.vibrato.freq = 5.5;
        note.vibrato.phase = 0.125;
        note.vibrato.offset = 1;
        note.vibrato.points.amp = {ControlPoint{0.0, 0.0}, ControlPoint{1.0, 1.0}};
        note.vibrato.points.freq = {ControlPoint{0.0, 1.0}};
        note.workspace["note"] = stdc::json::Object{{"marked", true}};

        auto singing = std::make_shared<SingingClip>();
        singing->name = "vocal";
        singing->time = ClipTime{0, 1920, 0, 1920};
        singing->control.gain = 0.5;
        singing->notes = {note};
        singing->workspace["clip"] = stdc::json::Object{{"colour", "red"}};

        Param pitch;
        pitch.original = {std::make_shared<ParamCurveFree>(0, 5, std::vector<int>{1, 2, 3})};
        pitch.edited = {std::make_shared<ParamCurveAnchor>(
            0, std::vector<AnchorNode>{AnchorNode{AnchorNode::Interpolation::Linear, 0, 6000},
                                       AnchorNode{AnchorNode::Interpolation::Hermite, 480, 6200}})};
        singing->params["pitch"] = pitch;

        Sources sources;
        sources.category = "voice";
        sources.singers = {std::make_shared<SingleSinger>("alice"),
                           std::make_shared<MixedSinger>(
                               std::vector<SingerRef>{std::make_shared<SingleSinger>("bob"),
                                                      std::make_shared<SingleSinger>("carol")},
                               SourceMixingRatio{0.4})};
        sources.mix = {DynamicMixingAnchor{0, SourceMixingRatio{0.5}}};
        singing->sources = sources;

        auto audio = std::make_shared<AudioClip>();
        audio->name = "backing";
        audio->path = "a/b.wav";
        audio->time = ClipTime{0, 3840, 0, 3840};

        Track track;
        track.name = "track";
        track.control.gain = -6.0;
        track.control.pan = -0.5;
        track.control.solo = true;
        track.clips = {singing, audio};
        track.workspace["track"] = stdc::json::Object{{"height", 120}};

        model.content.tracks = {track};
        return model;
    }

}

#endif // OPENDSPX_TEST_SERIALIZER_HELPERS_P_H
