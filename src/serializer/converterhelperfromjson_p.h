#ifndef OPENDSPX_SERIALIZATION_CONVERTERHELPER_P_H
#define OPENDSPX_SERIALIZATION_CONVERTERHELPER_P_H

#include "serializationerror.h"
#include "serializer.h"
#include <algorithm>
#include <array>
#include <limits>

#include <QJsonArray>
#include <QJsonObject>

#ifndef FAIL_FAST_RETURN
#define FAIL_FAST_RETURN(ret) \
    do { \
        if ((options & Serializer::FailFast) && errors.containsError()) { \
            return ret; \
        } \
    } while (false)
#endif

#define FROM_JSON_FAIL_FAST(val, json, parseFunc, ret) \
    do {\
        FAIL_FAST_RETURN(ret); \
        auto v = parseFunc(json, errors, options, path); \
        FAIL_FAST_RETURN(ret); \
        val = v; \
    } while (false)

#define FROM_JSON_PROPERTY_FAIL_FAST(entity, property, parseFunc, ret) \
    do { \
        if (!object.contains(#property)) \
            break; /* Suppose that property existency has been checked with `fromJsonObjectHelper` */ \
        FAIL_FAST_RETURN(ret); \
        auto v = parseFunc(object.value(#property), errors, options, path + "."#property); \
        FAIL_FAST_RETURN(ret); \
        entity property = v; \
    } while (false)

namespace QDspx {

    inline bool isInteger(double v) {
        return std::isfinite(v) && std::trunc(v) == v && v >= std::numeric_limits<int>::min() && v <= std::numeric_limits<int>::max();
    }

    inline InvalidDataTypeError::DataType getDataType(const QJsonValue &value) {
        switch (value.type()) {
            case QJsonValue::Type::Bool:
                return InvalidDataTypeError::Bool;
            case QJsonValue::Type::Double:
                return isInteger(value.toDouble()) ? InvalidDataTypeError::Integer : InvalidDataTypeError::Double;
            case QJsonValue::Type::String:
                return InvalidDataTypeError::String;
            case QJsonValue::Type::Array:
                return InvalidDataTypeError::Array;
            case QJsonValue::Type::Object:
                return InvalidDataTypeError::Object;
            default:
                return InvalidDataTypeError::Null;
        }
    }

    inline QString fromJsonStringHelper(const QJsonValue &value, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        if (!(options & Serializer::CheckError)) {
            return value.toString();
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::String) {
            errors.addError<InvalidDataTypeError>(path, actualType, QList{InvalidDataTypeError::String});
        }
        return value.toString();
    }

    inline double fromJsonDoubleHelper(const QJsonValue &value, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        if (!(options & Serializer::CheckError)) {
            return value.toDouble();
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Double && actualType != InvalidDataTypeError::Integer) {
            errors.addError<InvalidDataTypeError>(path, actualType, QList{InvalidDataTypeError::Double, InvalidDataTypeError::Integer});
        }
        return value.toDouble();
    }

    struct fromJsonDoubleHelperWithConstraint {
        fromJsonDoubleHelperWithConstraint(double minimum, double maximum = std::numeric_limits<double>::lowest()) : maximum(maximum), minimum(minimum) {
        }

        double operator()(const QJsonValue &value, SerializationErrorList &errors, Serializer::Option options, const QString &path) const {
            if (!(options & Serializer::CheckError)) {
                return value.toDouble();
            }
            if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Double && actualType != InvalidDataTypeError::Integer) {
                errors.addError<InvalidDataTypeError>(path, actualType, QList{InvalidDataTypeError::Double, InvalidDataTypeError::Integer});
                return 0;
            }
            auto v = value.toDouble();
            if (v < minimum || (maximum >= minimum && v > maximum)) {
                auto maxVar = maximum >= minimum ? maximum : QVariant();
                errors.addError<RangeConstraintViolationError>(path, v, minimum, maxVar);
            }
            return v;
        }

        double maximum;
        double minimum;
    };

    inline int fromJsonIntHelper(const QJsonValue &value, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        if (!(options & Serializer::CheckError)) {
            return value.toInt();
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Integer) {
            errors.addError<InvalidDataTypeError>(path, actualType, QList{InvalidDataTypeError::Integer});
        }
        return value.toInt();
    }

    struct fromJsonIntHelperWithConstraint {
        fromJsonIntHelperWithConstraint(int minimum, int maximum = std::numeric_limits<int>::lowest()) : maximum(maximum), minimum(minimum) {
        }

        int operator()(const QJsonValue &value, SerializationErrorList &errors, Serializer::Option options, const QString &path) const {
            if (!(options & Serializer::CheckError)) {
                return value.toInt();
            }
            if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Integer) {
                errors.addError<InvalidDataTypeError>(path, actualType, QList{InvalidDataTypeError::Integer});
                return 0;
            }
            auto v = value.toInt();
            if (v < minimum || (maximum >= minimum && v > maximum)) {
                auto maxVar = maximum >= minimum ? maximum : QVariant();
                errors.addError<RangeConstraintViolationError>(path, v, minimum, maxVar);
            }
            return v;
        }

        int maximum;
        int minimum;
    };

    inline bool fromJsonBoolHelper(const QJsonValue &value, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        if (!(options & Serializer::CheckError)) {
            return value.toBool();
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Bool) {
            errors.addError<InvalidDataTypeError>(path, actualType, QList{InvalidDataTypeError::Bool});
        }
        return value.toBool();
    }

    template <typename K, typename T, size_t N>
    struct fromJsonEnumHelper {
        static_assert(std::is_same_v<K, const char *> || std::is_same_v<K, int>);

        fromJsonEnumHelper(const std::array<QPair<K, T>, N> &enumValues) : enumValues(enumValues) {
        }

        static constexpr InvalidDataTypeError::DataType Flag = std::is_same_v<K, const char *> ? InvalidDataTypeError::String : InvalidDataTypeError::Integer;

        T operator()(const QJsonValue &value, SerializationErrorList &errors, Serializer::Option options, const QString &path) const {
            if (!(options & Serializer::CheckError)) {
                auto s = value.toVariant().value<K>();
                auto it = std::ranges::find_if(enumValues, [s](const auto &pair) {
                    return pair.first == s;
                });
                return it == enumValues.end() ? T{} : it->second;
            }
            if (auto actualType = getDataType(value); actualType != Flag) {
                errors.addError<InvalidDataTypeError>(path, actualType, QList{Flag});
                return {};
            }
            bool ok;
            T ret;
            if constexpr (std::is_same_v<K, const char *>) {
                auto s = value.toString();
                auto it = std::ranges::find_if(enumValues, [s](const auto &pair) {
                    return pair.first == s;
                });
                ok = it != enumValues.end();
                ret = ok ? it->second : T{};
            } else {
                auto s = value.toInt();
                auto it = std::ranges::find_if(enumValues, [s](const auto &pair) {
                    return pair.first == s;
                });
                ok = it != enumValues.end();
                ret = ok ? it->second : T{};
            }

            if (!ok) {
                QVariantList a;
                for (auto &pair : enumValues) {
                    a.append(pair.first);
                }
                errors.addError<EnumConstraintViolationError>(path, ret, a);
                return {};
            }
            return ret;
        }

        std::array<QPair<K, T>, N> enumValues;
    };

    template <typename T, typename C>
    QList<T> fromJsonArrayHelper(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QList<T> list;
        auto array = json.toArray();
        if (auto actualType = getDataType(array); (options & Serializer::CheckError) && actualType != InvalidDataTypeError::Array) {
            errors.addError<InvalidDataTypeError>(path, actualType, QList{InvalidDataTypeError::Array});
            return {};
        }
        for (auto it = array.begin(); it != array.end(); ++it) {
            auto index = std::distance(array.begin(), it);
            auto v = C::template fromJson<T>(*it, errors, options, path + "[" + QString::number(index) + "]");
            FAIL_FAST_RETURN(list);
            list.append(v);
        }
        return list;
    }

    template <typename C = void>
    inline QList<int> fromJsonArrayHelper(const QJsonValue &json, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QList<int> list;
        auto array = json.toArray();
        if (auto actualType = getDataType(array); (options & Serializer::CheckError) && actualType != InvalidDataTypeError::Array) {
            errors.addError<InvalidDataTypeError>(path, actualType, QList{InvalidDataTypeError::Array});
            return {};
        }
        for (auto it = array.begin(); it != array.end(); ++it) {
            auto index = std::distance(array.begin(), it);
            auto v = fromJsonIntHelper(*it, errors, options, path + "[" + QString::number(index) + "]");
            FAIL_FAST_RETURN(list);
            list.append(v);
        }
        return list;
    }

    inline QJsonObject fromJsonObjectHelper(const QJsonValue &value, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        if (!(options & Serializer::CheckError)) {
            return value.toObject();
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Object) {
            errors.addError<InvalidDataTypeError>(path, actualType, QList{InvalidDataTypeError::Object});
        }
        return value.toObject();
    }

    template <size_t N>
    struct fromJsonObjectHelperWithPropertyCheck {
        fromJsonObjectHelperWithPropertyCheck(std::array<const char *, N> &&properties) : properties(properties) {
        }

        QJsonObject operator()(const QJsonValue &value, SerializationErrorList &errors, Serializer::Option options, const QString &path) const {
            if (!(options & Serializer::CheckError)) {
                return value.toObject();
            }
            if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Object) {
                errors.addError<InvalidDataTypeError>(path, actualType, QList{InvalidDataTypeError::Object});
                return {};
            }
            auto obj = value.toObject();
            QStringList missingProperties;
            for (auto &property : properties) {
                if (!obj.contains(property)) {
                    missingProperties.append(property);
                }
            }
            if (!missingProperties.isEmpty()) {
                errors.addError<MissingPropertyError>(path, missingProperties);
            }
            if (!(options & Serializer::TolerateRedundantProperty) && obj.count() + missingProperties.size() > properties.size()) {
                QStringList redundantProperties;
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    if (!std::ranges::any_of(properties, [it](const auto &property) {
                        return it.key() == property;
                    })) {
                        redundantProperties.append(it.key());
                    }
                }
                errors.addError<RedundantPropertyError>(path, redundantProperties);
            }
            return obj;
        }

        std::array<const char *, N> properties;
    };

}

#endif //OPENDSPX_SERIALIZATION_CONVERTERHELPER_P_H
