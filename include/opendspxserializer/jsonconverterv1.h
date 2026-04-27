#ifndef OPENDSPX_SERIALIZER_JSONCONVERTERV1_H
#define OPENDSPX_SERIALIZER_JSONCONVERTERV1_H

#include <nlohmann/json.hpp>

#include <opendspx/model.h>
#include <opendspxserializer/serializer.h>
#include <opendspxserializer/private/declarative_p.h>

namespace opendspx {

    namespace impl {
        using INTERP_ENUM_DEFS = decl::EnumDef<const char *, AnchorNode::Interpolation,
            decl::EnumEntry<decl::FixedString("none"), AnchorNode::Interpolation::None>,
            decl::EnumEntry<decl::FixedString("linear"), AnchorNode::Interpolation::Linear>,
            decl::EnumEntry<decl::FixedString("hermite"), AnchorNode::Interpolation::Hermite>
        >;

        using CLIP_AUDIO_TYPE_ENUM_DEFS = decl::EnumDef<const char *, Clip::Type,
            decl::EnumEntry<decl::FixedString("audio"), Clip::Type::Audio>
        >;

        using CLIP_SINGING_TYPE_ENUM_DEFS = decl::EnumDef<const char *, Clip::Type,
            decl::EnumEntry<decl::FixedString("singing"), Clip::Type::Singing>
        >;

        using PARAM_CURVE_ANCHOR_TYPE_ENUM_DEFS = decl::EnumDef<const char *, ParamCurve::Type,
            decl::EnumEntry<decl::FixedString("anchor"), ParamCurve::Type::Anchor>
        >;

        using PARAM_CURVE_FREE_TYPE_ENUM_DEFS = decl::EnumDef<const char *, ParamCurve::Type,
            decl::EnumEntry<decl::FixedString("free"), ParamCurve::Type::Free>
        >;

        using MODEL_VERSION_ENUM_DEFS = decl::EnumDef<const char *, Model::Version,
            decl::EnumEntry<decl::FixedString("1.0.0"), Model::Version::V1>
        >;

        using DENOMINATOR_ENUM_DEFS = decl::EnumDef<int, int,
            decl::EnumEntry<1, 1>,
            decl::EnumEntry<2, 2>,
            decl::EnumEntry<4, 4>,
            decl::EnumEntry<8, 8>,
            decl::EnumEntry<16, 16>,
            decl::EnumEntry<32, 32>,
            decl::EnumEntry<64, 64>,
            decl::EnumEntry<128, 128>
        >;

        template<>
        struct decl::Mapping<AnchorNode> {
            using type = Entity<AnchorNode,
                Property<&AnchorNode::x, "x", RangeConstraintConvert<0>>,
                Property<&AnchorNode::y, "y">,
                Property<&AnchorNode::interp, "interp", EnumConstraintConvert<INTERP_ENUM_DEFS>>
            >;
        };

        template<>
        struct decl::Mapping<AudioClip> {
            using type = Entity<AudioClip,
                Property<&AudioClip::control, "control">,
                Property<&AudioClip::name, "name">,
                Property<&AudioClip::path, "path">,
                Property<&AudioClip::time, "time">,
                Property<&AudioClip::type, "type", EnumConstraintConvert<CLIP_AUDIO_TYPE_ENUM_DEFS>>,
                Property<&AudioClip::workspace, "workspace">
            >;
        };

        template<>
        struct decl::Mapping<BusControl> {
            using type = Entity<BusControl,
                Property<&BusControl::gain, "gain">,
                Property<&BusControl::mute, "mute">,
                Property<&BusControl::pan, "pan", RangeConstraintConvert<-1, 1>>
            >;
        };

        template<>
        struct decl::Mapping<Clip> {
            using type = BaseEntity<Clip, &Clip::type, "type",
                Derive<Clip::Type::Audio, "audio", AudioClip>,
                Derive<Clip::Type::Singing, "singing", SingingClip>
            >;
        };

        template<>
        struct decl::Mapping<ClipTime> {
            using type = Entity<ClipTime,
                Property<&ClipTime::clipLen, "clipLen", RangeConstraintConvert<0>>,
                Property<&ClipTime::clipStart, "clipStart", RangeConstraintConvert<0>>,
                Property<&ClipTime::length, "length", RangeConstraintConvert<0>>,
                Property<&ClipTime::pos, "pos", RangeConstraintConvert<0>>
            >;
        };

