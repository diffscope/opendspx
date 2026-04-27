#ifndef OPENDSPX_SERIALIZATION_DECLARATIVE_P_H
#define OPENDSPX_SERIALIZATION_DECLARATIVE_P_H

#include <optional>

#include <opendspxserializer/private/helpers_p.h>

namespace opendspx::impl::decl {

    template <typename T>
    struct member_pointer_traits;

    template <typename M, typename C>
    struct member_pointer_traits<M C::*> {
        using member_type = M;
        using class_type = C;
    };

    template <typename M, typename C>
    struct member_pointer_traits<M C::* const> {
        using member_type = M;
        using class_type = C;
    };

    template <typename T>
    struct is_vector : std::false_type {};

    template <typename T, typename Alloc>
    struct is_vector<std::vector<T, Alloc>> : std::true_type {};

    template <typename T>
    struct is_shared_ptr : std::false_type {};

    template <typename T>
    struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

    template <typename E>
    struct Mapping;

    template <typename T>
    concept IsMapped = requires {
        typename Mapping<T>::type;
    };

    struct TrivialOrMappingConvert {
        template <typename T_>
        static constexpr auto getFromJsonFunc() {
            using T = std::remove_cvref_t<T_>;
            if constexpr (std::is_same_v<T, nlohmann::json>) {
                return [](const nlohmann::json &object, T &entity, const JsonSerializationContext &) {
                    entity = object;
                    return true;
                };
            } else if constexpr (std::is_same_v<T, int>) {
                return fromJsonIntHelperWithConstraint<>;
            } else if constexpr (std::is_same_v<T, double>) {
                return fromJsonDoubleHelperWithConstraint<>;
            } else if constexpr (std::is_same_v<T, bool>) {
                return fromJsonBoolHelper;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return fromJsonStringHelper;
            } else if constexpr (IsMapped<T>) {
                return [](const nlohmann::json &object, T &entity, const JsonSerializationContext &context) {
                    return Mapping<T>::type::fromJson(object, entity, context);
                };
            } else if constexpr (is_vector<T>::value) {
                using E = typename T::value_type;
                return fromJsonArrayHelper<E, getFromJsonFunc<E>()>;
            } else if constexpr (is_shared_ptr<T>::value) {
                using E = typename T::element_type;
                static_assert(IsMapped<E>);
                return [](const nlohmann::json &object, T &entity, const JsonSerializationContext &context) {
                    return Mapping<E>::type::fromJson(object, entity, context);
                };
            } else {
                static_assert(sizeof(T) == 0, "Unsupported type");
            }
        }

        template<typename T_>
        static constexpr auto getToJsonFunc() {
            using T = std::remove_cvref_t<T_>;
            if constexpr (std::is_same_v<T, nlohmann::json>) {
                return [](nlohmann::json &object, const T &entity, const JsonSerializationContext &) {
                    object = entity;
                    return true;
                };
            } else if constexpr (std::is_same_v<T, int>) {
                return toJsonTrivial<T>;
            } else if constexpr (std::is_same_v<T, double>) {
                return toJsonTrivial<T>;
            } else if constexpr (std::is_same_v<T, bool>) {
                return toJsonTrivial<T>;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return toJsonTrivial<T>;
            } else if constexpr (IsMapped<T>) {
                return [](nlohmann::json &object, const T &entity, const JsonSerializationContext &context) {
                    return Mapping<T>::type::toJson(object, entity, context);
                };
            } else if constexpr (is_vector<T>::value) {
                using E = typename T::value_type;
                return toJsonArrayHelper<E, getToJsonFunc<E>()>;
            } else if constexpr (is_shared_ptr<T>::value) {
                using E = typename T::element_type;
                static_assert(IsMapped<E>);
                return [](nlohmann::json &object, const T &entity, const JsonSerializationContext &context) {
                    return Mapping<E>::type::toJson(object, entity, context);
                };
            } else {
                static_assert(sizeof(T) == 0, "Unsupported type");
            }
        }
    };

    template <auto minValue = std::nullopt, auto maxValue = std::nullopt>
    struct RangeConstraintConvert {
        template <typename T>
        static constexpr auto getFromJsonFunc() {
            if constexpr (std::is_same_v<T, int>) {
                return fromJsonIntHelperWithConstraint<minValue, maxValue>;
            } else if constexpr (std::is_same_v<T, double>) {
                return fromJsonDoubleHelperWithConstraint<minValue, maxValue>;
            } else {
                static_assert(sizeof(T) == 0, "Unsupported type");
            }
        }

        template <typename T>
        static constexpr auto getToJsonFunc() {
            return [] (nlohmann::json &json, const T &value, const JsonSerializationContext &context) {
                return toJsonNumberHelperWithConstraint<T, minValue, maxValue>(json, value, context);
            };
        }
    };

