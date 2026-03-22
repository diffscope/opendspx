#include "serializer.h"

#include <QJsonDocument>

#include <opendspx/model.h>

#include <opendspxserializer/jsonconverterv1.h>
#include <opendspxserializer/serializationerror.h>

namespace opendspx {

    QString Serializer::versionToText(Model::Version version) {
        switch (version) {
            case Model::V1:
                return "1.0.0";
            default:
                return {};
        }
    }

    Model::Version Serializer::versionFromText(const QString &text, bool *ok) {
        if (ok)
            *ok = true;
        if (text == "1.0.0") {
            return Model::V1;
        }
        if (ok)
            *ok = false;
        return {};
    }

    QByteArray Serializer::serialize(const Model &model, SerializationErrorList &errors, Option options) {
        switch (model.version) {
            case Model::V1:
                return JsonConverterV1::toJson(model, errors, options).toJson(QJsonDocument::Compact);
            default: {
                errors.addError<UnrecognizedVersionError>(model.version);
                return {};
            }
        }
    }

    Model Serializer::deserialize(const QByteArray &data, SerializationErrorList &errors, Option options) {
        QJsonParseError jsonParseError;
        auto doc = QJsonDocument::fromJson(data, &jsonParseError);
        if (jsonParseError.error != QJsonParseError::NoError) {
            errors.addError<JsonParseFailureError>(jsonParseError);
            return {};
        }
        if (!doc.isObject()) {
            errors.addError<JsonRootIsNotObjectError>();
        }
        auto obj = doc.object();
        auto versionText = obj.value("version").toString();
        bool ok;
        auto version = versionFromText(versionText, &ok);
        if (!ok) {
            errors.addError<UnrecognizedVersionError>(versionText);
            return {};
        }
        switch (version) {
            case Model::V1:
                return JsonConverterV1::fromJson<Model>(obj, errors, options);
        }
        Q_UNREACHABLE();
    }

}
