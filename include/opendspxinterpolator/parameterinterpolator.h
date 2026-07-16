#ifndef OPENDSPX_INTERPOLATOR_PARAMETERINTERPOLATOR_H
#define OPENDSPX_INTERPOLATOR_PARAMETERINTERPOLATOR_H

#include <algorithm>
#include <optional>
#include <utility>
#include <vector>

#include <opendspx/anchornode.h>
#include <opendspxinterpolator/interpolator.h>

namespace opendspx {

    class ParameterInterpolator {
    public:
        explicit ParameterInterpolator(std::vector<AnchorNode> nodes) : m_nodes(std::move(nodes)) {
            std::ranges::stable_sort(m_nodes, [](const AnchorNode &lhs, const AnchorNode &rhs) {
                return lhs.x < rhs.x;
            });
        }

        std::optional<double> evaluate(double x) const {
            const auto right = std::lower_bound(
                m_nodes.begin(), m_nodes.end(), x,
                [](const AnchorNode &node, double value) {
                    return node.x < value;
                });

            if (right != m_nodes.end() && right->x == x) {
                return static_cast<double>(right->y);
            }
            if (right == m_nodes.begin() || right == m_nodes.end()) {
                return std::nullopt;
            }

            const auto left = right - 1;
            switch (left->interp) {
                case AnchorNode::Interpolation::None:
                    return std::nullopt;
                case AnchorNode::Interpolation::Linear:
                    return createLinear(*left, *right).evaluate(x);
                case AnchorNode::Interpolation::Hermite:
                    return createHermite(left, right).evaluate(x);
            }

            return std::nullopt;
        }

    private:
        using NodeIterator = std::vector<AnchorNode>::const_iterator;

        static Interpolator<double> createLinear(const AnchorNode &left, const AnchorNode &right) {
            return Interpolator<double>::createLinear(left.x, left.y, right.x, right.y);
        }

        Interpolator<double> createHermite(NodeIterator left, NodeIterator right) const {
            const bool hasLeftReference = left != m_nodes.begin();
            const bool hasRightReference = right + 1 != m_nodes.end();

            if (hasLeftReference && hasRightReference) {
                const auto leftReference = left - 1;
                const auto rightReference = right + 1;
                return Interpolator<double>::create(
                    left->x, left->y, right->x, right->y,
                    leftReference->x, leftReference->y, rightReference->x, rightReference->y);
            }
            if (hasLeftReference) {
                const auto leftReference = left - 1;
                return Interpolator<double>::createWithRef1Only(
                    left->x, left->y, right->x, right->y, leftReference->x, leftReference->y);
            }
            if (hasRightReference) {
                const auto rightReference = right + 1;
                return Interpolator<double>::createWithRef2Only(
                    left->x, left->y, right->x, right->y, rightReference->x, rightReference->y);
            }
            return createLinear(*left, *right);
        }

        std::vector<AnchorNode> m_nodes;
    };

}

#endif // OPENDSPX_INTERPOLATOR_PARAMETERINTERPOLATOR_H
