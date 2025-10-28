#ifndef OPENDSPX_SERIALIZER_SERIALIZATIONERROR_H
#define OPENDSPX_SERIALIZER_SERIALIZATIONERROR_H

#include <QJsonParseError>
#include <QList>
#include <QSharedPointer>
#include <QString>
#include <QVariant>

namespace QDspx {

    class SerializationError {
    public:
        enum Type {
            JsonParseFailure = 0x0001,
            UnrecognizedVersion = 0x0002,
            InvalidDataType = 0x1001,
            InvalidObjectType = 0x1002,
            RangeConstraintViolation = 0x1101,
            EnumConstraintViolation = 0x1102,
            MissingProperty = 0x1401,
            RedundantProperty = 0x1402,
            OverlappingItem = 0x2001,
            ZeroLengthRange = 0x2002,
            ErroneousClipRange = 0x2101,
            ErroneousClipPosition = 0x2102,
            SafeRangeLimitExceeded = 0x2C01,
        };
        Type type() {
            return m_type;
        }

    protected:
        SerializationError(Type type) : m_type(type) {
        }

    private:
        Type m_type;

    };

    class JsonParseFailureError : public SerializationError {
    public:
        JsonParseFailureError(const QJsonParseError &error)
            : SerializationError(JsonParseFailure), m_error(error) {
        }
        QJsonParseError error() const {
            return m_error;
        }
    private:
        QJsonParseError m_error;
    };

    class UnrecognizedVersionError : public SerializationError {
    public:
        UnrecognizedVersionError(const QString &parsedVersion)
            : SerializationError(UnrecognizedVersion), m_parsedVersion(parsedVersion) {
        }
        QString parsedVersion() const {
            return m_parsedVersion;
        }
    private:
        QString m_parsedVersion;
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

        InvalidDataTypeError(const QString &path, DataType actualType, DataType expectedType)
            : SerializationError(InvalidDataType), m_path(path), m_actualType(actualType), m_expectedType(expectedType) {
        }

        QString path() const {
            return m_path;
        }
        DataType actualType() const {
            return m_actualType;
        }
        DataType expectedType() const {
            return m_expectedType;
        }

    private:
        QString m_path;
        DataType m_actualType;
        DataType m_expectedType;

    };

    class InvalidObjectTypeError : public SerializationError {
    public:
        InvalidObjectTypeError(const QString &path, const QString &actualType, const QStringList &expectedTypes)
            : SerializationError(InvalidObjectType), m_path(path), m_actualType(actualType), m_expectedTypes(expectedTypes) {
        }

        QString path() const {
            return m_path;
        }
        QString actualType() const {
            return m_actualType;
        }
        QStringList expectedTypes() const {
            return m_expectedTypes;
        }

    private:
        QString m_path;
        QString m_actualType;
        QStringList m_expectedTypes;
    };

    class RangeConstraintViolationError : public SerializationError {
    public:
        RangeConstraintViolationError(const QString &path, const QVariant &actualValue, const QVariant &expectedMinimum, const QVariant &expectedMaximum)
            : SerializationError(RangeConstraintViolation), m_path(path), m_actualValue(actualValue), m_expectedMinimum(expectedMinimum), m_expectedMaximum(expectedMaximum) {
        }

        QString path() const {
            return m_path;
        }
        QVariant actualValue() const {
            return m_actualValue;
        }
        QVariant expectedMinimum() const {
            return m_expectedMinimum;
        }
        QVariant expectedMaximum() const {
            return m_expectedMaximum;
        }

    private:
        QString m_path;
        QVariant m_actualValue;
        QVariant m_expectedMinimum;
        QVariant m_expectedMaximum;
    };

    class EnumConstraintViolationError : public SerializationError {
    public:
        EnumConstraintViolationError(const QString &path, const QVariant &actualEnumValue, const QStringList &expectedEnumValues)
            : SerializationError(EnumConstraintViolation), m_path(path), m_actualEnumValue(actualEnumValue), m_expectedEnumValues(expectedEnumValues) {
        }

        QString path() const {
            return m_path;
        }
        QVariant actualEnumValue() const {
            return m_actualEnumValue;
        }
        QStringList expectedEnumValues() const {
            return m_expectedEnumValues;
        }

    private:
        QString m_path;
        QVariant m_actualEnumValue;
        QStringList m_expectedEnumValues;
    };

    class MissingPropertyError : public SerializationError {
    public:
        MissingPropertyError(const QString &path, const QStringList &missingProperties)
            : SerializationError(MissingProperty), m_path(path), m_missingProperties(missingProperties) {
        }

        QString path() const {
            return m_path;
        }
        QStringList missingProperties() const {
            return m_missingProperties;
        }

    private:
        QString m_path;
        QStringList m_missingProperties;
    };

    class RedundantPropertyError : public SerializationError {
    public:
        RedundantPropertyError(const QString &path, const QStringList &redundantProperties)
            : SerializationError(RedundantProperty), m_path(path), m_redundantProperties(redundantProperties) {
        }

        QString path() const {
            return m_path;
        }
        QStringList redundantProperties() const {
            return m_redundantProperties;
        }

    private:
        QString m_path;
        QStringList m_redundantProperties;
    };

    class OverlappingItemError : public SerializationError {
    public:
        OverlappingItemError(const QString &path, const QList<int> &overlappedItemIndexes)
            : SerializationError(OverlappingItem), m_path(path), m_overlappedItemIndexes(overlappedItemIndexes) {
        }

        QString path() const {
            return m_path;
        }
        QList<int> overlappedItemIndexes() const {
            return m_overlappedItemIndexes;
        }

    private:
        QString m_path;
        QList<int> m_overlappedItemIndexes;
    };

    class ZeroLengthRangeError : public SerializationError {
    public:
        ZeroLengthRangeError(const QString &path)
            : SerializationError(ZeroLengthRange), m_path(path) {
        }

        QString path() const {
            return m_path;
        }

    private:
        QString m_path;
    };

    class ErroneousClipRangeError : public SerializationError {
    public:
        ErroneousClipRangeError(const QString &path)
            : SerializationError(ErroneousClipRange), m_path(path) {
        }

        QString path() const {
            return m_path;
        }

    private:
        QString m_path;
    };

    class ErroneousClipPositionError : public SerializationError {
    public:
        ErroneousClipPositionError(const QString &path)
            : SerializationError(ErroneousClipPosition), m_path(path) {
        }

        QString path() const {
            return m_path;
        }

    private:
        QString m_path;
    };

    class SafeRangeLimitExceededError : public SerializationError {
    public:
        SafeRangeLimitExceededError(const QString &path)
            : SerializationError(SafeRangeLimitExceeded), m_path(path) {
        }

        QString path() const {
            return m_path;
        }

    private:
        QString m_path;
    };

}

#endif //OPENDSPX_SERIALIZER_SERIALIZATIONERROR_H