        template<>
        struct decl::Mapping<Content> {
            using type = Entity<Content,
                Property<&Content::global, "global">,
                Property<&Content::master, "master">,
                Property<&Content::timeline, "timeline">,
                Property<&Content::tracks, "tracks">,
                Property<&Content::workspace, "workspace">
            >;
        };

        template<>
        struct decl::Mapping<ControlPoint> {
            using type = Entity<ControlPoint,
                Property<&ControlPoint::x, "x">,
                Property<&ControlPoint::y, "y">
            >;
        };

        template<>
        struct decl::Mapping<Global> {
            using type = Entity<Global,
                Property<&Global::author, "author">,
                Property<&Global::name, "name">,
                Property<&Global::centShift, "centShift", RangeConstraintConvert<-50, 50>>,
                Property<&Global::editorId, "editorId">,
                Property<&Global::editorName, "editorName">
            >;
        };

        template<>
        struct decl::Mapping<Label> {
            using type = Entity<Label,
                Property<&Label::pos, "pos", RangeConstraintConvert<0>>,
                Property<&Label::text, "text">
            >;
        };

        template<>
        struct decl::Mapping<Master> {
            using type = Entity<Master,
                Property<&Master::control, "control">
            >;
        };

        template<>
        struct decl::Mapping<Model> {
            using type = Entity<Model,
                Property<&Model::version, "version", EnumConstraintConvert<MODEL_VERSION_ENUM_DEFS>>,
                Property<&Model::content, "content">
            >;
        };

        template<>
        struct decl::Mapping<Note> {
            using type = Entity<Note,
                Property<&Note::pos, "pos", RangeConstraintConvert<0>>,
                Property<&Note::length, "length", RangeConstraintConvert<0>>,
                Property<&Note::keyNum, "keyNum", RangeConstraintConvert<0, 127>>,
                Property<&Note::centShift, "centShift", RangeConstraintConvert<-50, 50>>,
                Property<&Note::language, "language">,
                Property<&Note::lyric, "lyric">,
                Property<&Note::pronunciation, "pronunciation">,
                Property<&Note::phonemes, "phonemes">,
                Property<&Note::vibrato, "vibrato">,
                Property<&Note::workspace, "workspace">
            >;
        };

        template<>
        struct decl::Mapping<Param> {
            using type = Entity<Param,
                Property<&Param::original, "original">,
                Property<&Param::transform, "transform">,
                Property<&Param::edited, "edited">
            >;
        };

        template<>
        struct decl::Mapping<Params> {
            using type = MapEntity<Params>;
        };

        template<>
        struct decl::Mapping<ParamCurve> {
            using type = BaseEntity<ParamCurve, &ParamCurve::type, "type",
                Derive<ParamCurve::Type::Anchor, "anchor", ParamCurveAnchor>,
                Derive<ParamCurve::Type::Free, "free", ParamCurveFree>
            >;
        };

        template<>
        struct decl::Mapping<ParamCurveAnchor> {
            using type = Entity<ParamCurveAnchor,
                Property<&ParamCurveAnchor::start, "start">,
                Property<&ParamCurveAnchor::nodes, "nodes">,
                Property<&ParamCurveAnchor::type, "type", EnumConstraintConvert<PARAM_CURVE_ANCHOR_TYPE_ENUM_DEFS>>
            >;
        };

        template<>
        struct decl::Mapping<ParamCurveFree> {
            using type = Entity<ParamCurveFree,
                Property<&ParamCurveFree::start, "start">,
                Property<&ParamCurveFree::step, "step", RangeConstraintConvert<5, 5>>,
                Property<&ParamCurveFree::values, "values", ArrayRangeConstraintConvert<>>,
                Property<&ParamCurveFree::type, "type", EnumConstraintConvert<PARAM_CURVE_FREE_TYPE_ENUM_DEFS>>
            >;
        };

        template<>
        struct decl::Mapping<Phoneme> {
            using type = Entity<Phoneme,
                Property<&Phoneme::language, "language">,
                Property<&Phoneme::token, "token">,
                Property<&Phoneme::start, "start">,
                Property<&Phoneme::onset, "onset">
            >;
        };

        template<>
        struct decl::Mapping<Phonemes> {
            using type = Entity<Phonemes,
                Property<&Phonemes::original, "original">,
                Property<&Phonemes::edited, "edited">
            >;
        };

