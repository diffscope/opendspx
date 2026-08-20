#ifndef OPENDSPX_SERIALIZER_HELPERS_P_H
#define OPENDSPX_SERIALIZER_HELPERS_P_H

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include <opendspx/serializer/serializationerror.h>
#include <opendspx/serializer/serializer.h>

#include <stdcorelib/support/json.h>

namespace opendspx::impl {

    struct JsonSerializationContext {
        SerializationErrorList &errors;
        Serializer::Option options;
        std::string path;
    };

    template <typename T>
    bool toJsonTrivial(stdc::json::Value &json, const T &value, const JsonSerializationContext &) {
        json = value;
        return true;
    }

    template <typename T, auto minimum_ = std::nullopt, auto maximum_ = std::nullopt>
    bool toJsonNumberHelperWithConstraint(stdc::json::Value &json, const T &value, const JsonSerializationContext &context) {
        const std::optional<T> minimum = minimum_;
        const std::optional<T> maximum = maximum_;
        json = 0;
        if (!(context.options & Serializer::CheckError)) {
            json = value;
            return true;
        }
        bool ok = true;
        if (minimum.has_value() && maximum.has_value()) {
            if (value < minimum || value > maximum) {
                context.errors.addError<RangeConstraintViolationError>(context.path, value, minimum.value(), maximum.value());
                ok = false;
            }
        } else if (minimum.has_value()) {
            if (value < minimum) {
                context.errors.addError<RangeConstraintViolationError>(context.path, value, minimum.value(), std::any());
                ok = false;
            }
        } else if (maximum.has_value()) {
            if (value > maximum) {
                context.errors.addError<RangeConstraintViolationError>(context.path, value, std::any(), maximum.value());
                ok = false;
            }
        }
        json = value;
        return ok;
    }

    template <typename K, typename T, size_t N>
    struct toJsonEnumHelper {
        explicit toJsonEnumHelper(const std::array<std::pair<K, T>, N> &enumValues) : enumValues(enumValues) {
        }

        bool operator()(stdc::json::Value &json, const T &value, const JsonSerializationContext &context) const {
            auto it = std::ranges::find_if(enumValues, [value](const auto &pair) {
                return pair.second == value;
            });
            if (it == enumValues.end()) {
                json = stdc::json::Value();
                if (!(context.options & Serializer::CheckError)) {
                    return true;
                }
                std::vector<std::any> a;
                for (auto &pair : enumValues) {
                    a.push_back(pair.second);
                }
                context.errors.addError<EnumConstraintViolationError>(context.path, value, std::move(a));
                return false;
            }
            json = it->first;
            return true;
        }

        std::array<std::pair<K, T>, N> enumValues;
    };

    template <typename T, auto toJson = toJsonTrivial<T>>
    bool toJsonArrayHelper(stdc::json::Value &json, const std::vector<T> &entity, const JsonSerializationContext &context) {
        stdc::json::Array array;
        array.reserve(entity.size());
        bool ok = true;
        for (auto it = entity.begin(); it != entity.end(); ++it) {
            auto index = std::distance(entity.begin(), it);
            stdc::json::Value item;
            ok = toJson(item, *it, JsonSerializationContext{context.errors, context.options, context.path + "[" + std::to_string(index) + "]"}) && ok;
            if ((context.options & Serializer::FailFast) && !ok) {
                json = std::move(array);
                return false;
            }
            array.push_back(std::move(item));
        }
        json = std::move(array);
        if (!(context.options & Serializer::CheckError))
            return true;
        return ok;
    }

}

namespace opendspx::impl {

    inline bool isInteger(double v) {
        return std::isfinite(v) && std::trunc(v) == v && v >= std::numeric_limits<int>::min() && v <= std::numeric_limits<int>::max();
    }

    inline InvalidDataTypeError::DataType getDataType(const stdc::json::Value &value) {
        switch (value.type()) {
            case stdc::json::Type::Bool:
                return InvalidDataTypeError::Bool;
            case stdc::json::Type::Int:
            case stdc::json::Type::Double:
                return isInteger(value.toDouble()) ? InvalidDataTypeError::Integer : InvalidDataTypeError::Double;
            case stdc::json::Type::String:
                return InvalidDataTypeError::String;
            case stdc::json::Type::Array:
                return InvalidDataTypeError::Array;
            case stdc::json::Type::Object:
                return InvalidDataTypeError::Object;
            default:
                return InvalidDataTypeError::Null;
        }
    }

