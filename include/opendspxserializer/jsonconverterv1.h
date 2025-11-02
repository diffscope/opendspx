#ifndef OPENDSPX_SERIALIZER_JSONCONVERTERV1_H
#define OPENDSPX_SERIALIZER_JSONCONVERTERV1_H

#include <QJsonValue>

#include <opendspxserializer/serializer.h>

namespace QDspx {

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
    QJsonValue JsonConverterV1::toJson(const AnchorNode &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    AnchorNode JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const AudioClipRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    AudioClipRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const BusControl &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    BusControl JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const ClipRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    ClipRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const ClipTime &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    ClipTime JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Content &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Content JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const ControlPoint &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    ControlPoint JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Global &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Global JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Label &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Label JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const QList<Label> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QList<Label> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Master &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Master JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Model &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Model JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Note &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Note JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const QList<Note> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QList<Note> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Param &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Param JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const ParamCurveRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    ParamCurveRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const ParamCurveAnchorRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    ParamCurveAnchorRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const ParamCurveFreeRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    ParamCurveFreeRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const QList<ParamCurveRef> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QList<ParamCurveRef> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Params &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Params JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Phoneme &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Phoneme JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const QList<Phoneme> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QList<Phoneme> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Phonemes &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Phonemes JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Pronunciation &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Pronunciation JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const SingingClipRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    SingingClipRef JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Sources &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Sources JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Tempo &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Tempo JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const QList<Tempo> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QList<Tempo> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const TimeSignature &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    TimeSignature JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const QList<TimeSignature> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QList<TimeSignature> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Timeline &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Timeline JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Track &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Track JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const QList<Track> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QList<Track> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const TrackControl &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    TrackControl JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const Vibrato &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    Vibrato JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const VibratoPoints &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    VibratoPoints JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const QList<ControlPoint> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QList<ControlPoint> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const QList<AnchorNode> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QList<AnchorNode> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QJsonValue JsonConverterV1::toJson(const QList<ClipRef> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path);

    template <>
    QList<ClipRef> JsonConverterV1::fromJson(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path);

}

#endif //OPENDSPX_SERIALIZER_JSONCONVERTERV1_H
