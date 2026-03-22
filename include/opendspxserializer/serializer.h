#ifndef OPENDSPX_SERIALIZER_SERIALIZER_H
#define OPENDSPX_SERIALIZER_SERIALIZER_H

#include <QFlags>

#include <iosfwd>

#include <opendspx/model.h>

#include <opendspxserializer/opendspxserializerglobal.h>
#include <opendspxserializer/serializationerror.h>

namespace opendspx {

    class OPENDSPX_SERIALIZER_EXPORT Serializer {
    public:
        enum OptionFlag {
            FailFast = 0x0001,
            CheckError = 0x0002,
            TolerateRedundantProperty = 0x0004,
            CheckWarning = 0x0008,
        };
        Q_DECLARE_FLAGS(Option, OptionFlag)

        static std::string versionToText(Model::Version version);
        static Model::Version versionFromText(const std::string &text, bool *ok = nullptr);

        static void serialize(std::ostream &out, const Model &model, SerializationErrorList &errors, Option options = {FailFast | CheckError}, bool compress = false);
        static Model deserialize(std::istream &in, SerializationErrorList &errors, Option options = {FailFast | CheckError});
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(Serializer::Option)

}

#endif //OPENDSPX_SERIALIZER_SERIALIZER_H
