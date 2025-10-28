#ifndef OPENDSPX_SERIALIZER_SERIALIZER_H
#define OPENDSPX_SERIALIZER_SERIALIZER_H

#include <QSharedPointer>

#include <opendspxserializer/opendspxserializerglobal.h>
#include <opendspxserializer/serializationerror.h>

namespace QDspx {

    struct Model;

    class OPENDSPX_SERIALIZER_EXPORT Serializer {
    public:
        enum OptionFlag {
            FailFast = 0x0001,
            CheckError = 0x0002,
            TolerateRedundantProperty = 0x0004,
            CheckWarning = 0x0008,
        };
        Q_DECLARE_FLAGS(Option, OptionFlag)

        static QByteArray serialize(const Model &model, SerializationErrorList &errors, Option options = {FailFast | CheckError});
        static Model deserialize(const QByteArray &data, SerializationErrorList &errors, Option options = {FailFast | CheckError});
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(Serializer::Option)

}

#endif //OPENDSPX_SERIALIZER_SERIALIZER_H
