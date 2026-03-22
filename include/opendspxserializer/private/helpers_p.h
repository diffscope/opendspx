#ifndef OPENDSPX_SERIALIZATION_HELPERS_P_H
#define OPENDSPX_SERIALIZATION_HELPERS_P_H

#include <nlohmann/json.hpp>

#include <opendspxserializer/serializer.h>
#include <opendspxserializer/serializationerror.h>

namespace opendspx::impl {

    struct JsonSerializationContext {
        SerializationErrorList &errors;
        Serializer::Option options;
        std::string path;
    };

    template <typename T>
    bool toJsonTrivial(nlohmann::json &json, const T &value, const JsonSerializationContext &) {
        json = value;
        return true;
    }

    struct toJsonNumberHelperWithConstraint {
        explicit toJsonNumberHelperWithConstraint(std::optional<double> minimum = {}, std::optional<double> maximum = {}) : maximum(maximum), minimum(minimum) {
        }

        template <typename T>
        bool operator()(nlohmann::json &json, const T &value, const JsonSerializationContext &context) const {
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

        std::optional<double> maximum;
        std::optional<double> minimum;
    };

    template <typename K, typename T, size_t N>
    struct toJsonEnumHelper {
        explicit toJsonEnumHelper(const std::array<std::pair<K, T>, N> &enumValues) : enumValues(enumValues) {
        }

        bool operator()(nlohmann::json &json, const T &value, const JsonSerializationContext &context) const {
            auto it = std::ranges::find_if(enumValues, [value](const auto &pair) {
                return pair.second == value;
            });
            if (it == enumValues.end()) {
                json = {};
                if (!(context.options & Serializer::CheckError)) {
                    return false;
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
    bool toJsonArrayHelper(nlohmann::json &json, const std::vector<T> &entity, const JsonSerializationContext &context) {
        nlohmann::json array = nlohmann::json::array();
        bool ok = true;
        for (auto it = entity.begin(); it != entity.end(); ++it) {
            auto index = std::distance(entity.begin(), it);
            nlohmann::json item;
            ok = toJson(item, *it, JsonSerializationContext{context.errors, context.options, context.path + "[" + std::to_string(index) + "]"}) && ok;
            array.push_back(std::move(item));
            if ((context.options & Serializer::FailFast) && context.errors.containsError()) {
                json = std::move(array);
                return false;
            }
        }
        json = std::move(array);
        return ok;
    }

}

namespace opendspx::impl {

    inline bool isInteger(double v) {
        return std::isfinite(v) && std::trunc(v) == v && v >= std::numeric_limits<int>::min() && v <= std::numeric_limits<int>::max();
    }

    inline InvalidDataTypeError::DataType getDataType(const nlohmann::json &value) {
        switch (value.type()) {
            case nlohmann::json::value_t::boolean:
                return InvalidDataTypeError::Bool;
            case nlohmann::json::value_t::number_float:
            case nlohmann::json::value_t::number_integer:
            case nlohmann::json::value_t::number_unsigned:
                return isInteger(value.get<double>()) ? InvalidDataTypeError::Integer : InvalidDataTypeError::Double;
            case nlohmann::json::value_t::string:
                return InvalidDataTypeError::String;
            case nlohmann::json::value_t::array:
                return InvalidDataTypeError::Array;
            case nlohmann::json::value_t::object:
                return InvalidDataTypeError::Object;
            default:
                return InvalidDataTypeError::Null;
        }
    }

    inline bool fromJsonStringHelper(const nlohmann::json &value, std::string &out, const JsonSerializationContext &context) {
        if (!(context.options & Serializer::CheckError)) {
            out = value.is_string() ? value.get<std::string>() : std::string{};
            return value.is_string();
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::String) {
            context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::String});
            out = {};
            return false;
        }
        out = value.get<std::string>();
        return true;
    }

    struct fromJsonDoubleHelperWithConstraint {
        explicit fromJsonDoubleHelperWithConstraint(std::optional<double> minimum = {}, std::optional<double> maximum = {}) : maximum(maximum), minimum(minimum) {
        }

        bool operator()(const nlohmann::json &value, double &out, const JsonSerializationContext &context) const {
            if (!(context.options & Serializer::CheckError)) {
                out = value.is_number() ? value.get<double>() : 0;
                return value.is_number();
            }
            if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Double && actualType != InvalidDataTypeError::Integer) {
                context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Double, InvalidDataTypeError::Integer});
                out = {};
                return false;
            }
            auto v = value.get<double>();
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

        std::optional<double> maximum;
        std::optional<double> minimum;
    };

    struct fromJsonIntHelperWithConstraint {
        explicit fromJsonIntHelperWithConstraint(std::optional<int> minimum = {}, std::optional<int> maximum = {}) : maximum(maximum), minimum(minimum) {
        }

