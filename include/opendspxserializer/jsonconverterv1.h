#ifndef OPENDSPX_SERIALIZER_JSONCONVERTERV1_H
#define OPENDSPX_SERIALIZER_JSONCONVERTERV1_H

#include <QJsonValue>

#include <opendspxserializer/serializer.h>

namespace opendspx {

    struct AnchorNode;
    struct AudioClip;
    using AudioClipRef = QSharedPointer<AudioClip>;
    struct BusControl;
    struct Clip;
    using ClipRef = QSharedPointer<Clip>;
    struct ClipTime;
    struct Content;
    struct ControlPoint;
    struct Global;
    class Label;
    struct Master;
    struct Model;
    struct Note;
    struct Param;
    struct ParamCurve;
    using ParamCurveRef = QSharedPointer<ParamCurve>;
    struct ParamCurveAnchor;
    using ParamCurveAnchorRef = QSharedPointer<ParamCurveAnchor>;
    struct ParamCurveFree;
    using ParamCurveFreeRef = QSharedPointer<ParamCurveFree>;
    using Params = QMap<QString, Param>;
    struct Phoneme;
    struct Phonemes;
    struct Pronunciation;
    struct SingingClip;
    using SingingClipRef = QSharedPointer<SingingClip>;
    using Sources = QMap<QString, QJsonObject>;
    struct Tempo;
    struct TimeSignature;
    struct Timeline;
    struct Track;
    struct TrackControl;
    struct Vibrato;
    struct VibratoPoints;
    using Workspace = QMap<QString, QJsonObject>;

    class OPENDSPX_SERIALIZER_EXPORT JsonConverterV1 {
    public:
        template <typename T>
        static QJsonValue toJson(const T &entity, SerializationErrorList &errors, Serializer::Option options = {Serializer::FailFast | Serializer::CheckError}, const QString &path = {"$"}) {
            static_assert(sizeof(T) == 0, "JsonConverterV1::toJson: unsupported type");
        }

        template <typename T>
        static T fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options = {Serializer::FailFast | Serializer::CheckError}, const QString &path = {"$"}) {
            static_assert(sizeof(T) == 0, "JsonConverterV1::fromJson: unsupported type");
        }

    };

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const AnchorNode &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT AnchorNode JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const AudioClipRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT AudioClipRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const BusControl &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT BusControl JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const ClipRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT ClipRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const ClipTime &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT ClipTime JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Content &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Content JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const ControlPoint &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT ControlPoint JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Global &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Global JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Label &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Label JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const QList<Label> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QList<Label> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Master &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Master JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Model &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Model JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Note &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Note JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const QList<Note> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QList<Note> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Param &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Param JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const ParamCurveRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT ParamCurveRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const ParamCurveAnchorRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT ParamCurveAnchorRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const ParamCurveFreeRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT ParamCurveFreeRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const QList<ParamCurveRef> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QList<ParamCurveRef> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Params &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Params JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Phoneme &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Phoneme JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const QList<Phoneme> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QList<Phoneme> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Phonemes &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Phonemes JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Pronunciation &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Pronunciation JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const SingingClipRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT SingingClipRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Sources &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Sources JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Tempo &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Tempo JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const QList<Tempo> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QList<Tempo> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const TimeSignature &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT TimeSignature JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const QList<TimeSignature> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QList<TimeSignature> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Timeline &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Timeline JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Track &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Track JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const QList<Track> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QList<Track> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const TrackControl &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT TrackControl JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const Vibrato &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT Vibrato JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const VibratoPoints &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT VibratoPoints JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const QList<ControlPoint> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QList<ControlPoint> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const QList<AnchorNode> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QList<AnchorNode> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QJsonValue JsonConverterV1::toJson(const QList<ClipRef> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    OPENDSPX_SERIALIZER_EXPORT QList<ClipRef> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

}

#endif //OPENDSPX_SERIALIZER_JSONCONVERTERV1_H