    template <auto minValue = std::nullopt, auto maxValue = std::nullopt>
    struct ArrayRangeConstraintConvert {
        template <typename T_>
        static constexpr auto getFromJsonFunc() {
            using T = std::remove_cvref_t<T_>;
            if constexpr (std::is_same_v<typename T::value_type, int>) {
                return fromJsonArrayHelper<int, fromJsonIntHelperWithConstraint<minValue, maxValue>>;
            } else if constexpr (std::is_same_v<typename T::value_type, double>) {
                return fromJsonArrayHelper<double, fromJsonDoubleHelperWithConstraint<minValue, maxValue>>;
            } else {
                static_assert(sizeof(T) == 0, "Unsupported type");
            }
        }

        template <typename T_>
        static constexpr auto getToJsonFunc() {
            return toJsonArrayHelper<typename T_::value_type, toJsonNumberHelperWithConstraint<typename T_::value_type, minValue, maxValue>>;
        }
    };

    template <auto enumKey_, auto enumValue_>
    struct EnumEntry {
        static constexpr auto enumKey = enumKey_;
        static constexpr auto enumValue = enumValue_;
    };

    template <typename K, typename T, typename... EnumEntries>
    struct EnumDef {
        static constexpr std::array<std::pair<K, T>, sizeof...(EnumEntries)> toArray() {
            return {std::pair<K, T>{K(EnumEntries::enumKey), T(EnumEntries::enumValue)}...};
        }
    };

    template <typename E>
    struct EnumConstraintConvert {
        template <typename T>
        static constexpr auto getFromJsonFunc() {
            return fromJsonEnumHelper(E::toArray());
        }

        template <typename T>
        static constexpr auto getToJsonFunc() {
            return toJsonEnumHelper(E::toArray());
        }
    };

    template<size_t N>
    struct FixedString {
        char str[N];
        constexpr FixedString(const char (&str_)[N]) {
            std::copy_n(str_, N, str);
        }
        constexpr operator const char*() const {
            return str;
        }
    };

    template <auto propertyPtr_, FixedString propertyName_, typename Convert = TrivialOrMappingConvert>
    struct Property {
        static constexpr auto propertyPtr = propertyPtr_;
        static constexpr auto propertyName = propertyName_.str;
        using EntityType = typename member_pointer_traits<decltype(propertyPtr)>::class_type;
        using PropertyType = typename member_pointer_traits<decltype(propertyPtr)>::member_type;

        static bool fromJson(const nlohmann::json &object, EntityType &entity, const JsonSerializationContext &context) {
            if (!object.contains(propertyName)) {
                return false;
            }
            const auto &v = object.at(propertyName);
            bool ok = Convert::template getFromJsonFunc<PropertyType>()(v, entity.*propertyPtr, {context.errors, context.options, context.path + "." + propertyName});
            if (!ok && (context.options & Serializer::FailFast)) {
                return false;
            }
            return ok;
        }
        static bool toJson(nlohmann::json &object, const EntityType &entity, const JsonSerializationContext &context) {
            const auto &v = entity.*propertyPtr;
            bool ok = Convert::template getToJsonFunc<PropertyType>()(object[propertyName], v, {context.errors, context.options, context.path + "." + propertyName});
            if (!ok && (context.options & Serializer::FailFast)) {
                return false;
            }
            return ok;
        }
    };

    template <typename T, typename... PropertyDecls>
    struct Entity {
        static bool fromJson(const nlohmann::json &object_, T &entity, const JsonSerializationContext &context) {
            nlohmann::json object;
            bool ok = fromJsonObjectHelperWithPropertyCheck(std::array{PropertyDecls::propertyName...})(object_, object, context);
            if (!ok && (context.options & Serializer::FailFast)) {
                return false;
            }
            if (context.options & Serializer::FailFast)
                return (PropertyDecls::fromJson(object, entity, context) && ...);
            return (PropertyDecls::fromJson(object, entity, context), ...);
        }
        static bool fromJson(const nlohmann::json &object, std::shared_ptr<T> &entity, const JsonSerializationContext &context) {
            if (!entity) {
                entity = std::make_shared<T>();
            }
            return fromJson(object, *entity, context);
        }
        static bool toJson(nlohmann::json &object, const T &entity, const JsonSerializationContext &context) {
            object = nlohmann::json::object();
            if (context.options & Serializer::FailFast)
                return (PropertyDecls::toJson(object, entity, context) && ...);
            return (PropertyDecls::toJson(object, entity, context), ...);
        }
        static bool toJson(nlohmann::json &object, const std::shared_ptr<T> &entity, const JsonSerializationContext &context) {
            return toJson(object, *entity, context);
        }
    };