        bool operator()(const nlohmann::json &value, int &out, const JsonSerializationContext &context) const {
            if (!(context.options & Serializer::CheckError)) {
                out = value.is_number() ? value.get<int>() : 0;
                return value.is_number();
            }
            if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Integer) {
                context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Integer});
                out = {};
                return false;
            }
            auto v = value.get<int>();
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

        std::optional<int> maximum;
        std::optional<int> minimum;
    };

    inline bool fromJsonBoolHelper(const nlohmann::json &value, bool &out, const JsonSerializationContext &context) {
        if (!(context.options & Serializer::CheckError)) {
            out = value.is_boolean() ? value.get<bool>() : false;
            return value.is_boolean();
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Bool) {
            context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Bool});
            out = false;
            return false;
        }
        out = value.get<bool>();
        return true;
    }

    template <typename K, typename T, size_t N>
    struct fromJsonEnumHelper {
        static_assert(std::is_same_v<K, const char *> || std::is_same_v<K, int>);
        using EnumType = std::conditional_t<std::is_same_v<K, const char *>, std::string, int>;

        explicit fromJsonEnumHelper(const std::array<std::pair<K, T>, N> &enumValues) : enumValues(enumValues) {
        }

        static constexpr InvalidDataTypeError::DataType Flag = std::is_same_v<K, const char *> ? InvalidDataTypeError::String : InvalidDataTypeError::Integer;

        bool operator()(const nlohmann::json &value, T &out, const JsonSerializationContext &context) const {
            if (!(context.options & Serializer::CheckError)) {
                if (auto actualType = getDataType(value); actualType != Flag) {
                    out = {};
                    return false;
                }
                auto s = value.get<EnumType>();
                auto it = std::ranges::find_if(enumValues, [s](const auto &pair) {
                    return pair.first == s;
                });
                if (it == enumValues.end()) {
                    out = {};
                    return false;
                }
                out = it->second;
                return true;
            }

            if (auto actualType = getDataType(value); actualType != Flag) {
                context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{Flag});
                out = {};
                return false;
            }
            auto s = value.get<EnumType>();
            auto it = std::ranges::find_if(enumValues, [s](const auto &pair) {
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
    bool fromJsonArrayHelper(const nlohmann::json &json, std::vector<T> &list, const JsonSerializationContext &context) {
        list.clear();
        auto &errors = context.errors;
        auto options = context.options;
        if (auto actualType = getDataType(json); (options & Serializer::CheckError) && actualType != InvalidDataTypeError::Array) {
            errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Array});
            return false;
        }
        const auto &array = json.is_array() ? json : nlohmann::json::array();
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
        return ok;
    }

    inline bool fromJsonObjectHelper(const nlohmann::json &value, nlohmann::json &out, const JsonSerializationContext &context) {
        if (!(context.options & Serializer::CheckError)) {
            out = value.is_object() ? value : nlohmann::json::object();
            return value.is_object();
        }
        if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Object) {
            context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Object});
            out = nlohmann::json::object();
            return false;
        }
        out = value;
        return true;
    }

    template <size_t N>
    struct fromJsonObjectHelperWithPropertyCheck {
        explicit fromJsonObjectHelperWithPropertyCheck(std::array<const char *, N> &&properties) : properties(properties) {
        }

        bool operator()(const nlohmann::json &value, nlohmann::json &out, const JsonSerializationContext &context) const {
            if (!(context.options & Serializer::CheckError)) {
                out = value.is_object() ? value : nlohmann::json::object();
                return value.is_object();
            }
            if (auto actualType = getDataType(value); actualType != InvalidDataTypeError::Object) {
                context.errors.addError<InvalidDataTypeError>(context.path, actualType, std::vector{InvalidDataTypeError::Object});
                out = nlohmann::json::object();
                return false;
            }
            const auto &obj = value;
            std::vector<std::string> missingProperties;
            for (auto &property : properties) {
                if (!obj.contains(property)) {
                    missingProperties.push_back(property);
                }
            }
            if (!missingProperties.empty()) {
                context.errors.addError<MissingPropertyError>(context.path, std::move(missingProperties));
            }
            if (!(context.options & Serializer::TolerateRedundantProperty) && obj.size() + missingProperties.size() > properties.size()) {
                std::vector<std::string> redundantProperties;
                for (auto it = obj.begin(); it != obj.end(); ++it) {
                    if (!std::ranges::any_of(properties, [it](const auto &property) {
                        return it.key() == property;
                    })) {
                        redundantProperties.push_back(it.key());
                    }
                }
                context.errors.addError<RedundantPropertyError>(context.path, std::move(redundantProperties));
            }
            out = obj;
            return missingProperties.empty() && ((context.options & Serializer::TolerateRedundantProperty) || obj.size() + missingProperties.size() <= properties.size());
        }

        std::array<const char *, N> properties;
    };

}

#endif //OPENDSPX_SERIALIZATION_HELPERS_P_H
