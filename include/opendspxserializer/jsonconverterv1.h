#ifndef OPENDSPX_SERIALIZER_JSONCONVERTERV1_H
#define OPENDSPX_SERIALIZER_JSONCONVERTERV1_H

#include <QJsonValue>

#include <opendspxserializer/serializer.h>

namespace QDspx {

    struct Model;

    class OPENDSPX_SERIALIZER_EXPORT JsonConverterV1 {
    public:
        template <typename T>
        static QJsonValue toJson(const T &entity, QList<SerializationErrorRef> &errors, Serializer::Option options = {Serializer::FastFail | Serializer::CheckError}, const QString &path = {"$"}) {
            static_assert(false);
        }

        template <typename T>
        static T fromJson(const QJsonValue &json, QList<SerializationErrorRef> &errors, Serializer::Option options = {Serializer::FastFail | Serializer::CheckError}, const QString &path = {"$"}) {
            static_assert(false);
        }

        template <>
        static QJsonValue toJson(const Model &entity, QList<SerializationErrorRef> &errors, Serializer::Option options, const QString &path);

        template <>
        static Model fromJson(const QJsonValue &json, QList<SerializationErrorRef> &errors, Serializer::Option options, const QString &path);

    };

}

#endif //OPENDSPX_SERIALIZER_JSONCONVERTERV1_H