    inline bool fromJsonStringHelper(const stdc::json::Value &value, std::string &out, const JsonSerializationContext &context) {
        if (!(context.options & Serializer::CheckError)) {
            out = value.toString();
            return true;
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::String) {
            context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::String});
            out = {};
            return false;
        }
        out = value.toString();
        return true;
    }

    template <auto minimum_ = std::nullopt, auto maximum_ = std::nullopt>
    bool fromJsonDoubleHelperWithConstraint(const stdc::json::Value &value, double &out, const JsonSerializationContext &context) {
        const std::optional<double> minimum = minimum_;
        const std::optional<double> maximum = maximum_;
        if (!(context.options & Serializer::CheckError)) {
            out = value.toDouble();
            return true;
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Double && actualType != InvalidDataTypeError::Integer) {
            context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Double, InvalidDataTypeError::Integer});
            out = {};
            return false;
        }
        auto v = value.toDouble();
        bool ok = true;
        if (minimum.has_value() && maximum.has_value()) {
            if (v < minimum || v > maximum) {
                context.errors.addError<RangeConstraintViolationError>(context.path, v, minimum.value(), maximum.value());
                ok = false;
            }
        } else if (minimum.has_value()) {
            if (v < minimum) {
                context.errors.addError<RangeConstraintViolationError>(context.path, v, minimum.value(), std::any());
                ok = false;
            }
        } else if (maximum.has_value()) {
            if (v > maximum) {
                context.errors.addError<RangeConstraintViolationError>(context.path, v, std::any(), maximum.value());
                ok = false;
            }
        }
        out = v;
        return ok;
    }

    template <auto minimum_ = std::nullopt, auto maximum_ = std::nullopt>
    bool fromJsonIntHelperWithConstraint(const stdc::json::Value &value, int &out, const JsonSerializationContext &context) {
        const std::optional<int> minimum = minimum_;
        const std::optional<int> maximum = maximum_;
        if (!(context.options & Serializer::CheckError)) {
            out = static_cast<int>(value.toInt());
            return true;
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Integer) {
            context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Integer});
            out = {};
            return false;
        }
        auto v = static_cast<int>(value.toInt());
        bool ok = true;
        if (minimum.has_value() && maximum.has_value()) {
            if (v < minimum || v > maximum) {
                context.errors.addError<RangeConstraintViolationError>(context.path, v, minimum.value(), maximum.value());
                ok = false;
            }
        } else if (minimum.has_value()) {
            if (v < minimum) {
                context.errors.addError<RangeConstraintViolationError>(context.path, v, minimum.value(), std::any());
                ok = false;
            }
        } else if (maximum.has_value()) {
            if (v > maximum) {
                context.errors.addError<RangeConstraintViolationError>(context.path, v, std::any(), maximum.value());
                ok = false;
            }
        }
        out = v;
        return ok;
    }

    inline bool fromJsonBoolHelper(const stdc::json::Value &value, bool &out, const JsonSerializationContext &context) {
        if (!(context.options & Serializer::CheckError)) {
            out = value.toBool();
            return true;
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Bool) {
            context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Bool});
            out = false;
            return false;
        }
        out = value.toBool();
        return true;
    }

    template <typename K, typename T, size_t N>
    struct fromJsonEnumHelper {
        static_assert(std::is_same_v<K, const char *> || std::is_same_v<K, int>);
        using EnumType = std::conditional_t<std::is_same_v<K, const char *>, std::string, int>;

        explicit fromJsonEnumHelper(const std::array<std::pair<K, T>, N> &enumValues) : enumValues(enumValues) {
        }

        static constexpr InvalidDataTypeError::DataType Flag = std::is_same_v<K, const char *> ? InvalidDataTypeError::String : InvalidDataTypeError::Integer;

        static EnumType enumKeyOf(const stdc::json::Value &value) {
            if constexpr (std::is_same_v<EnumType, std::string>) {
                return value.toString();
            } else {
                return static_cast<int>(value.toInt());
            }
        }

        bool operator()(const stdc::json::Value &value, T &out, const JsonSerializationContext &context) const {
            if (!(context.options & Serializer::CheckError)) {
                if (auto actualType = getDataType(value); actualType != Flag) {
                    out = {};
                    return true;
                }
                auto s = enumKeyOf(value);
                auto it = std::ranges::find_if(enumValues, [&s](const auto &pair) {
                    return pair.first == s;
                });
                if (it == enumValues.end()) {
                    out = {};
                    return true;
                }
                out = it->second;
                return true;
            }

            if (auto actualType = getDataType(value); actualType != Flag) {
                context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{Flag});
                out = {};
                return false;
            }
            auto s = enumKeyOf(value);
            auto it = std::ranges::find_if(enumValues, [&s](const auto &pair) {
                return pair.first == s;
            });
            bool ok = it != enumValues.end();
            out = ok ? it->second : T{};

            if (!ok) {
                std::vector<std::any> a;
                for (auto &pair : enumValues) {
                    a.push_back(pair.first);
                }
                context.errors.addError<EnumConstraintViolationError>(context.path, out, std::move(a));
                return false;
            }
            return true;
        }

        std::array<std::pair<K, T>, N> enumValues;
    };

    template <typename T, auto fromJson>
    bool fromJsonArrayHelper(const stdc::json::Value &json, std::vector<T> &list, const JsonSerializationContext &context) {
        list.clear();
        auto &errors = context.errors;
        auto options = context.options;
        if (auto actualType = getDataType(json); (options & Serializer::CheckError) && actualType != InvalidDataTypeError::Array) {
            errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Array});
            return false;
        }
        const auto &array = json.toArray();
        bool ok = true;
        for (auto it = array.begin(); it != array.end(); ++it) {
            auto index = std::distance(array.begin(), it);
            T v{};
            ok = fromJson(*it, v, JsonSerializationContext{context.errors, context.options, context.path + "[" + std::to_string(index) + "]"}) && ok;
            if ((options & Serializer::FailFast) && errors.containsError()) {
                return false;
            }
            list.push_back(std::move(v));
        }
        if (!(context.options & Serializer::CheckError))
            return true;
        return ok;
    }

    // For a property whose declared type is an object rather than any value, so the object has to
    // be handed over rather than pointed at
    inline bool fromJsonObjectValueHelper(const stdc::json::Value &value, stdc::json::Object &out, const JsonSerializationContext &context) {
        if (!(context.options & Serializer::CheckError)) {
            out = value.toObject();
            return true;
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Object) {
            context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Object});
            out = {};
            return false;
        }
        out = value.toObject();
        return true;
    }

    // The object is handed back by pointer rather than by value, because a json::Value owns its
    // children and copying one here would copy the whole subtree at every level of nesting.
    // A value that is not an object yields the shared empty object, so the caller never sees null.
    inline bool fromJsonObjectHelper(const stdc::json::Value &value, const stdc::json::Object *&out, const JsonSerializationContext &context) {
        out = &value.toObject();
        if (!(context.options & Serializer::CheckError)) {
            return true;
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Object) {
            context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Object});
            return false;
        }
        return true;
    }

    template <size_t N>
    struct fromJsonObjectHelperWithPropertyCheck {
        explicit fromJsonObjectHelperWithPropertyCheck(std::array<const char *, N> &&properties) : properties(properties) {
        }

        bool operator()(const stdc::json::Value &value, const stdc::json::Object *&out, const JsonSerializationContext &context) const {
            out = &value.toObject();
            if (!(context.options & Serializer::CheckError)) {
                return true;
            }
            if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Object) {
                context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Object});
                return false;
            }
            const auto &obj = *out;
            std::vector<std::string> missingProperties;
            std::vector<std::string> redundantProperties;
            for (auto &property : properties) {
                if (!obj.contains(property)) {
                    missingProperties.push_back(property);
                }
            }
            if (!missingProperties.empty()) {
                context.errors.addError<MissingPropertyError>(context.path, std::move(missingProperties));
            }
            if (!(context.options & Serializer::TolerateRedundantProperty) && obj.size() + missingProperties.size() > properties.size()) {
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    if (!std::ranges::any_of(properties, [it](const auto &property) {
                            return it->first == property;
                        })) {
                        redundantProperties.push_back(it->first);
                    }
                }
                context.errors.addError<RedundantPropertyError>(context.path, std::move(redundantProperties));
            }
            return missingProperties.empty() && ((context.options & Serializer::TolerateRedundantProperty) || redundantProperties.empty());
        }

        std::array<const char *, N> properties;
    };

}

#endif //OPENDSPX_SERIALIZER_HELPERS_P_H
