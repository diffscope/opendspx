#ifndef OPENDSPX_SERIALIZER_SERIALIZATIONERROR_H
#define OPENDSPX_SERIALIZER_SERIALIZATIONERROR_H

#include <any>
#include <utility>

#include <nlohmann/json.hpp>

namespace opendspx {

    class SerializationError {
    public:
        enum Type {
            JsonParseFailure = 0x0001,
            JsonRootIsNotObject = 0x0002,
            UnrecognizedVersion = 0x0003,
            CompressionFailure = 0x0041,
            InvalidDataType = 0x1001,
            InvalidObjectType = 0x1002,
            RangeConstraintViolation = 0x1101,
            EnumConstraintViolation = 0x1102,
            InvalidRatioPartition = 0x11C1,
            PartCountNotMatch = 0x11C2,
            EmptySingerMixing = 0x11C3,
            MissingProperty = 0x1401,
            RedundantProperty = 0x1402,
            OverlappingItem = 0x2001,
            ZeroLengthRange = 0x2002,
            ErroneousClipRange = 0x2101,
            ErroneousClipPosition = 0x2102,
            SafeRangeLimitExceeded = 0x2C01,
        };
        Type type() const {
            return m_type;
        }
        bool isFatal() const {
            return (m_type & 0xF000) == 0x0000;
        }
        bool isError() const {
            return (m_type & 0xF000) == 0x1000;
        }
        bool isWarning() const {
            return (m_type & 0xF000) == 0x2000;
        }

    protected:
        SerializationError(Type type) : m_type(type) {
        }

    private:
        Type m_type;

    };

    using SerializationErrorRef = std::shared_ptr<SerializationError>;

    class JsonParseFailureError : public SerializationError {
    public:
        JsonParseFailureError(int code, std::size_t index, std::string message)
            : SerializationError(JsonParseFailure), m_code(code), m_index(index), m_message(std::move(message)) {
        }
        int code() const {
            return m_code;
        }
        std::size_t index() const {
            return m_index;
        }
        std::string message() const {
            return m_message;
        }
    private:
        int m_code;
        std::size_t m_index;
        std::string m_message;
    };

    class JsonRootIsNotObjectError : public SerializationError {
    public:
        JsonRootIsNotObjectError()
            : SerializationError(JsonRootIsNotObject) {
        }
    };

    class UnrecognizedVersionError : public SerializationError {
    public:
        UnrecognizedVersionError(std::string actualVersion)
            : SerializationError(UnrecognizedVersion), m_actualVersion(std::move(actualVersion)), m_actualVersionFlag(0) {
        }
        UnrecognizedVersionError(int actualVersionFlag)
            : SerializationError(UnrecognizedVersion), m_actualVersionFlag(actualVersionFlag) {
        }
        std::string actualVersion() const {
            return m_actualVersion;
        }
        int actualVersionFlag() const {
            return m_actualVersionFlag;
        }
    private:
        std::string m_actualVersion;
        int m_actualVersionFlag;
    };

    class CompressionFailureError : public SerializationError {
    public:
        explicit CompressionFailureError(std::string message)
            : SerializationError(CompressionFailure), m_message(std::move(message)) {
        }

        std::string message() const {
            return m_message;
        }

    private:
        std::string m_message;
    };

    class InvalidDataTypeError : public SerializationError {
    public:
        enum DataType {
            Null,
            Bool,
            Integer,
            Double,
            String,
            Array,
            Object,
        };

        InvalidDataTypeError(std::string path, DataType actualType, std::vector<DataType> expectedTypes)
            : SerializationError(InvalidDataType), m_path(std::move(path)), m_actualType(actualType), m_expectedTypes(std::move(expectedTypes)) {
        }

        std::string path() const {
            return m_path;
        }
        DataType actualType() const {
            return m_actualType;
        }
        std::vector<DataType> expectedTypes() const {
            return m_expectedTypes;
        }

    private:
        std::string m_path;
        DataType m_actualType;
        std::vector<DataType> m_expectedTypes;

    };

    class InvalidObjectTypeError : public SerializationError {
    public:
        InvalidObjectTypeError(std::string path, std::string actualType, std::vector<std::string> expectedTypes)
            : SerializationError(InvalidObjectType), m_path(std::move(path)), m_actualType(std::move(actualType)), m_expectedTypes(std::move(expectedTypes)) {
        }

        std::string path() const {
            return m_path;
        }
        std::string actualType() const {
            return m_actualType;
        }
        std::vector<std::string> expectedTypes() const {
            return m_expectedTypes;
        }

    private:
        std::string m_path;
        std::string m_actualType;
        std::vector<std::string> m_expectedTypes;
    };

    class RangeConstraintViolationError : public SerializationError {
    public:
        RangeConstraintViolationError(std::string path, std::any actualValue, std::any expectedMinimum, std::any expectedMaximum)
            : SerializationError(RangeConstraintViolation), m_path(std::move(path)), m_actualValue(std::move(actualValue)), m_expectedMinimum(std::move(expectedMinimum)), m_expectedMaximum(std::move(expectedMaximum)) {
        }

        std::string path() const {
            return m_path;
        }
        std::any actualValue() const {
            return m_actualValue;
        }
        std::any expectedMinimum() const {
            return m_expectedMinimum;
        }
        std::any expectedMaximum() const {
            return m_expectedMaximum;
        }

    private:
        std::string m_path;
        std::any m_actualValue;
        std::any m_expectedMinimum;
        std::any m_expectedMaximum;
    };

