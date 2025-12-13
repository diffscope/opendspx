#include "jsonconverterv1.h"

#include <QJsonObject>
#include <QJsonArray>

#include <opendspx/model.h>
#include <opendspxserializer/serializationerror.h>

#include "converterhelperfromjson_p.h"
#include "converterhelpertojson_p.h"

namespace QDspx{

    constexpr auto INTERP_ENUM_DEFS = std::array{
        std::pair{"none", AnchorNode::None},
        std::pair{"linear", AnchorNode::Linear},
        std::pair{"hermite", AnchorNode::Hermite}
    };

    template <>
    QJsonValue JsonConverterV1::toJson<AnchorNode>(const AnchorNode &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., x, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., y, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., interp, toJsonEnumHelper(INTERP_ENUM_DEFS), object);
        return object;
    }

    template <>
    AnchorNode JsonConverterV1::fromJson<AnchorNode>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"x", "y", "interp"}), {});
        AnchorNode node;
        FROM_JSON_PROPERTY_FAIL_FAST(node., x, fromJsonIntHelperWithConstraint(0), node);
        FROM_JSON_PROPERTY_FAIL_FAST(node., y, fromJsonIntHelper, node);
        FROM_JSON_PROPERTY_FAIL_FAST(node., interp, fromJsonEnumHelper(INTERP_ENUM_DEFS), node);
        return node;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QSharedPointer<AudioClip>>(const AudioClipRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("type", "audio");
        TO_JSON_PROPERTY_FAIL_FAST(entity->, name, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, control, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, time, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, path, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, workspace, toJson, object);
        return object;
    }

    template <>
    AudioClipRef JsonConverterV1::fromJson<QSharedPointer<AudioClip>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"type", "name", "control", "time", "path", "workspace"}), {});
        auto clip = QSharedPointer<AudioClip>::create();
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, name, fromJsonStringHelper, clip);
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, control, fromJson<BusControl>, clip);
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, time, fromJson<ClipTime>, clip);
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, path, fromJsonStringHelper, clip);
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, workspace, fromJson<Workspace>, clip);
        return clip;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<BusControl>(const BusControl &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., gain, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., pan, toJsonNumberHelperWithConstraint(-1, 1), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., mute, toJsonTrivial, object);
        return object;
    }

    template <>
    BusControl JsonConverterV1::fromJson<BusControl>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"gain", "pan", "mute"}), {});
        BusControl control;
        FROM_JSON_PROPERTY_FAIL_FAST(control., gain, fromJsonDoubleHelperWithConstraint(0), control);
        FROM_JSON_PROPERTY_FAIL_FAST(control., pan, fromJsonDoubleHelperWithConstraint(-1, 1), control);
        FROM_JSON_PROPERTY_FAIL_FAST(control., mute, fromJsonBoolHelper, control);
        return control;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<ClipTime>(const ClipTime &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., start, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., length, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., clipStart, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., clipLen, toJsonNumberHelperWithConstraint(0), object);
        return object;
    }

    template <>
    ClipTime JsonConverterV1::fromJson<ClipTime>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"start", "length", "clipStart", "clipLen"}), {});
        ClipTime time;
        FROM_JSON_PROPERTY_FAIL_FAST(time., start, fromJsonIntHelper, time);
        FROM_JSON_PROPERTY_FAIL_FAST(time., length, fromJsonIntHelperWithConstraint(0), time);
        FROM_JSON_PROPERTY_FAIL_FAST(time., clipStart, fromJsonIntHelperWithConstraint(0), time);
        FROM_JSON_PROPERTY_FAIL_FAST(time., clipLen, fromJsonIntHelperWithConstraint(0), time);
        return time;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<ControlPoint>(const ControlPoint &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., x, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., y, toJsonTrivial, object);
        return object;
    }

    template <>
    ControlPoint JsonConverterV1::fromJson<ControlPoint>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"x", "y"}), {});
        ControlPoint point;
        FROM_JSON_PROPERTY_FAIL_FAST(point., x, fromJsonDoubleHelper, point);
        FROM_JSON_PROPERTY_FAIL_FAST(point., y, fromJsonDoubleHelper, point);
        return point;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QSharedPointer<Clip>>(const ClipRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        switch (entity->type) {
            case Clip::Type::Audio:
                return toJson<QSharedPointer<AudioClip>>(entity.staticCast<AudioClip>(), errors, options, path);
            case Clip::Type::Singing:
                return toJson<QSharedPointer<SingingClip>>(entity.staticCast<SingingClip>(), errors, options, path);
            default: {
                Q_UNREACHABLE();
            }
        }
    }

    template <>
    ClipRef JsonConverterV1::fromJson<QSharedPointer<Clip>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelper, {});
        auto type = object.value("type").toString();
        if (type == "audio") {
            return fromJson<QSharedPointer<AudioClip>>(json, errors, options, path);
        }
        if (type == "singing") {
            return fromJson<QSharedPointer<SingingClip>>(json, errors, options, path);
        }
        errors.addError<InvalidObjectTypeError>(path, type, QStringList{"audio", "singing"});
        return {};
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Content>(const Content &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., global, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., master, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., timeline, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., tracks, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., workspace, toJson, object);
        return object;
    }

    template <>
    Content JsonConverterV1::fromJson<Content>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"global", "master", "timeline", "tracks", "workspace"}), {});
        Content content;
        FROM_JSON_PROPERTY_FAIL_FAST(content., global, fromJson<Global>, content);
        FROM_JSON_PROPERTY_FAIL_FAST(content., master, fromJson<Master>, content);
        FROM_JSON_PROPERTY_FAIL_FAST(content., timeline, fromJson<Timeline>, content);
        FROM_JSON_PROPERTY_FAIL_FAST(content., tracks, fromJson<QList<Track>>, content);
        FROM_JSON_PROPERTY_FAIL_FAST(content., workspace, fromJson<Workspace>, content);
        return content;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Global>(const Global &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., author, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., name, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., centShift, toJsonNumberHelperWithConstraint(-50, 50), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., editorId, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., editorName, toJsonTrivial, object);
        return object;
    }

    template <>
    Global JsonConverterV1::fromJson<Global>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"author", "name", "centShift", "editorId", "editorName"}), {});
        Global global;
        FROM_JSON_PROPERTY_FAIL_FAST(global., author, fromJsonStringHelper, global);
        FROM_JSON_PROPERTY_FAIL_FAST(global., name, fromJsonStringHelper, global);
        FROM_JSON_PROPERTY_FAIL_FAST(global., centShift, fromJsonIntHelperWithConstraint(-50, 50), global);
        FROM_JSON_PROPERTY_FAIL_FAST(global., editorId, fromJsonStringHelper, global);
        FROM_JSON_PROPERTY_FAIL_FAST(global., editorName, fromJsonStringHelper, global);
        return global;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Label>(const Label &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., pos, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., text, toJsonTrivial, object);
        return object;
    }

    template <>
    Label JsonConverterV1::fromJson<Label>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"pos", "text"}), {});
        Label label;
        FROM_JSON_PROPERTY_FAIL_FAST(label., pos, fromJsonIntHelperWithConstraint(0), label);
        FROM_JSON_PROPERTY_FAIL_FAST(label., text, fromJsonStringHelper, label);
        return label;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<Label>>(const QList<Label> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper<Label, JsonConverterV1>(entity, errors, options, path);
    }

    template <>
    QList<Label> JsonConverterV1::fromJson<QList<Label>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<Label, JsonConverterV1>(json, errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Master>(const Master &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., control, toJson, object);
        return object;
    }

    template <>
    Master JsonConverterV1::fromJson<Master>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"control"}), {});
        Master master;
        FROM_JSON_PROPERTY_FAIL_FAST(master., control, fromJson<BusControl>, master);
        return master;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Model>(const Model &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("version", "1.0.0");
        TO_JSON_PROPERTY_FAIL_FAST(entity., content, toJson, object);
        return object;
    }

    template <>
    Model JsonConverterV1::fromJson<Model>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"version", "content"}), {});
        Model model;
        model.version = Model::V1;
        FROM_JSON_PROPERTY_FAIL_FAST(model., content, fromJson<Content>, model);
        return model;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Note>(const Note &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., pos, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., length, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., keyNum, toJsonNumberHelperWithConstraint(0, 127), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., centShift, toJsonNumberHelperWithConstraint(-50, 50), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., language, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., lyric, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., pronunciation, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., phonemes, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., vibrato, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., workspace, toJson, object);
        return object;
    }

    template <>
    Note JsonConverterV1::fromJson<Note>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"pos", "length", "keyNum", "centShift", "language", "lyric", "pronunciation", "phonemes", "vibrato", "workspace"}), {});
        Note note;
        FROM_JSON_PROPERTY_FAIL_FAST(note., pos, fromJsonIntHelperWithConstraint(0), note);
        FROM_JSON_PROPERTY_FAIL_FAST(note., length, fromJsonIntHelperWithConstraint(0), note);
        FROM_JSON_PROPERTY_FAIL_FAST(note., keyNum, fromJsonIntHelperWithConstraint(0, 127), note);
        FROM_JSON_PROPERTY_FAIL_FAST(note., centShift, fromJsonIntHelperWithConstraint(-50, 50), note);
        FROM_JSON_PROPERTY_FAIL_FAST(note., language, fromJsonStringHelper, note);
        FROM_JSON_PROPERTY_FAIL_FAST(note., lyric, fromJsonStringHelper, note);
        FROM_JSON_PROPERTY_FAIL_FAST(note., pronunciation, fromJson<Pronunciation>, note);
        FROM_JSON_PROPERTY_FAIL_FAST(note., phonemes, fromJson<Phonemes>, note);
        FROM_JSON_PROPERTY_FAIL_FAST(note., vibrato, fromJson<Vibrato>, note);
        FROM_JSON_PROPERTY_FAIL_FAST(note., workspace, fromJson<Workspace>, note);
        return note;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<Note>>(const QList<Note> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper<Note, JsonConverterV1>(entity, errors, options, path);
    }

    template <>
    QList<Note> JsonConverterV1::fromJson<QList<Note>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<Note, JsonConverterV1>(json, errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Param>(const Param &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., original, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., transform, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., edited, toJson, object);
        return object;
    }

    template <>
    Param JsonConverterV1::fromJson<Param>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"original", "transform", "edited"}), {});
        Param param;
        FROM_JSON_PROPERTY_FAIL_FAST(param., original, fromJson<QList<ParamCurveRef>>, param);
        FROM_JSON_PROPERTY_FAIL_FAST(param., transform, fromJson<QList<ParamCurveRef>>, param);
        FROM_JSON_PROPERTY_FAIL_FAST(param., edited, fromJson<QList<ParamCurveRef>>, param);
        return param;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QSharedPointer<ParamCurve>>(const ParamCurveRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        switch (entity->type) {
            case ParamCurve::Anchor:
                return toJson<QSharedPointer<ParamCurveAnchor>>(entity.staticCast<ParamCurveAnchor>(), errors, options, path);
            case ParamCurve::Free:
                return toJson<QSharedPointer<ParamCurveFree>>(entity.staticCast<ParamCurveFree>(), errors, options, path);
            default: {
                Q_UNREACHABLE();
            }
        }
    }

    template <>
    ParamCurveRef JsonConverterV1::fromJson<QSharedPointer<ParamCurve>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelper, {});
        auto type = object.value("type").toString();
        if (type == "anchor") {
            return fromJson<QSharedPointer<ParamCurveAnchor>>(json, errors, options, path);
        }
        if (type == "free") {
            return fromJson<QSharedPointer<ParamCurveFree>>(json, errors, options, path);
        }
        errors.addError<InvalidObjectTypeError>(path, type, QStringList{"anchor", "free"});
        return {};
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QSharedPointer<ParamCurveAnchor>>(const ParamCurveAnchorRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("type", "anchor");
        TO_JSON_PROPERTY_FAIL_FAST(entity->, start, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, nodes, toJson, object);
        return object;
    }

    template <>
    ParamCurveAnchorRef JsonConverterV1::fromJson<QSharedPointer<ParamCurveAnchor>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"type", "start", "nodes"}), {});
        auto curve = QSharedPointer<ParamCurveAnchor>::create();
        FROM_JSON_PROPERTY_FAIL_FAST(curve->, start, fromJsonIntHelper, curve);
        FROM_JSON_PROPERTY_FAIL_FAST(curve->, nodes, fromJson<QList<AnchorNode>>, curve);
        return curve;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QSharedPointer<ParamCurveFree>>(const ParamCurveFreeRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("type", "free");
        TO_JSON_PROPERTY_FAIL_FAST(entity->, start, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, step, toJsonNumberHelperWithConstraint(5, 5), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, values, toJsonArrayHelper, object);
        return object;
    }

    template <>
    ParamCurveFreeRef JsonConverterV1::fromJson<QSharedPointer<ParamCurveFree>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"type", "start", "step", "values"}), {});
        auto curve = QSharedPointer<ParamCurveFree>::create();
        FROM_JSON_PROPERTY_FAIL_FAST(curve->, start, fromJsonIntHelper, curve);
        FROM_JSON_PROPERTY_FAIL_FAST(curve->, step, fromJsonIntHelperWithConstraint(5, 5), curve);
        FROM_JSON_PROPERTY_FAIL_FAST(curve->, values, fromJsonArrayHelper<int>, curve);
        return curve;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<ParamCurveRef>>(const QList<ParamCurveRef> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper<ParamCurveRef, JsonConverterV1>(entity, errors, options, path);
    }

    template <>
    QList<ParamCurveRef> JsonConverterV1::fromJson<QList<ParamCurveRef>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<ParamCurveRef, JsonConverterV1>(json, errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Params>(const Params &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        for (const auto &[key, value] : entity.asKeyValueRange()) {
            object.insert(key, toJson<Param>(value, errors, options, path + "." + key));
        }
        return object;
    }

    template <>
    Params JsonConverterV1::fromJson<Params>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelper, {});
        Params params;
        for (auto it = object.begin(); it != object.end(); ++it) {
            params.insert(it.key(), fromJson<Param>(it.value(), errors, options, path + "." + it.key()));
        }
        return params;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Phoneme>(const Phoneme &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., language, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., token, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., start, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., onset, toJsonTrivial, object);
        return object;
    }

    template <>
    Phoneme JsonConverterV1::fromJson<Phoneme>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"language", "token", "start", "onset"}), {});
        Phoneme phoneme;
        FROM_JSON_PROPERTY_FAIL_FAST(phoneme., language, fromJsonStringHelper, phoneme);
        FROM_JSON_PROPERTY_FAIL_FAST(phoneme., token, fromJsonStringHelper, phoneme);
        FROM_JSON_PROPERTY_FAIL_FAST(phoneme., start, fromJsonIntHelper, phoneme);
        FROM_JSON_PROPERTY_FAIL_FAST(phoneme., onset, fromJsonBoolHelper, phoneme);
        return phoneme;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<Phoneme>>(const QList<Phoneme> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper<Phoneme, JsonConverterV1>(entity, errors, options, path);
    }

    template <>
    QList<Phoneme> JsonConverterV1::fromJson<QList<Phoneme>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<Phoneme, JsonConverterV1>(json, errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Phonemes>(const Phonemes &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., original, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., edited, toJson, object);
        return object;
    }

    template <>
    Phonemes JsonConverterV1::fromJson<Phonemes>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"original", "edited"}), {});
        Phonemes phonemes;
        FROM_JSON_PROPERTY_FAIL_FAST(phonemes., original, fromJson<QList<Phoneme>>, phonemes);
        FROM_JSON_PROPERTY_FAIL_FAST(phonemes., edited, fromJson<QList<Phoneme>>, phonemes);
        return phonemes;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Pronunciation>(const Pronunciation &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., original, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., edited, toJsonTrivial, object);
        return object;
    }

    template <>
    Pronunciation JsonConverterV1::fromJson<Pronunciation>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"original", "edited"}), {});
        Pronunciation pronunciation;
        FROM_JSON_PROPERTY_FAIL_FAST(pronunciation., original, fromJsonStringHelper, pronunciation);
        FROM_JSON_PROPERTY_FAIL_FAST(pronunciation., edited, fromJsonStringHelper, pronunciation);
        return pronunciation;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QSharedPointer<SingingClip>>(const SingingClipRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("type", "singing");
        TO_JSON_PROPERTY_FAIL_FAST(entity->, name, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, control, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, time, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, notes, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, params, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, sources, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity->, workspace, toJson, object);
        return object;
    }

    template <>
    SingingClipRef JsonConverterV1::fromJson<QSharedPointer<SingingClip>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"type", "name", "control", "time", "notes", "params", "sources", "workspace"}), {});
        auto clip = QSharedPointer<SingingClip>::create();
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, name, fromJsonStringHelper, clip);
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, control, fromJson<BusControl>, clip);
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, time, fromJson<ClipTime>, clip);
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, notes, fromJson<QList<Note>>, clip);
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, params, fromJson<Params>, clip);
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, sources, fromJson<Sources>, clip);
        FROM_JSON_PROPERTY_FAIL_FAST(clip->, workspace, fromJson<Workspace>, clip);
        return clip;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Sources>(const Sources &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        for (const auto &[key, value] : entity.asKeyValueRange()) {
            object.insert(key, value);
        }
        return object;
    }

    template <>
    Sources JsonConverterV1::fromJson<Sources>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelper, {});
        Sources sources;
        for (auto it = object.begin(); it != object.end(); ++it) {
            QJsonObject o = fromJsonObjectHelper(it.value(), errors, options, path + "." + it.key());
            FAIL_FAST_RETURN(sources);
            sources.insert(it.key(), o);
        }
        return sources;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Tempo>(const Tempo &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., pos, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., value, toJsonNumberHelperWithConstraint(10, 1000), object);
        return object;
    }

    template <>
    Tempo JsonConverterV1::fromJson<Tempo>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"pos", "value"}), {});
        Tempo tempo;
        FROM_JSON_PROPERTY_FAIL_FAST(tempo., pos, fromJsonIntHelperWithConstraint(0), tempo);
        FROM_JSON_PROPERTY_FAIL_FAST(tempo., value, fromJsonDoubleHelperWithConstraint(10, 1000), tempo);
        return tempo;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<Tempo>>(const QList<Tempo> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper<Tempo, JsonConverterV1>(entity, errors, options, path);
    }

    template <>
    QList<Tempo> JsonConverterV1::fromJson<QList<Tempo>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<Tempo, JsonConverterV1>(json, errors, options, path);
    }

    constexpr auto DENOMINATOR_ENUM_DEFS = std::array{
        std::pair{1, 1},
        std::pair{2, 2},
        std::pair{4, 4},
        std::pair{8, 8},
        std::pair{16, 16},
        std::pair{32, 32},
        std::pair{64, 64},
        std::pair{128, 128}
    };

    template <>
    QJsonValue JsonConverterV1::toJson<TimeSignature>(const TimeSignature &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., index, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., numerator, toJsonNumberHelperWithConstraint(1), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., denominator, toJsonEnumHelper(DENOMINATOR_ENUM_DEFS), object);
        return object;
    }

    template <>
    TimeSignature JsonConverterV1::fromJson<TimeSignature>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"index", "numerator", "denominator"}), {});
        TimeSignature sig;
        FROM_JSON_PROPERTY_FAIL_FAST(sig., index, fromJsonIntHelperWithConstraint(0), sig);
        FROM_JSON_PROPERTY_FAIL_FAST(sig., numerator, fromJsonIntHelperWithConstraint(1), sig);
        FROM_JSON_PROPERTY_FAIL_FAST(sig., denominator, fromJsonEnumHelper(DENOMINATOR_ENUM_DEFS), sig);
        return sig;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<TimeSignature>>(const QList<TimeSignature> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper<TimeSignature, JsonConverterV1>(entity, errors, options, path);
    }

    template <>
    QList<TimeSignature> JsonConverterV1::fromJson<QList<TimeSignature>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<TimeSignature, JsonConverterV1>(json, errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Timeline>(const Timeline &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., labels, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., tempos, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., timeSignatures, toJson, object);
        return object;
    }

    template <>
    Timeline JsonConverterV1::fromJson<Timeline>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"labels", "tempos", "timeSignatures"}), {});
        Timeline timeline;
        FROM_JSON_PROPERTY_FAIL_FAST(timeline., labels, fromJson<QList<Label>>, timeline);
        FROM_JSON_PROPERTY_FAIL_FAST(timeline., tempos, fromJson<QList<Tempo>>, timeline);
        FROM_JSON_PROPERTY_FAIL_FAST(timeline., timeSignatures, fromJson<QList<TimeSignature>>, timeline);
        return timeline;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Track>(const Track &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., name, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., control, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., clips, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., workspace, toJson, object);
        return object;
    }

    template <>
    Track JsonConverterV1::fromJson<Track>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"name", "control", "clips", "workspace"}), {});
        Track track;
        FROM_JSON_PROPERTY_FAIL_FAST(track., name, fromJsonStringHelper, track);
        FROM_JSON_PROPERTY_FAIL_FAST(track., control, fromJson<TrackControl>, track);
        FROM_JSON_PROPERTY_FAIL_FAST(track., clips, fromJson<QList<ClipRef>>, track);
        FROM_JSON_PROPERTY_FAIL_FAST(track., workspace, fromJson<Workspace>, track);
        return track;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<Track>>(const QList<Track> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper<Track, JsonConverterV1>(entity, errors, options, path);
    }

    template <>
    QList<Track> JsonConverterV1::fromJson<QList<Track>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<Track, JsonConverterV1>(json, errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<TrackControl>(const TrackControl &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., gain, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., pan, toJsonNumberHelperWithConstraint(-1, 1), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., mute, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., solo, toJsonTrivial, object);
        return object;
    }

    template <>
    TrackControl JsonConverterV1::fromJson<TrackControl>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"gain", "pan", "mute", "solo"}), {});
        TrackControl control;
        FROM_JSON_PROPERTY_FAIL_FAST(control., gain, fromJsonDoubleHelperWithConstraint(0), control);
        FROM_JSON_PROPERTY_FAIL_FAST(control., pan, fromJsonDoubleHelperWithConstraint(-1, 1), control);
        FROM_JSON_PROPERTY_FAIL_FAST(control., mute, fromJsonBoolHelper, control);
        FROM_JSON_PROPERTY_FAIL_FAST(control., solo, fromJsonBoolHelper, control);
        return control;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Vibrato>(const Vibrato &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., start, toJsonNumberHelperWithConstraint(0, 1), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., end, toJsonNumberHelperWithConstraint(0, 1), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., amp, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., freq, toJsonNumberHelperWithConstraint(0), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., phase, toJsonNumberHelperWithConstraint(0, 1), object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., offset, toJsonTrivial, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., points, toJson, object);
        return object;
    }

    template <>
    Vibrato JsonConverterV1::fromJson<Vibrato>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"start", "end", "amp", "freq", "phase", "offset", "points"}), {});
        Vibrato vibrato;
        FROM_JSON_PROPERTY_FAIL_FAST(vibrato., start, fromJsonDoubleHelperWithConstraint(0, 1), vibrato);
        FROM_JSON_PROPERTY_FAIL_FAST(vibrato., end, fromJsonDoubleHelperWithConstraint(0, 1), vibrato);
        FROM_JSON_PROPERTY_FAIL_FAST(vibrato., amp, fromJsonIntHelperWithConstraint(0), vibrato);
        FROM_JSON_PROPERTY_FAIL_FAST(vibrato., freq, fromJsonDoubleHelperWithConstraint(0), vibrato);
        FROM_JSON_PROPERTY_FAIL_FAST(vibrato., phase, fromJsonDoubleHelperWithConstraint(0, 1), vibrato);
        FROM_JSON_PROPERTY_FAIL_FAST(vibrato., offset, fromJsonIntHelper, vibrato);
        FROM_JSON_PROPERTY_FAIL_FAST(vibrato., points, fromJson<VibratoPoints>, vibrato);
        return vibrato;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<VibratoPoints>(const VibratoPoints &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        TO_JSON_PROPERTY_FAIL_FAST(entity., amp, toJson, object);
        TO_JSON_PROPERTY_FAIL_FAST(entity., freq, toJson, object);
        return object;
    }

    template <>
    VibratoPoints JsonConverterV1::fromJson<VibratoPoints>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"amp", "freq"}), {});
        VibratoPoints points;
        FROM_JSON_PROPERTY_FAIL_FAST(points., amp, fromJson<QList<ControlPoint>>, points);
        FROM_JSON_PROPERTY_FAIL_FAST(points., freq, fromJson<QList<ControlPoint>>, points);
        return points;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<ControlPoint>>(const QList<ControlPoint> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper<ControlPoint, JsonConverterV1>(entity, errors, options, path);
    }

    template <>
    QList<ControlPoint> JsonConverterV1::fromJson<QList<ControlPoint>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<ControlPoint, JsonConverterV1>(json, errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<AnchorNode>>(const QList<AnchorNode> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper<AnchorNode, JsonConverterV1>(entity, errors, options, path);
    }

    template <>
    QList<AnchorNode> JsonConverterV1::fromJson<QList<AnchorNode>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<AnchorNode, JsonConverterV1>(json, errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<ClipRef>>(const QList<ClipRef> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper<ClipRef, JsonConverterV1>(entity, errors, options, path);
    }

    template <>
    QList<ClipRef> JsonConverterV1::fromJson<QList<ClipRef>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<ClipRef, JsonConverterV1>(json, errors, options, path);
    }

}
