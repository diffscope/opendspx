#ifndef OPENDSPX_VIBRATO_VIBRATOCURVE_H
#define OPENDSPX_VIBRATO_VIBRATOCURVE_H

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include <opendspx/vibrato.h>

namespace opendspx {

    class VibratoCurve {
    public:
        VibratoCurve(Vibrato vibrato, double length)
            : m_vibrato(std::move(vibrato)),
              m_length(length),
              m_amplitudeScale(m_vibrato.points.amp),
              m_frequencyScale(m_vibrato.points.freq) {
        }

        double evaluate(double t) const {
            if (t < m_vibrato.start || t > m_vibrato.end) {
                return 0.0;
            }

            const double cycles = m_vibrato.freq * m_length * m_frequencyScale.integralFromZero(t);
            const double phase = twoPi * (cycles + m_vibrato.phase);
            return static_cast<double>(m_vibrato.amp) * m_amplitudeScale.valueAt(t) * std::sin(phase) + static_cast<double>(m_vibrato.offset);
        }

    private:
        class ScaleCurve {
        public:
            explicit ScaleCurve(std::vector<ControlPoint> points) : m_points(std::move(points)) {
                std::stable_sort(m_points.begin(), m_points.end(), [](const ControlPoint &lhs, const ControlPoint &rhs) {
                    return lhs.x < rhs.x;
                });

                std::vector<ControlPoint> uniquePoints;
                uniquePoints.reserve(m_points.size());
                for (const auto &point : m_points) {
                    if (!uniquePoints.empty() && uniquePoints.back().x == point.x) {
                        uniquePoints.back().y = point.y;
                    } else {
                        uniquePoints.push_back(point);
                    }
                }
                m_points = std::move(uniquePoints);
            }

            double valueAt(double t) const {
                if (m_points.empty()) {
                    return 1.0;
                }
                if (t <= m_points.front().x) {
                    return m_points.front().y;
                }
                if (t >= m_points.back().x) {
                    return m_points.back().y;
                }

                const auto right = std::upper_bound(
                    m_points.begin(), m_points.end(), t,
                    [](double value, const ControlPoint &point) {
                        return value < point.x;
                    });
                const auto left = right - 1;
                const double ratio = (t - left->x) / (right->x - left->x);
                return left->y + (right->y - left->y) * ratio;
            }

            double integralFromZero(double t) const {
                if (m_points.empty()) {
                    return t;
                }
                if (t >= 0.0) {
                    return integrateForward(0.0, t);
                }
                return -integrateForward(t, 0.0);
            }

        private:
            double integrateForward(double from, double to) const {
                if (!(from < to)) {
                    return 0.0;
                }

                double result = 0.0;
                double cursor = from;
                while (cursor < to) {
                    const auto right = std::upper_bound(
                        m_points.begin(), m_points.end(), cursor,
                        [](double value, const ControlPoint &point) {
                            return value < point.x;
                        });
                    const double next = right == m_points.end() ? to : std::min(to, right->x);
                    if (!(cursor < next)) {
                        cursor = next;
                        continue;
                    }

                    result += (valueAt(cursor) + valueAt(next)) * 0.5 * (next - cursor);
                    cursor = next;
                }
                return result;
            }

            std::vector<ControlPoint> m_points;
        };

        static constexpr double twoPi = 6.283185307179586476925286766559;

        Vibrato m_vibrato;
        double m_length;
        ScaleCurve m_amplitudeScale;
        ScaleCurve m_frequencyScale;
    };

}

#endif // OPENDSPX_VIBRATO_VIBRATOCURVE_H
