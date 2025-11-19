#include "serializer.h"

#include <QJsonDocument>

#include <opendspx/model.h>

#include <opendspxserializer/jsonconverterv1.h>
#include <opendspxserializer/serializationerror.h>

namespace QDspx {

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
        auto version = obj.value("version").toString();
        if (version == "1.0.0") {
            return JsonConverterV1::fromJson<Model>(obj, errors, options);
        }
        errors.addError<UnrecognizedVersionError>(version);
        return {};
    }

}