    template <auto objectTypeEnum_, FixedString objectTypeName_, typename T>
    struct Derive {
        using EntityType = T;
        static constexpr auto objectTypeName = objectTypeName_.str;
        static constexpr auto objectTypeEnum = objectTypeEnum_;
    };

    template <typename T, auto objectTypePropertyPtr, FixedString objectTypePropertyName_, typename... DeriveDecls>
    struct BaseEntity {
        static constexpr auto objectTypePropertyName = objectTypePropertyName_.str;
        static bool fromJson(const nlohmann::json &, T &, const JsonSerializationContext &) {
            static_assert(sizeof(T) == 0, "BaseEntity::fromJson with T & not supported");
        }
        static bool fromJson(const nlohmann::json &object_, std::shared_ptr<T> &entity, const JsonSerializationContext &context) {
            nlohmann::json object;
            bool ok = fromJsonObjectHelper(object_, object, context);
            if (!ok) {
                // Ignore fail-fast, because even if fail-fast is disabled, nothing can be done
                return false;
            }
            if (!object.contains(objectTypePropertyName)) {
                if (context.options & Serializer::CheckError) {
                    context.errors.addError<MissingPropertyError>(context.path, std::vector<std::string>{objectTypePropertyName});
                    return false;
                } else {
                    // Cannot determine object type, so here just do nothing and return
                    return true;
                }
            }
            auto objectTypeName = object[objectTypePropertyName].template get<std::string>();
            bool fwdRet = false;
            bool found = ((DeriveDecls::objectTypeName == objectTypeName ? ([&] {
                typename DeriveDecls::EntityType e;
                fwdRet = Mapping<typename DeriveDecls::EntityType>::type::fromJson(object, e, context);
                entity = std::make_shared<typename DeriveDecls::EntityType>(std::move(e));
            }(), true) : false) || ...);
            if (!found) {
                if (context.options & Serializer::CheckError) {
                    context.errors.addError<InvalidObjectTypeError>(context.path, objectTypeName, std::vector<std::string>{DeriveDecls::objectTypeName...});
                    return false;
                } else {
                    return true;
                }
            }
            return fwdRet;
        }
        static bool toJson(nlohmann::json &object, const T &entity, const JsonSerializationContext &context) {
            auto objectTypeEnum = entity.*objectTypePropertyPtr;
            bool fwdRet = false;
            bool found = ((DeriveDecls::objectTypeEnum == objectTypeEnum ? ([&] {
                fwdRet = Mapping<typename DeriveDecls::EntityType>::type::toJson(object, static_cast<const typename DeriveDecls::EntityType &>(entity), context);
            }(), true) : false) || ...);
            if (!found) {
                assert(false && "Unknown object type enum");
            }
            return fwdRet;
        }
        static bool toJson(nlohmann::json &object, const std::shared_ptr<T> &entity, const JsonSerializationContext &context) {
            return toJson(object, *entity, context);
        }
    };

    template <typename T>
    struct MapEntity {
        static bool fromJson(const nlohmann::json &object_, T &entity, const JsonSerializationContext &context) {
            nlohmann::json object;
            bool ok = fromJsonObjectHelper(object_, object, context);
            if (!ok) {
                // Ignore fail-fast, because even if fail-fast is disabled, nothing can be done
                return false;
            }
            for (auto it = object.begin(); it != object.end(); ++it) {
                typename T::mapped_type contentEntity;
                if (!TrivialOrMappingConvert::getFromJsonFunc<typename T::mapped_type>()(it.value(), contentEntity, context)) {
                    if (context.options & Serializer::FailFast)
                        return false;
                }
                entity[it.key()] = contentEntity;
            }
            return true;
        }
        static bool fromJson(const nlohmann::json &object, std::shared_ptr<T> &entity, const JsonSerializationContext &context) {
            if (!entity) {
                entity = std::make_shared<T>();
            }
            return fromJson(object, *entity, context);
        }
        static bool toJson(nlohmann::json &object, const T &entity, const JsonSerializationContext &context) {
            object = nlohmann::json::object();
            for (const auto &[key, value] : entity) {
                nlohmann::json contentJson;
                if (!TrivialOrMappingConvert::getToJsonFunc<decltype(value)>()(contentJson, value, context)) {
                    if (context.options & Serializer::FailFast)
                        return false;
                }
                object[key] = contentJson;
            }
            return true;
        }
        static bool toJson(nlohmann::json &object, const std::shared_ptr<T> &entity, const JsonSerializationContext &context) {
            return toJson(object, *entity, context);
        }
    };

}

#endif //OPENDSPX_SERIALIZATION_DECLARATIVE_P_H