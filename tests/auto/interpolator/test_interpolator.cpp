#include <cmath>

#include <opendspx/interpolator/interpolator.h>

#include <boost/test/unit_test.hpp>

using namespace opendspx;

namespace {

    constexpr double tolerance = 1e-9;

    // The four factories differ only in where the end derivatives come from, so every case that
    // checks a curve checks it the same way: at the two nodes it was built from.
    void checkPassesThroughNodes(const Interpolator<double> &curve, double x1, double y1, double x2,
                                 double y2) {
        BOOST_CHECK_SMALL(curve.evaluate(x1) - y1, tolerance);
        BOOST_CHECK_SMALL(curve.evaluate(x2) - y2, tolerance);
    }

}

BOOST_AUTO_TEST_SUITE(test_interpolator)

// The whole class is constexpr, and a curve evaluated at build time is the reason it is. Losing
// that would still compile everywhere it is called from, so it is asserted rather than checked.
BOOST_AUTO_TEST_CASE(test_usable_at_compile_time) {
    constexpr auto line = Interpolator<double>::createLinear(0.0, 0.0, 2.0, 4.0);
    static_assert(line.evaluate(1.0) == 2.0);
    static_assert(line.evaluate(0.0) == 0.0);

    constexpr auto hermite =
        Interpolator<double>::create(0.0, 0.0, 1.0, 1.0, -1.0, -1.0, 2.0, 2.0);
    static_assert(hermite.evaluate(0.5) > 0.0);

    BOOST_CHECK(true); // the assertions above are the case
}

BOOST_AUTO_TEST_CASE(test_linear) {
    const auto curve = Interpolator<double>::createLinear(1.0, 10.0, 3.0, 20.0);
    checkPassesThroughNodes(curve, 1.0, 10.0, 3.0, 20.0);
    BOOST_CHECK_SMALL(curve.evaluate(2.0) - 15.0, tolerance);

    // A line is a line outside its own two nodes as well; nothing clamps.
    BOOST_CHECK_SMALL(curve.evaluate(5.0) - 30.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(-1.0) - 0.0, tolerance);
}

// A descending segment, which is where a sign error in createLinear's constant term would show.
BOOST_AUTO_TEST_CASE(test_linear_descending) {
    const auto curve = Interpolator<double>::createLinear(-2.0, 8.0, 4.0, -4.0);
    checkPassesThroughNodes(curve, -2.0, 8.0, 4.0, -4.0);
    BOOST_CHECK_SMALL(curve.evaluate(1.0) - 2.0, tolerance);
}

BOOST_AUTO_TEST_CASE(test_hermite_passes_through_its_nodes) {
    // Reference points on both sides, on one side, and on the other.
    checkPassesThroughNodes(
        Interpolator<double>::create(1.0, 2.0, 4.0, 9.0, -3.0, 7.0, 6.0, 1.0), 1.0, 2.0, 4.0, 9.0);
    checkPassesThroughNodes(
        Interpolator<double>::createWithRef1Only(1.0, 2.0, 4.0, 9.0, -3.0, 7.0), 1.0, 2.0, 4.0,
        9.0);
    checkPassesThroughNodes(
        Interpolator<double>::createWithRef2Only(1.0, 2.0, 4.0, 9.0, 6.0, 1.0), 1.0, 2.0, 4.0, 9.0);
}

// When every point given is on one line, the estimated derivatives come out as that line's slope
// and the cubic collapses to it. This is the property that keeps a straight run of anchors
// straight even though the user asked for Hermite.
BOOST_AUTO_TEST_CASE(test_hermite_reproduces_a_straight_line) {
    const auto line = [](double x) {
        return 3.0 * x - 1.0;
    };

    const auto both = Interpolator<double>::create(1.0, line(1.0), 2.0, line(2.0), 0.0, line(0.0),
                                                  3.0, line(3.0));
    const auto left =
        Interpolator<double>::createWithRef1Only(1.0, line(1.0), 2.0, line(2.0), 0.0, line(0.0));
    const auto right =
        Interpolator<double>::createWithRef2Only(1.0, line(1.0), 2.0, line(2.0), 3.0, line(3.0));

    for (double x = 1.0; x <= 2.0; x += 0.125) {
        BOOST_CHECK_SMALL(both.evaluate(x) - line(x), tolerance);
        BOOST_CHECK_SMALL(left.evaluate(x) - line(x), tolerance);
        BOOST_CHECK_SMALL(right.evaluate(x) - line(x), tolerance);
    }
}

// The derivative estimator answers zero when the two neighbouring slopes disagree in sign, which
// is what stops the curve from bulging past a node that is a local extremum. Without it a run of
// anchors that turns around produces values outside the range the user drew.
BOOST_AUTO_TEST_CASE(test_hermite_does_not_overshoot_a_turning_point) {
    // Rises to (1, 1) and falls away again, so x = 1 is a maximum.
    const auto curve = Interpolator<double>::create(1.0, 1.0, 2.0, 0.0, 0.0, 0.0, 3.0, 1.0);

    for (double x = 1.0; x <= 2.0; x += 1.0 / 64) {
        const double y = curve.evaluate(x);
        BOOST_CHECK_LE(y, 1.0 + tolerance);
        BOOST_CHECK_GE(y, 0.0 - tolerance);
    }

    // The maximum is at the node itself: the segment leaves it going flat, not upward.
    BOOST_CHECK_LE(curve.evaluate(1.0 + 1e-6), 1.0 + tolerance);
}

// A flat neighbour is the boundary of that rule -- the product of the slopes is zero, not
// negative -- and it has to take the same branch, or a plateau grows a bump.
BOOST_AUTO_TEST_CASE(test_hermite_keeps_a_plateau_flat) {
    const auto curve = Interpolator<double>::create(1.0, 1.0, 2.0, 1.0, 0.0, 0.0, 3.0, 1.0);
    for (double x = 1.0; x <= 2.0; x += 1.0 / 64) {
        BOOST_CHECK_LE(curve.evaluate(x), 1.0 + tolerance);
    }
}

// The estimator weights the two slopes by the widths of the intervals they came from, so an
// uneven spacing is not the same as an even one. Checked as a property -- the curve stays inside
// the values it was given -- rather than against coefficients nobody can read.
BOOST_AUTO_TEST_CASE(test_hermite_stays_within_range_on_uneven_spacing) {
    const auto curve = Interpolator<double>::create(0.0, 0.0, 10.0, 1.0, -0.5, 0.0, 10.25, 1.0);
    for (double x = 0.0; x <= 10.0; x += 0.25) {
        const double y = curve.evaluate(x);
        BOOST_CHECK_GE(y, -tolerance);
        BOOST_CHECK_LE(y, 1.0 + tolerance);
    }
}

BOOST_AUTO_TEST_CASE(test_float_instantiation) {
    const auto curve = Interpolator<float>::createLinear(0.0f, 0.0f, 4.0f, 2.0f);
    BOOST_CHECK_CLOSE(curve.evaluate(2.0f), 1.0f, 1e-3);
}

BOOST_AUTO_TEST_SUITE_END()
