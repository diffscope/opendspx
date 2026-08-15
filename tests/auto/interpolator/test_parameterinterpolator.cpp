#include <optional>
#include <vector>

#include <opendspx/interpolator/parameterinterpolator.h>

#include <boost/test/unit_test.hpp>

using namespace opendspx;

namespace {

    constexpr double tolerance = 1e-9;

    AnchorNode node(int x, int y, AnchorNode::Interpolation interp) {
        AnchorNode n;
        n.x = x;
        n.y = y;
        n.interp = interp;
        return n;
    }

    constexpr auto None = AnchorNode::Interpolation::None;
    constexpr auto Linear = AnchorNode::Interpolation::Linear;
    constexpr auto Hermite = AnchorNode::Interpolation::Hermite;

}

BOOST_AUTO_TEST_SUITE(test_parameterinterpolator)

// Outside the anchors there is nothing to interpolate between, which is not the same as zero --
// the caller has to be able to tell "no value here" from "the value is zero".
BOOST_AUTO_TEST_CASE(test_outside_the_anchors_is_no_value) {
    const ParameterInterpolator curve({node(10, 100, Linear), node(20, 200, Linear)});

    BOOST_CHECK(!curve.evaluate(9.999).has_value());
    BOOST_CHECK(!curve.evaluate(20.001).has_value());
    BOOST_CHECK(curve.evaluate(10.0).has_value());
    BOOST_CHECK(curve.evaluate(20.0).has_value());
}

BOOST_AUTO_TEST_CASE(test_empty_and_single_anchor) {
    BOOST_CHECK(!ParameterInterpolator({}).evaluate(0.0).has_value());

    const ParameterInterpolator one({node(5, 50, Linear)});
    BOOST_CHECK_EQUAL(one.evaluate(5.0).value(), 50.0);
    BOOST_CHECK(!one.evaluate(4.0).has_value());
    BOOST_CHECK(!one.evaluate(6.0).has_value());
}

// An anchor's own x answers with its own y whatever the interpolation says, including None.
BOOST_AUTO_TEST_CASE(test_landing_on_an_anchor) {
    const ParameterInterpolator curve({node(0, 1, None), node(10, 2, None), node(20, 3, None)});

    BOOST_CHECK_EQUAL(curve.evaluate(0.0).value(), 1.0);
    BOOST_CHECK_EQUAL(curve.evaluate(10.0).value(), 2.0);
    BOOST_CHECK_EQUAL(curve.evaluate(20.0).value(), 3.0);
}

// None leaves the span between two anchors undefined rather than holding the left value. A
// caller that drew it as a step would be drawing something the model does not say.
BOOST_AUTO_TEST_CASE(test_none_leaves_the_span_empty) {
    const ParameterInterpolator curve({node(0, 0, None), node(10, 100, Linear)});

    BOOST_CHECK(!curve.evaluate(5.0).has_value());
    BOOST_CHECK(curve.evaluate(0.0).has_value());
    BOOST_CHECK(curve.evaluate(10.0).has_value());
}

// The interpolation of the *left* anchor governs the span, not the right one.
BOOST_AUTO_TEST_CASE(test_the_left_anchor_governs_the_span) {
    const ParameterInterpolator curve(
        {node(0, 0, Linear), node(10, 100, None), node(20, 200, Linear)});

    BOOST_CHECK_SMALL(curve.evaluate(5.0).value() - 50.0, tolerance);
    BOOST_CHECK(!curve.evaluate(15.0).has_value());
}

BOOST_AUTO_TEST_CASE(test_linear_span) {
    const ParameterInterpolator curve({node(0, 0, Linear), node(4, 100, Linear)});

    BOOST_CHECK_SMALL(curve.evaluate(1.0).value() - 25.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(2.0).value() - 50.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(3.0).value() - 75.0, tolerance);
}

// Anchors are sorted on construction, so the caller is not required to hand them over in order.
BOOST_AUTO_TEST_CASE(test_unsorted_input_is_sorted) {
    const ParameterInterpolator curve(
        {node(20, 200, Linear), node(0, 0, Linear), node(10, 100, Linear)});

    BOOST_CHECK_SMALL(curve.evaluate(5.0).value() - 50.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(15.0).value() - 150.0, tolerance);
    BOOST_CHECK(!curve.evaluate(-1.0).has_value());
    BOOST_CHECK(!curve.evaluate(21.0).has_value());
}

// The sort is stable, so two anchors on the same x keep the order they were given and the first
// of them is the one lower_bound lands on.
BOOST_AUTO_TEST_CASE(test_duplicate_x_keeps_the_first) {
    const ParameterInterpolator curve(
        {node(0, 0, Linear), node(10, 111, Linear), node(10, 222, Linear), node(20, 300, Linear)});

    BOOST_CHECK_EQUAL(curve.evaluate(10.0).value(), 111.0);
}

// A Hermite span with no neighbour on either side has no reference to estimate a slope from, so
// it falls back to the straight line between its two anchors.
BOOST_AUTO_TEST_CASE(test_hermite_without_references_is_linear) {
    const ParameterInterpolator curve({node(0, 0, Hermite), node(10, 100, Hermite)});

    BOOST_CHECK_SMALL(curve.evaluate(2.5).value() - 25.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(7.5).value() - 75.0, tolerance);
}

// With neighbours on both sides the span is a cubic, but it still meets its anchors exactly and
// stays inside them while the run is monotone.
BOOST_AUTO_TEST_CASE(test_hermite_with_references) {
    const ParameterInterpolator curve({node(0, 0, Hermite), node(10, 100, Hermite),
                                       node(20, 300, Hermite), node(30, 600, Hermite)});

    BOOST_CHECK_SMALL(curve.evaluate(10.0).value() - 100.0, tolerance);
    BOOST_CHECK_SMALL(curve.evaluate(20.0).value() - 300.0, tolerance);

    for (double x = 10.0; x <= 20.0; x += 0.5) {
        const auto y = curve.evaluate(x);
        BOOST_REQUIRE(y.has_value());
        BOOST_CHECK_GE(*y, 100.0 - tolerance);
        BOOST_CHECK_LE(*y, 300.0 + tolerance);
    }
}

// A run that turns around must not bulge past the anchor it turns at.
BOOST_AUTO_TEST_CASE(test_hermite_does_not_overshoot_a_turning_point) {
    const ParameterInterpolator curve(
        {node(0, 0, Hermite), node(10, 100, Hermite), node(20, 0, Hermite), node(30, 100, Hermite)});

    for (double x = 10.0; x <= 20.0; x += 0.25) {
        const auto y = curve.evaluate(x);
        BOOST_REQUIRE(y.has_value());
        BOOST_CHECK_LE(*y, 100.0 + tolerance);
        BOOST_CHECK_GE(*y, 0.0 - tolerance);
    }
}

BOOST_AUTO_TEST_SUITE_END()
