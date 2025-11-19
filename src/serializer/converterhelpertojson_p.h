#ifndef OPENDSPX_SERIALIZER_CONVERTERHELPERTOJSON_P_H
#define OPENDSPX_SERIALIZER_CONVERTERHELPERTOJSON_P_H

#include <QJsonArray>
#include <QJsonValue>

#include <opendspxserializer/serializer.h>
#include <opendspxserializer/serializationerror.h>

#ifndef FAIL_FAST_RETURN
#define FAIL_FAST_RETURN(ret) \
    do { \
        if ((options & Serializer::FailFast) && errors.containsError()) { \
            return ret; \
        } \
    } while (false)
#endif

#define TO_JSON_PROPERTY_FAIL_FAST(entity, property, parseFunc, ret) \
    do { \
        FAIL_FAST_RETURN(ret); \
        auto v = parseFunc(entity property, errors, options, path + "."#property); \
        FAIL_FAST_RETURN(ret); \
        object.insert(#property, v); \
    } while (false)

namespace QDspx {

    template <typename T>
    QJsonValue toJsonTrivial(T v, SerializationErrorList &, Serializer::Option, const QString &) {
        return v;
    }

    struct toJsonNumberHelperWithConstraint {
        toJsonNumberHelperWithConstraint(double minimum, double maximum = std::numeric_limits<double>::lowest()) : minimum(minimum), maximum(maximum) {
        }

        template <typename T>
        QJsonValue operator()(T v, SerializationErrorList &errors, Serializer::Option options, const QString &path) const {
            if (!(options & Serializer::CheckError)) {
                return v;
            }
            if (v < minimum || (maximum >= minimum && v > maximum)) {
                auto maxVar = maximum >= minimum ? maximum : QVariant();
                errors.addError<RangeConstraintViolationError>(path, v, minimum, maxVar);
            }
            return v;
        }

        double maximum;
        double minimum;
    };

    template <typename K, typename T, size_t N>
    struct toJsonEnumHelper {
        toJsonEnumHelper(const std::array<QPair<K, T>, N> &enumValues) : enumValues(enumValues) {
        }

        QJsonValue operator()(T value, SerializationErrorList &errors, Serializer::Option options, const QString &path) const {
            auto it = std::ranges::find_if(enumValues, [value](const auto &pair) {
                return pair.second == value;
            });
            if (it == enumValues.end()) {
                if (!(options & Serializer::CheckError)) {
                    return {};
                }
                QVariantList a;
                for (auto &pair : enumValues) {
                    a.append(pair.second);
                }
                errors.addError<EnumConstraintViolationError>(path, value, a);
                return {};
            }
            return it->first;
        }

        std::array<QPair<K, T>, N> enumValues;
    };

    template <typename T, typename C>
    QJsonArray toJsonArrayHelper(const QList<T> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonArray array;
        for (auto it = entity.begin(); it != entity.end(); ++it) {
            auto index = std::distance(entity.begin(), it);
            array.append(C::template toJson<T>(*it, errors, options, path + "[" + QString::number(index) + "]"));
        }
        return array;
    }

    template <typename C = void>
    QJsonArray toJsonArrayHelper(const QList<int> &entity, SerializationErrorList &errors, Serializer::Option options, const QString &path) {
        QJsonArray array;
        for (auto it = entity.begin(); it != entity.end(); ++it) {
            array.append(*it);
        }
        return array;
    }

}

#endif //OPENDSPX_SERIALIZER_CONVERTERHELPERTOJSON_P_H
