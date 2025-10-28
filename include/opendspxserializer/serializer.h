#ifndef OPENDSPX_SERIALIZER_SERIALIZER_H
#define OPENDSPX_SERIALIZER_SERIALIZER_H

#include <QSharedPointer>

#include <opendspxserializer/opendspxserializerglobal.h>

namespace QDspx {

    struct Model;
    class SerializationError;
    using SerializationErrorRef = QSharedPointer<SerializationError>;

    class OPENDSPX_SERIALIZER_EXPORT Serializer {
    public:
        enum OptionFlag {
            FastFail = 0x0001,
            CheckError = 0x0002,
            TolerateRedundantProperty = 0x0004,
            CheckWarning = 0x0008,
        };
        Q_DECLARE_FLAGS(Option, OptionFlag)

        static QByteArray serialize(const Model &model, QList<SerializationErrorRef> &errors, Option options = {FastFail | CheckError});
        static Model deserialize(const QByteArray &data, QList<SerializationErrorRef> &errors, Option options = {FastFail | CheckError});
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(Serializer::Option)

}

#endif //OPENDSPX_SERIALIZER_SERIALIZER_H
