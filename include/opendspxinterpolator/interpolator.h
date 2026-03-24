#ifndef OPENDSPX_INTERPOLATOR_INTERPOLATOR_H
#define OPENDSPX_INTERPOLATOR_INTERPOLATOR_H

#include <array>
#include <cmath>

namespace opendspx {

    template <typename T>
    class Interpolator {
    public:
        static constexpr Interpolator create(T x1, T y1, T x2, T y2, T xref1, T yref1, T xref2, T yref2);
        static constexpr Interpolator createWithRef1Only(T x1, T y1, T x2, T y2, T xref1, T yref1);
        static constexpr Interpolator createWithRef2Only(T x1, T y1, T x2, T y2, T xref2, T yref2);
        static constexpr Interpolator createLinear(T x1, T y1, T x2, T y2);

        constexpr T evaluate(T x) const;

    private:
        std::array<T, 4> m_coeffs;

        constexpr Interpolator(std::array<T, 4> coeffs) : m_coeffs(coeffs) {
        }
    };

    namespace impl {
        template <typename T>
        constexpr std::array<T, 4> calculateHermiteCoefficients(T x1, T y1, T d1, T x2, T y2, T d2) {
            T h = x2 - x1;
            T f_x1x2 = (y2 - y1) / h;

            T c0 = y1;
            T c1 = d1;
            T c2 = (f_x1x2 - d1) / h;
            T f_x1x2x2 = (d2 - f_x1x2) / h;
            T c3 = (f_x1x2x2 - c2) / h;

            std::array<T, 4> a;

            a[3] = c3;
            a[2] = c2 - c3 * (2 * x1 + x2);
            a[1] = c1 - 2 * x1 * c2 + c3 * (x1 * x1 + 2 * x1 * x2);
            a[0] = c0 - x1 * c1 + x1 * x1 * c2 - x1 * x1 * x2 * c3;

            return a;
        }

        template <typename T>
        constexpr T fb(T xl, T yl, T x0, T y0, T xr, T yr) {
            const T h_l = x0 - xl;
            const T h_r = xr - x0;

            const T delta_l = (y0 - yl) / h_l;
            const T delta_r = (yr - y0) / h_r;

            if (delta_l * delta_r <= 0) {
                return 0;
            }

            const T w1 = 2 * h_r + h_l;
            const T w2 = h_r + 2 * h_l;

            return (w1 + w2) / (w1 / delta_l + w2 / delta_r);
        }

        template <typename T>
        constexpr T fbLeftBoundary(T x0, T y0, T x1, T y1, T x2, T y2) {
            const T h1 = x1 - x0;
            const T h2 = x2 - x1;
            const T delta1 = (y1 - y0) / h1;
            const T delta2 = (y2 - y1) / h2;

            T d = ((2 * h1 + h2) * delta1 - h1 * delta2) / (h1 + h2);

            if (d * delta1 <= 0) {
                d = 0;
            } else if (delta1 * delta2 <= 0 && std::abs(d) > std::abs(3 * delta1)) {
                d = 3 * delta1;
            }
            return d;
        }

        template <typename T>
        constexpr T fbRightBoundary(T xn_2, T yn_2, T xn_1, T yn_1, T xn, T yn) {
            const T h1 = xn_1 - xn_2;
            const T h2 = xn - xn_1;
            const T delta1 = (yn_1 - yn_2) / h1;
            const T delta2 = (yn - yn_1) / h2;

            T d = ((2 * h2 + h1) * delta2 - h2 * delta1) / (h1 + h2);

            if (d * delta2 <= 0) {
                d = 0;
            } else if (delta1 * delta2 <= 0 && std::abs(d) > std::abs(3 * delta2)) {
                d = 3 * delta2;
            }
            return d;
        }

    }

    template <typename T>
    constexpr Interpolator<T> Interpolator<T>::create(T x1, T y1, T x2, T y2, T xref1, T yref1, T xref2, T yref2) {
        T d1 = impl::fb(xref1, yref1, x1, y1, x2, y2);
        T d2 = impl::fb(x1, y1, x2, y2, xref2, yref2);
        return Interpolator{impl::calculateHermiteCoefficients(x1, y1, d1, x2, y2, d2)};
    }

    template <typename T>
    constexpr Interpolator<T> Interpolator<T>::createWithRef1Only(T x1, T y1, T x2, T y2, T xref1, T yref1) {
        T d1 = impl::fb(xref1, yref1, x1, y1, x2, y2);
        T d2 = impl::fbRightBoundary(xref1, yref1, x1, y1, x2, y2);
        return Interpolator{impl::calculateHermiteCoefficients(x1, y1, d1, x2, y2, d2)};
    }

    template <typename T>
    constexpr Interpolator<T> Interpolator<T>::createWithRef2Only(T x1, T y1, T x2, T y2, T xref2, T yref2) {
        T d1 = impl::fbLeftBoundary(x1, y1, x2, y2, xref2, yref2);
        T d2 = impl::fb(x1, y1, x2, y2, xref2, yref2);
        return Interpolator{impl::calculateHermiteCoefficients(x1, y1, d1, x2, y2, d2)};
    }

    template <typename T>
    constexpr Interpolator<T> Interpolator<T>::createLinear(T x1, T y1, T x2, T y2) {
        return Interpolator{{(x1 * y2 - x2 * y1) / (x1 - x2), (y1 - y2) / (x1 - x2), T(0), T(0)}};
    }

    template <typename T>
    constexpr T Interpolator<T>::evaluate(T x) const {
        return ((m_coeffs[3] * x + m_coeffs[2]) * x + m_coeffs[1]) * x + m_coeffs[0];
    }

}

#endif //OPENDSPX_INTERPOLATOR_INTERPOLATOR_H
