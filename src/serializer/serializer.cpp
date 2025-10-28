#include "serializer.h"

#include <QJsonDocument>

#include <opendspx/model.h>
#include <opendspxserializer/jsonconverterv1.h>
#include <opendspxserializer/serializationerror.h>

namespace QDspx{

    QByteArray Serializer::serialize(const Model &model, QList<SerializationErrorRef> &errors, Option options) {
        switch (model.version) {
        case Model::V1:
            return JsonConverterV1::toJson(model, errors, options).toJson(QJsonDocument::Compact);
            default: {
                errors.append(QSharedPointer<UnrecognizedVersionError>::create(model.version));
                return {};
            }
        }
    }

    Model Serializer::deserialize(const QByteArray &data, QList<SerializationErrorRef> &errors, Option options) {

    }

}