        template<>
        struct decl::Mapping<Pronunciation> {
            using type = Entity<Pronunciation,
                Property<&Pronunciation::original, "original">,
                Property<&Pronunciation::edited, "edited">
            >;
        };

        template<>
        struct decl::Mapping<SingingClip> {
            using type = Entity<SingingClip,
                Property<&SingingClip::control, "control">,
                Property<&SingingClip::name, "name">,
                Property<&SingingClip::notes, "notes">,
                Property<&SingingClip::params, "params">,
                Property<&SingingClip::sources, "sources">,
                Property<&SingingClip::time, "time">,
                Property<&SingingClip::type, "type", EnumConstraintConvert<CLIP_SINGING_TYPE_ENUM_DEFS>>,
                Property<&SingingClip::workspace, "workspace">
            >;
        };

        template<>
        struct decl::Mapping<Sources> {
            using type = MapEntity<Sources>;
        };

        template<>
        struct decl::Mapping<Tempo> {
            using type = Entity<Tempo,
                Property<&Tempo::pos, "pos", RangeConstraintConvert<0>>,
                Property<&Tempo::value, "value", RangeConstraintConvert<10, 1000>>
            >;
        };

        template<>
        struct decl::Mapping<TimeSignature> {
            using type = Entity<TimeSignature,
                Property<&TimeSignature::index, "index", RangeConstraintConvert<0>>,
                Property<&TimeSignature::numerator, "numerator", RangeConstraintConvert<1>>,
                Property<&TimeSignature::denominator, "denominator", EnumConstraintConvert<DENOMINATOR_ENUM_DEFS>>
            >;
        };

        template<>
        struct decl::Mapping<Timeline> {
            using type = Entity<Timeline,
                Property<&Timeline::labels, "labels">,
                Property<&Timeline::tempos, "tempos">,
                Property<&Timeline::timeSignatures, "timeSignatures">
            >;
        };

        template<>
        struct decl::Mapping<Track> {
            using type = Entity<Track,
                Property<&Track::name, "name">,
                Property<&Track::control, "control">,
                Property<&Track::clips, "clips">,
                Property<&Track::workspace, "workspace">
            >;
        };

        template<>
        struct decl::Mapping<TrackControl> {
            using type = Entity<TrackControl,
                Property<&TrackControl::gain, "gain">,
                Property<&TrackControl::pan, "pan", RangeConstraintConvert<-1, 1>>,
                Property<&TrackControl::mute, "mute">,
                Property<&TrackControl::solo, "solo">
            >;
        };

        template<>
        struct decl::Mapping<Vibrato> {
            using type = Entity<Vibrato,
                Property<&Vibrato::start, "start", RangeConstraintConvert<0, 1>>,
                Property<&Vibrato::end, "end", RangeConstraintConvert<0, 1>>,
                Property<&Vibrato::amp, "amp", RangeConstraintConvert<0>>,
                Property<&Vibrato::freq, "freq", RangeConstraintConvert<0>>,
                Property<&Vibrato::phase, "phase", RangeConstraintConvert<0, 1>>,
                Property<&Vibrato::offset, "offset">,
                Property<&Vibrato::points, "points">
            >;
        };

        template<>
        struct decl::Mapping<VibratoPoints> {
            using type = Entity<VibratoPoints,
                Property<&VibratoPoints::amp, "amp">,
                Property<&VibratoPoints::freq, "freq">
            >;
        };

        template<>
        struct decl::Mapping<Workspace> {
            using type = MapEntity<Workspace>;
        };

    }

    class OPENDSPX_SERIALIZER_EXPORT JsonConverterV1 {
    public:
        template <typename T>
        static nlohmann::json toJson(const T &entity, SerializationErrorList &errors, Serializer::Option options = {Serializer::FailFast | Serializer::CheckError}, const std::string &path = {"$"}) {
            nlohmann::json json;
            impl::decl::TrivialOrMappingConvert::getToJsonFunc<T>()(json, entity, impl::JsonSerializationContext{errors, options, path});
            return json;
        }

        template <typename T>
        static T fromJson(const nlohmann::json &json, SerializationErrorList &errors, Serializer::Option options = {Serializer::FailFast | Serializer::CheckError}, const std::string &path = {"$"}) {
            T entity;
            impl::decl::TrivialOrMappingConvert::getFromJsonFunc<T>()(json, entity, impl::JsonSerializationContext{errors, options, path});
            return entity;
        }

    };

}

#endif //OPENDSPX_SERIALIZER_JSONCONVERTERV1_H