    class EnumConstraintViolationError : public SerializationError {
    public:
        EnumConstraintViolationError(std::string path, std::any actualEnumValue, std::vector<std::any> expectedEnumValues)
            : SerializationError(EnumConstraintViolation), m_path(std::move(path)), m_actualEnumValue(std::move(actualEnumValue)), m_expectedEnumValues(std::move(expectedEnumValues)) {
        }

        std::string path() const {
            return m_path;
        }
        std::any actualEnumValue() const {
            return m_actualEnumValue;
        }
        std::vector<std::any> expectedEnumValues() const {
            return m_expectedEnumValues;
        }

    private:
        std::string m_path;
        std::any m_actualEnumValue;
        std::vector<std::any> m_expectedEnumValues;
    };

    class InvalidRatioPartitionError : public SerializationError {
    public:
        InvalidRatioPartitionError(std::string path, std::vector<double> ratio)
            : SerializationError(InvalidRatioPartition), m_path(std::move(path)), m_ratio(std::move(ratio)) {
        }
        std::string path() const {
            return m_path;
        }
        std::vector<double> ratio() const {
            return m_ratio;
        }

    private:
        std::string m_path;
        std::vector<double> m_ratio;
    };

    class EmptySingerMixingError : public SerializationError {
    public:
        EmptySingerMixingError(std::string path)
            : SerializationError(EmptySingerMixing), m_path(std::move(path)) {
        }
        std::string path() const {
            return m_path;
        }
    private:
        std::string m_path;
    };

    class PartCountNotMatchError : public SerializationError {
    public:
        PartCountNotMatchError(std::string path, int expectedPartCount, int actualPartCount)
            : SerializationError(PartCountNotMatch), m_path(std::move(path)), m_expectedPartCount(expectedPartCount), m_actualPartCount(actualPartCount) {
        }

        std::string path() const {
            return m_path;
        }
        int expectedPartCount() const {
            return m_expectedPartCount;
        }
        int actualPartCount() const {
            return m_actualPartCount;
        }

    private:
        std::string m_path;
        int m_expectedPartCount;
        int m_actualPartCount;
    };

    class MissingPropertyError : public SerializationError {
    public:
        MissingPropertyError(std::string path, std::vector<std::string> missingProperties)
            : SerializationError(MissingProperty), m_path(std::move(path)), m_missingProperties(std::move(missingProperties)) {
        }

        std::string path() const {
            return m_path;
        }
        std::vector<std::string> missingProperties() const {
            return m_missingProperties;
        }

    private:
        std::string m_path;
        std::vector<std::string> m_missingProperties;
    };

    class RedundantPropertyError : public SerializationError {
    public:
        RedundantPropertyError(const std::string &path, const std::vector<std::string> &redundantProperties)
            : SerializationError(RedundantProperty), m_path(path), m_redundantProperties(redundantProperties) {
        }

        std::string path() const {
            return m_path;
        }
        std::vector<std::string> redundantProperties() const {
            return m_redundantProperties;
        }

    private:
        std::string m_path;
        std::vector<std::string> m_redundantProperties;
    };

    class OverlappingItemError : public SerializationError {
    public:
        OverlappingItemError(const std::string &path, const std::vector<int> &overlappedItemIndexes)
            : SerializationError(OverlappingItem), m_path(path), m_overlappedItemIndexes(overlappedItemIndexes) {
        }

        std::string path() const {
            return m_path;
        }
        std::vector<int> overlappedItemIndexes() const {
            return m_overlappedItemIndexes;
        }

    private:
        std::string m_path;
        std::vector<int> m_overlappedItemIndexes;
    };

    class ZeroLengthRangeError : public SerializationError {
    public:
        ZeroLengthRangeError(std::string path)
            : SerializationError(ZeroLengthRange), m_path(std::move(path)) {
        }

        std::string path() const {
            return m_path;
        }

    private:
        std::string m_path;
    };

    class ErroneousClipRangeError : public SerializationError {
    public:
        ErroneousClipRangeError(std::string path)
            : SerializationError(ErroneousClipRange), m_path(std::move(path)) {
        }

        std::string path() const {
            return m_path;
        }

    private:
        std::string m_path;
    };

    class ErroneousClipPositionError : public SerializationError {
    public:
        ErroneousClipPositionError(std::string path)
            : SerializationError(ErroneousClipPosition), m_path(std::move(path)) {
        }

        std::string path() const {
            return m_path;
        }

    private:
        std::string m_path;
    };

    class SafeRangeLimitExceededError : public SerializationError {
    public:
        SafeRangeLimitExceededError(const std::string &path)
            : SerializationError(SafeRangeLimitExceeded), m_path(path) {
        }

        std::string path() const {
            return m_path;
        }

    private:
        std::string m_path;
    };

    class SerializationErrorList : public std::vector<SerializationErrorRef> {
    public:
        using std::vector<SerializationErrorRef>::vector;

        template<typename T, typename... Args>
        void addError(Args &&... args) {
            auto s = std::make_shared<T>(std::forward<Args>(args)...);
            push_back(s);
            m_containsFatal = m_containsFatal || s->isFatal();
            m_containsError = m_containsError || s->isError();
            m_containsWarning = m_containsWarning || s->isWarning();
        }

        bool containsFatal() const {
            return m_containsFatal;
        }

        bool containsError() const {
            return m_containsError;
        }

        bool containsWarning() const {
            return m_containsWarning;
        }

    private:
        bool m_containsFatal = false;
        bool m_containsError = false;
        bool m_containsWarning = false;
    };

}

#endif //OPENDSPX_SERIALIZER_SERIALIZATIONERROR_H
