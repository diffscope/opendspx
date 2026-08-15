#include <cmath>
#include <vector>

#include <opendspx/interpolator/vibratocurve.h>

#include <boost/test/unit_test.hpp>

using namespace opendspx;

namespace {

    constexpr double tolerance = 1e-9;

    ControlPoint point(double x, double y) {
        return ControlPoint{x, y};
    }

    // A plain vibrato over the whole note: one cycle across it, amplitude ten, nothing shaped.
    Vibrato plainVibrato() {
        Vibrato v;
        v.start = 0.0;
        v.end = 1.0;
        v.amp = 10;
        v.freq = 1.0;
        v.phase = 0.0;
        v.offset = 0;
        return v;
    }

}

BOOST_AUTO_TEST_SUITE(test_vibratocurve)

// Outside the window the curve is flat zero, not the offset. A note is unaffected where the
// vibrato does not reach, and an offset applied outside would bend the pitch there.
BOOST_AUTO_TEST_CASE(test_outside_the_window_is_zero) {
    auto v = plainVibrato();
    v.start = 0.25;
    v.end = 0.75;
    v.offset = 7;
    const VibratoCurve curve(v, 1.0);

    BOOST_CHECK_EQUAL(curve.evaluate(0.0), 0.0);
    BOOST_CHECK_EQUAL(curve.evaluate(0.249), 0.0);
    BOOST_CHECK_EQUAL(curve.evaluate(0.751), 0.0);
    BOOST_CHECK_EQUAL(curve.evaluate(1.0), 0.0);

    // The two ends belong to the window.
    BOOST_CHECK_NE(curve.evaluate(0.25), 0.0);
    BOOST_CHECK_NE(curve.evaluate(0.75), 0.0);
}

BOOST_AUTO_TEST_CASE(test_one_cycle_over_the_note) {
    const VibratoCurve curve(plainVibrato(), 1.0);

    BOOST_CHECK_SMALL(curve.evaluate(0.0), tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(0.25) - 10.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(0.5), tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(0.75) + 10.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(1.0), tolerance);
}

// freq is cycles per unit length, so the note's length is what turns it into cycles across the
// window. A longer note at the same freq oscillates more times, which is the point of storing
// the two separately.
BOOST_AUTO_TEST_CASE(test_length_scales_the_cycle_count) {
    const VibratoCurve twice(plainVibrato(), 2.0);
    BOOST_CHECK_SMALL(twice.evaluate(0.125) - 10.0, tolerance);
    BOOST_CHECK_SMALL(twice.evaluate(0.25), tolerance);

    auto faster = plainVibrato();
    faster.freq = 2.0;
    const VibratoCurve doubled(faster, 1.0);
    BOOST_CHECK_SMALL(doubled.evaluate(0.125) - 10.0, tolerance);
}

// Phase is in cycles, not radians. A quarter turn starts the curve at its peak.
BOOST_AUTO_TEST_CASE(test_phase_is_in_cycles) {
    auto v = plainVibrato();
    v.phase = 0.25;
    const VibratoCurve curve(v, 1.0);

    BOOST_CHECK_SMALL(curve.evaluate(0.0) - 10.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(0.25), tolerance);

    v.phase = 0.5;
    const VibratoCurve inverted(v, 1.0);
    BOOST_CHECK_SMALL(inverted.evaluate(0.25) + 10.0, tolerance);
}

BOOST_AUTO_TEST_CASE(test_offset_shifts_the_whole_curve) {
    auto v = plainVibrato();
    v.offset = 3;
    const VibratoCurve curve(v, 1.0);

    BOOST_CHECK_SMALL(curve.evaluate(0.0) - 3.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(0.25) - 13.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(0.75) + 7.0, tolerance);
}

// An empty shaping curve means no shaping -- amplitude one everywhere -- rather than zero, which
// would silence every vibrato that did not draw one.
BOOST_AUTO_TEST_CASE(test_no_shaping_curve_means_full_amplitude) {
    auto v = plainVibrato();
    BOOST_CHECK(v.points.amp.empty());
    BOOST_CHECK(v.points.freq.empty());

    const VibratoCurve curve(v, 1.0);
    BOOST_CHECK_SMALL(curve.evaluate(0.25) - 10.0, tolerance);
}

BOOST_AUTO_TEST_CASE(test_amplitude_curve_scales_the_peak) {
    auto v = plainVibrato();
    v.points.amp = {point(0.0, 0.0), point(1.0, 1.0)};
    const VibratoCurve curve(v, 1.0);

    // A ramp from silent to full, sampled at the peak of the first cycle.
    BOOST_CHECK_SMALL(curve.evaluate(0.25) - 2.5, tolerance);

    v.points.amp = {point(0.0, 0.5), point(1.0, 0.5)};
    const VibratoCurve half(v, 1.0);
    BOOST_CHECK_SMALL(half.evaluate(0.25) - 5.0, tolerance);
}

// Outside the drawn range the shaping curve holds its end value rather than falling to zero.
BOOST_AUTO_TEST_CASE(test_amplitude_curve_holds_beyond_its_ends) {
    auto v = plainVibrato();
    v.points.amp = {point(0.4, 0.5), point(0.6, 0.5)};
    const VibratoCurve curve(v, 1.0);

    BOOST_CHECK_SMALL(curve.evaluate(0.25) - 5.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(0.75) + 5.0, tolerance);
}

// The frequency curve is integrated, not sampled: it says how fast phase advances, so a constant
// two doubles the cycles rather than doubling the value.
BOOST_AUTO_TEST_CASE(test_frequency_curve_is_integrated) {
    auto v = plainVibrato();
    v.points.freq = {point(0.0, 2.0)};
    const VibratoCurve doubled(v, 1.0);

    // Phase now advances at twice the rate, so the first peak arrives at an eighth.
    BOOST_CHECK_SMALL(doubled.evaluate(0.125) - 10.0, tolerance);
    BOOST_CHECK_SMALL(doubled.evaluate(0.25), tolerance);

    // A ramp from zero to two integrates to t^2 over [0, 1], so a quarter cycle lands at t = 1/2.
    v.points.freq = {point(0.0, 0.0), point(1.0, 2.0)};
    const VibratoCurve ramp(v, 1.0);
    BOOST_CHECK_SMALL(ramp.evaluate(0.5) - 10.0, tolerance);
    BOOST_CHECK_SMALL(ramp.evaluate(0.0), tolerance);
}

BOOST_AUTO_TEST_CASE(test_shaping_points_are_sorted) {
    auto v = plainVibrato();
    v.points.amp = {point(1.0, 1.0), point(0.0, 0.0)};
    const VibratoCurve curve(v, 1.0);

    BOOST_CHECK_SMALL(curve.evaluate(0.25) - 2.5, tolerance);
}

// Two points on the same x are one point, and the later one is the one that counts. Without the
// collapse the segment between them has zero width and the interpolation divides by zero.
BOOST_AUTO_TEST_CASE(test_duplicate_x_keeps_the_last) {
    auto v = plainVibrato();
    v.points.amp = {point(0.0, 1.0), point(1.0, 0.0), point(1.0, 0.5)};
    const VibratoCurve curve(v, 1.0);

    // The ramp now runs 1.0 -> 0.5, so a quarter of the way in the scale is 0.875.
    BOOST_CHECK_SMALL(curve.evaluate(0.25) - 8.75, tolerance);
    BOOST_CHECK(std::isfinite(curve.evaluate(1.0)));
}

BOOST_AUTO_TEST_SUITE_END()
