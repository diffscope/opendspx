#ifndef OPENDSPX_INTERPOLATOR_MIXINTERPOLATOR_H
#define OPENDSPX_INTERPOLATOR_MIXINTERPOLATOR_H

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include <opendspx/dynamicmixinganchor.h>
#include <opendspx/sourcemixingratio.h>

namespace opendspx {

    class MixInterpolator {
    public:
        MixInterpolator(std::vector<DynamicMixingAnchor> anchors, std::size_t count)
            : m_anchors(std::move(anchors)), m_count(count) {
            if (m_count == 0) {
                throw std::invalid_argument("MixInterpolator requires at least one source");
            }
            if (std::ranges::any_of(m_anchors, [this](const DynamicMixingAnchor &anchor) {
                    return anchor.ratio.size() + 1 != m_count;
                })) {
                throw std::invalid_argument("Dynamic mixing anchor ratio count does not match source count");
            }

            std::ranges::stable_sort(m_anchors, [](const DynamicMixingAnchor &lhs, const DynamicMixingAnchor &rhs) {
                return lhs.pos < rhs.pos;
            });
        }

        SourceMixingRatio evaluate(double pos) const {
            if (m_anchors.empty()) {
                return SourceMixingRatio(m_count - 1, 1.0 / static_cast<double>(m_count));
            }

            const auto right = std::lower_bound(
                m_anchors.begin(), m_anchors.end(), pos,
                [](const DynamicMixingAnchor &anchor, double value) {
                    return anchor.pos < value;
                });

            if (right == m_anchors.begin()) {
                return right->ratio;
            }
            if (right == m_anchors.end()) {
                return m_anchors.back().ratio;
            }
            if (right->pos == pos) {
                return right->ratio;
            }

            const auto left = right - 1;
            const double factor = (pos - left->pos) / (right->pos - left->pos);
            SourceMixingRatio result = left->ratio;
            for (std::size_t i = 0; i < result.size(); ++i) {
                result[i] += (right->ratio[i] - left->ratio[i]) * factor;
            }
            return result;
        }

    private:
        std::vector<DynamicMixingAnchor> m_anchors;
        std::size_t m_count;
    };

}

#endif // OPENDSPX_INTERPOLATOR_MIXINTERPOLATOR_H
