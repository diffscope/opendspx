#include "jsonconverterv1.h"

#include <opendspx/model.h>

namespace QDspx{

    template <>
    QJsonValue JsonConverterV1::toJson<Model>(const Model &entity, QList<SerializationErrorRef> &errors, Serializer::Option options, const QString &path) {
        return {};
    }

    template <>
    Model JsonConverterV1::fromJson<Model>(const QJsonValue &json, QList<SerializationErrorRef> &errors, Serializer::Option options, const QString &path) {
        return {};
    }

}
