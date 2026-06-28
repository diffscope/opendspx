#ifndef OPENDSPX_SERIALIZER_SOURCEHELPERS_P_H
#define OPENDSPX_SERIALIZER_SOURCEHELPERS_P_H

#include <string>
#include <type_traits>
#include <vector>

#include <nlohmann/json.hpp>

#include <opendspx/mixedsinger.h>
#include <opendspx/sources.h>
#include <opendspx/sourcemixingratio.h>
#include <opendspxserializer/private/helpers_p.h>
#include <opendspxserializer/serializationerror.h>

namespace opendspx::impl {

	struct SourceMixingRatioConvert {
		template <typename T>
		static constexpr auto getFromJsonFunc() {
			if constexpr (std::is_same_v<T, SourceMixingRatio>) {
				return [](const nlohmann::json &value, SourceMixingRatio &out, const JsonSerializationContext &context) {
					std::vector<double> ratios;
					bool ok = fromJsonArrayHelper<double, fromJsonDoubleHelperWithConstraint<0, 1>>(value, ratios, context);
					out.assign(ratios.begin(), ratios.end());

					if (!(context.options & Serializer::CheckError)) {
						return true;
					}

					if (!out.valid()) {
						context.errors.addError<InvalidRatioPartitionError>(context.path, ratios);
						ok = false;
					}
					return ok;
				};
			} else {
				static_assert(sizeof(T) == 0, "Unsupported type");
			}
		}

		template <typename T>
		static constexpr auto getToJsonFunc() {
		    if constexpr (std::is_same_v<T, SourceMixingRatio>) {
		        return [](nlohmann::json &json, const SourceMixingRatio &value, const JsonSerializationContext &context) {
					std::vector<double> ratios(value.begin(), value.end());
					bool ok = toJsonArrayHelper<double, toJsonNumberHelperWithConstraint<double, 0, 1>>(json, ratios, context);

					if (!(context.options & Serializer::CheckError)) {
						return true;
					}

					if (!value.valid()) {
						context.errors.addError<InvalidRatioPartitionError>(context.path, ratios);
						ok = false;
					}
					return ok;
		        };
		    } else {
		        static_assert(sizeof(T) == 0, "Unsupported type");
		    }
		}
	};

    struct EntityWithRatioPostCheck {

        template <typename T>
        static bool fromJson(const nlohmann::json &object, T &entity, const JsonSerializationContext &context) {
			(void) object;
			return checkConstraint(entity, context);
        }

        template <typename T>
        static bool toJson(nlohmann::json &object, const T &entity, const JsonSerializationContext &context) {
			(void) object;
			return checkConstraint(entity, context);
        }

	private:
		static bool checkConstraint(const MixedSinger &entity, const JsonSerializationContext &context) {
			if (!(context.options & Serializer::CheckError)) {
				return true;
			}
			if (entity.singers.empty()) {
				context.errors.addError<EmptySingerMixingError>(context.path + ".singers");
				if (context.options & Serializer::FailFast) {
					return false;
				}
			}
			const auto expectedPartCount = static_cast<int>(entity.singers.size());
			const auto actualPartCount = static_cast<int>(entity.ratio.size()) + 1;
			if (expectedPartCount == actualPartCount) {
				return true;
			}
			context.errors.addError<PartCountNotMatchError>(context.path + ".ratio", expectedPartCount, actualPartCount);
			return false;
		}

		static bool checkConstraint(const Sources &entity, const JsonSerializationContext &context) {
			if (!(context.options & Serializer::CheckError)) {
				return true;
			}
			if (entity.singers.empty()) {
				context.errors.addError<EmptySingerMixingError>(context.path + ".singers");
				if (context.options & Serializer::FailFast) {
					return false;
				}
			}
			bool ok = true;
			const auto expectedPartCount = static_cast<int>(entity.singers.size());
			for (auto it = entity.mix.begin(); it != entity.mix.end(); ++it) {
				const auto index = std::distance(entity.mix.begin(), it);
				const auto actualPartCount = static_cast<int>(it->ratio.size()) + 1;
				if (expectedPartCount != actualPartCount) {
					context.errors.addError<PartCountNotMatchError>(context.path + ".mix[" + std::to_string(index) + "].ratio", expectedPartCount, actualPartCount);
					ok = false;
					if (context.options & Serializer::FailFast) {
						return false;
					}
				}
			}
			return ok;
		}

		template <typename T>
		static bool checkConstraint(const T &, const JsonSerializationContext &) {
			static_assert(sizeof(T) == 0, "Unsupported type");
			return false;
		}
    };

}

#endif // OPENDSPX_SERIALIZER_SOURCEHELPERS_P_H