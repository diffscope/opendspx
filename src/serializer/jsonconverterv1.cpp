#include "jsonconverterv1.h"

#include <QJsonObject>
#include <QJsonArray>

#include <opendspx/model.h>
#include <opendspxserializer/serializationerror.h>

#include "converterhelper_p.h"

namespace QDspx{

    template <>
    QJsonValue JsonConverterV1::toJson<AnchorNode>(const AnchorNode &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("x", entity.x);
        object.insert("y", entity.y);
        QString interpString;
        switch (entity.interp) {
            case AnchorNode::None:
                interpString = "none";
                break;
            case AnchorNode::Linear:
                interpString = "linear";
                break;
            case AnchorNode::Hermite:
                interpString = "hermite";
                break;
        }
        object.insert("interp", interpString);
        return object;
    }

    template <>
    AnchorNode JsonConverterV1::fromJson<AnchorNode>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"x", "y", "interp"}), {});
        AnchorNode node;
        FROM_JSON_PROPERTY_FAIL_FAST(node., x, fromJsonIntHelperWithConstraint(0), node);
        FROM_JSON_PROPERTY_FAIL_FAST(node., y, fromJsonIntHelper, node);
        FROM_JSON_PROPERTY_FAIL_FAST(node., interp, fromJsonEnumHelper(std::array{
            QPair{"none", AnchorNode::None},
            QPair{"linear", AnchorNode::Linear},
            QPair{"hermite", AnchorNode::Hermite}
        }), node);
        return node;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QSharedPointer<AudioClip>>(const AudioClipRef &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("type", "audio");
        object.insert("name", entity->name);
        object.insert("control", toJson<BusControl>(entity->control, errors, options, path + ".control"));
        object.insert("time", toJson<ClipTime>(entity->time, errors, options, path + ".time"));
        object.insert("path", entity->path);
        object.insert("workspace", toJson<Workspace>(entity->workspace, errors, options, path + ".workspace"));
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
        object.insert("gain", entity.gain);
        object.insert("pan", entity.pan);
        object.insert("mute", entity.mute);
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
        object.insert("start", entity.start);
        object.insert("length", entity.length);
        object.insert("clipStart", entity.clipStart);
        object.insert("clipLen", entity.clipLen);
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
        object.insert("x", entity.x);
        object.insert("y", entity.y);
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
                // TODO error
                return {};
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
        object.insert("global", toJson<Global>(entity.global, errors, options, path + ".global"));
        object.insert("master", toJson<Master>(entity.master, errors, options, path + ".master"));
        object.insert("timeline", toJson<Timeline>(entity.timeline, errors, options, path + ".timeline"));
        object.insert("tracks", toJson<QList<Track>>(entity.tracks, errors, options, path + ".tracks"));
        object.insert("workspace", toJson<Workspace>(entity.workspace, errors, options, path + ".workspace"));
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
        object.insert("author", entity.author);
        object.insert("name", entity.name);
        object.insert("centShift", entity.centShift);
        object.insert("editorId", entity.editorId);
        object.insert("editorName", entity.editorName);
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
        object.insert("pos", entity.pos);
        object.insert("text", entity.text);
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
        return toJsonArrayHelper(entity, errors, options, path);
    }

    template <>
    QList<Label> JsonConverterV1::fromJson<QList<Label>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<Label>(json.toArray(), errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Master>(const Master &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("control", toJson<BusControl>(entity.control, errors, options, path + ".control"));
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
        object.insert("content", toJson<Content>(entity.content, errors, options, path + ".content"));
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
        object.insert("pos", entity.pos);
        object.insert("length", entity.length);
        object.insert("keyNum", entity.keyNum);
        object.insert("centShift", entity.centShift);
        object.insert("language", entity.language);
        object.insert("lyric", entity.lyric);
        object.insert("pronunciation", toJson<Pronunciation>(entity.pronunciation, errors, options, path + ".pronunciation"));
        object.insert("phonemes", toJson<Phonemes>(entity.phonemes, errors, options, path + ".phonemes"));
        object.insert("vibrato", toJson<Vibrato>(entity.vibrato, errors, options, path + ".vibrato"));
        object.insert("workspace", toJson<Workspace>(entity.workspace, errors, options, path + ".workspace"));
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
        return toJsonArrayHelper(entity, errors, options, path);
    }

    template <>
    QList<Note> JsonConverterV1::fromJson<QList<Note>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<Note>(json.toArray(), errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Param>(const Param &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("original", toJson<QList<ParamCurveRef>>(entity.original, errors, options, path + ".original"));
        object.insert("transform", toJson<QList<ParamCurveRef>>(entity.transform, errors, options, path + ".transform"));
        object.insert("edited", toJson<QList<ParamCurveRef>>(entity.edited, errors, options, path + ".edited"));
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
                // TODO error
                return {};
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
        object.insert("start", entity->start);
        object.insert("nodes", toJson<QList<AnchorNode>>(entity->nodes, errors, options, path + ".nodes"));
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
        object.insert("start", entity->start);
        object.insert("step", entity->step);
        object.insert("values", toJsonArrayHelper(entity->values, errors, options, path + ".values"));
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
        return toJsonArrayHelper(entity, errors, options, path);
    }

    template <>
    QList<ParamCurveRef> JsonConverterV1::fromJson<QList<ParamCurveRef>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<ParamCurveRef>(json.toArray(), errors, options, path);
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
        object.insert("language", entity.language);
        object.insert("token", entity.token);
        object.insert("start", entity.start);
        object.insert("onset", entity.onset);
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
        return toJsonArrayHelper(entity, errors, options, path);
    }

    template <>
    QList<Phoneme> JsonConverterV1::fromJson<QList<Phoneme>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<Phoneme>(json.toArray(), errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Phonemes>(const Phonemes &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("original", toJson<QList<Phoneme>>(entity.original, errors, options, path + ".original"));
        object.insert("edited", toJson<QList<Phoneme>>(entity.edited, errors, options, path + ".edited"));
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
        object.insert("original", entity.original);
        object.insert("edited", entity.edited);
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
        object.insert("name", entity->name);
        object.insert("control", toJson<BusControl>(entity->control, errors, options, path + ".control"));
        object.insert("time", toJson<ClipTime>(entity->time, errors, options, path + ".time"));
        object.insert("notes", toJson<QList<Note>>(entity->notes, errors, options, path + ".notes"));
        object.insert("params", toJson<Params>(entity->params, errors, options, path + ".params"));
        object.insert("sources", toJson<Sources>(entity->sources, errors, options, path + ".sources"));
        object.insert("workspace", toJson<Workspace>(entity->workspace, errors, options, path + ".workspace"));
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
        object.insert("pos", entity.pos);
        object.insert("value", entity.value);
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
        return toJsonArrayHelper(entity, errors, options, path);
    }

    template <>
    QList<Tempo> JsonConverterV1::fromJson<QList<Tempo>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<Tempo>(json.toArray(), errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<TimeSignature>(const TimeSignature &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("index", entity.index);
        object.insert("numerator", entity.numerator);
        object.insert("denominator", entity.denominator);
        return object;
    }

    template <>
    TimeSignature JsonConverterV1::fromJson<TimeSignature>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"index", "numerator", "denominator"}), {});
        TimeSignature sig;
        FROM_JSON_PROPERTY_FAIL_FAST(sig., index, fromJsonIntHelperWithConstraint(0), sig);
        FROM_JSON_PROPERTY_FAIL_FAST(sig., numerator, fromJsonIntHelperWithConstraint(1), sig);
        FROM_JSON_PROPERTY_FAIL_FAST(sig., denominator, fromJsonEnumHelper(std::array{
            QPair{1, 1},
            QPair{2, 2},
            QPair{4, 4},
            QPair{8, 8},
            QPair{16, 16},
            QPair{32, 32},
            QPair{64, 64},
            QPair{128, 128}
        }), {});
        return sig;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<TimeSignature>>(const QList<TimeSignature> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper(entity, errors, options, path);
    }

    template <>
    QList<TimeSignature> JsonConverterV1::fromJson<QList<TimeSignature>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<TimeSignature>(json.toArray(), errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Timeline>(const Timeline &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("labels", toJson<QList<Label>>(entity.labels, errors, options, path + ".labels"));
        object.insert("tempos", toJson<QList<Tempo>>(entity.tempos, errors, options, path + ".tempos"));
        object.insert("timeSignatures", toJson<QList<TimeSignature>>(entity.timeSignatures, errors, options, path + ".timeSignatures"));
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
        object.insert("name", entity.name);
        object.insert("control", toJson<TrackControl>(entity.control, errors, options, path + ".control"));
        object.insert("clips", toJson<QList<ClipRef>>(entity.clips, errors, options, path + ".clips"));
        object.insert("workspace", toJson<Workspace>(entity.workspace, errors, options, path + ".workspace"));
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
        return toJsonArrayHelper(entity, errors, options, path);
    }

    template <>
    QList<Track> JsonConverterV1::fromJson<QList<Track>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<Track>(json.toArray(), errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<TrackControl>(const TrackControl &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("gain", entity.gain);
        object.insert("pan", entity.pan);
        object.insert("mute", entity.mute);
        object.insert("solo", entity.solo);
        return object;
    }

    template <>
    TrackControl JsonConverterV1::fromJson<TrackControl>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        FROM_JSON_FAIL_FAST(object, json, fromJsonObjectHelperWithPropertyCheck(std::array{"gain", "pan", "mute", "solo"}), {});
        TrackControl control;
        FROM_JSON_PROPERTY_FAIL_FAST(control., gain, fromJsonDoubleHelper, control);
        FROM_JSON_PROPERTY_FAIL_FAST(control., pan, fromJsonDoubleHelperWithConstraint(-1, 1), control);
        FROM_JSON_PROPERTY_FAIL_FAST(control., mute, fromJsonBoolHelper, control);
        FROM_JSON_PROPERTY_FAIL_FAST(control., solo, fromJsonBoolHelper, control);
        return control;
    }

    template <>
    QJsonValue JsonConverterV1::toJson<Vibrato>(const Vibrato &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonObject object;
        object.insert("start", entity.start);
        object.insert("end", entity.end);
        object.insert("amp", entity.amp);
        object.insert("freq", entity.freq);
        object.insert("phase", entity.phase);
        object.insert("offset", entity.offset);
        object.insert("points", toJson<VibratoPoints>(entity.points, errors, options, path + ".points"));
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
        object.insert("amp", toJson<QList<ControlPoint>>(entity.amp, errors, options, path + ".amp"));
        object.insert("freq", toJson<QList<ControlPoint>>(entity.freq, errors, options, path + ".freq"));
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
        return toJsonArrayHelper(entity, errors, options, path);
    }

    template <>
    QList<ControlPoint> JsonConverterV1::fromJson<QList<ControlPoint>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<ControlPoint>(json.toArray(), errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<AnchorNode>>(const QList<AnchorNode> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper(entity, errors, options, path);
    }

    template <>
    QList<AnchorNode> JsonConverterV1::fromJson<QList<AnchorNode>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<AnchorNode>(json.toArray(), errors, options, path);
    }

    template <>
    QJsonValue JsonConverterV1::toJson<QList<ClipRef>>(const QList<ClipRef> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return toJsonArrayHelper(entity, errors, options, path);
    }

    template <>
    QList<ClipRef> JsonConverterV1::fromJson<QList<ClipRef>>(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        return fromJsonArrayHelper<ClipRef>(json.toArray(), errors, options, path);
    }

}
