#include <stdexcept>
#include <vector>

#include <opendspx/interpolator/mixinterpolator.h>

#include <boost/test/unit_test.hpp>

using namespace opendspx;

namespace {

    constexpr double tolerance = 1e-9;

    DynamicMixingAnchor anchor(int pos, SourceMixingRatio ratio) {
        DynamicMixingAnchor a;
        a.pos = pos;
        a.ratio = std::move(ratio);
        return a;
    }

    void checkRatio(const SourceMixingRatio &actual, const std::vector<double> &expected) {
        BOOST_REQUIRE_EQUAL(actual.size(), expected.size());
        for (std::size_t i = 0; i < expected.size(); ++i) {
            BOOST_CHECK_SMALL(actual[i] - expected[i], tolerance);
        }
    }

}

BOOST_AUTO_TEST_SUITE(test_mixinterpolator)

// A ratio holds one number fewer than there are sources -- the last source gets what is left --
// so a mismatch is caught when the interpolator is built rather than read out later as a value
// nobody can interpret.
BOOST_AUTO_TEST_CASE(test_rejects_a_ratio_of_the_wrong_length) {
    BOOST_CHECK_THROW(MixInterpolator({anchor(0, SourceMixingRatio{0.5})}, 3),
                      std::invalid_argument);
    BOOST_CHECK_THROW(MixInterpolator({anchor(0, SourceMixingRatio{0.2, 0.3, 0.4})}, 3),
                      std::invalid_argument);
    BOOST_CHECK_NO_THROW(MixInterpolator({anchor(0, SourceMixingRatio{0.2, 0.3})}, 3));

    // The check is over every anchor, not only the first.
    BOOST_CHECK_THROW(
        MixInterpolator({anchor(0, SourceMixingRatio{0.2, 0.3}), anchor(1, SourceMixingRatio{0.5})},
                        3),
        std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_rejects_no_sources) {
    BOOST_CHECK_THROW(MixInterpolator({}, 0), std::invalid_argument);
}

// With nothing drawn, every source is worth the same. One source means an empty ratio -- there
// is nothing to divide -- which is the case an off-by-one in the size would get wrong.
BOOST_AUTO_TEST_CASE(test_no_anchors_is_an_even_mix) {
    checkRatio(MixInterpolator({}, 4).evaluate(0.0), {0.25, 0.25, 0.25});
    checkRatio(MixInterpolator({}, 2).evaluate(1234.0), {0.5});
    checkRatio(MixInterpolator({}, 1).evaluate(0.0), {});
}

BOOST_AUTO_TEST_CASE(test_landing_on_an_anchor) {
    const MixInterpolator curve(
        {anchor(0, SourceMixingRatio{0.1}), anchor(100, SourceMixingRatio{0.9})}, 2);

    checkRatio(curve.evaluate(0.0), {0.1});
    checkRatio(curve.evaluate(100.0), {0.9});
}

// Outside the anchors the nearest one holds, rather than the mix running off to nothing.
BOOST_AUTO_TEST_CASE(test_clamps_outside_the_anchors) {
    const MixInterpolator curve(
        {anchor(10, SourceMixingRatio{0.2}), anchor(20, SourceMixingRatio{0.8})}, 2);

    checkRatio(curve.evaluate(-1000.0), {0.2});
    checkRatio(curve.evaluate(9.999), {0.2});
    checkRatio(curve.evaluate(20.001), {0.8});
    checkRatio(curve.evaluate(1e9), {0.8});
}

BOOST_AUTO_TEST_CASE(test_blends_between_anchors) {
    const MixInterpolator curve(
        {anchor(0, SourceMixingRatio{0.0, 1.0}), anchor(100, SourceMixingRatio{1.0, 0.0})}, 3);

    checkRatio(curve.evaluate(25.0), {0.25, 0.75});
    checkRatio(curve.evaluate(50.0), {0.5, 0.5});
    checkRatio(curve.evaluate(75.0), {0.75, 0.25});
}

// Each component is blended on its own, so a mix where the components move by different amounts
// is not the same as scaling one of them.
BOOST_AUTO_TEST_CASE(test_blends_each_component_separately) {
    const MixInterpolator curve(
        {anchor(0, SourceMixingRatio{0.1, 0.6}), anchor(10, SourceMixingRatio{0.5, 0.2})}, 3);

    checkRatio(curve.evaluate(5.0), {0.3, 0.4});
    checkRatio(curve.evaluate(2.5), {0.2, 0.5});
}

BOOST_AUTO_TEST_CASE(test_unsorted_anchors_are_sorted) {
    const MixInterpolator curve({anchor(100, SourceMixingRatio{1.0}),
                                 anchor(0, SourceMixingRatio{0.0}),
                                 anchor(50, SourceMixingRatio{0.25})},
                                2);

    checkRatio(curve.evaluate(0.0), {0.0});
    checkRatio(curve.evaluate(50.0), {0.25});
    checkRatio(curve.evaluate(100.0), {1.0});
    // Between the first two, so the 0.0 -> 0.25 leg rather than the whole span.
    checkRatio(curve.evaluate(25.0), {0.125});
}

// A blend of two valid partitions is still a valid partition, which is the property the model
// relies on when it hands a mid-anchor ratio to a renderer.
BOOST_AUTO_TEST_CASE(test_a_blend_of_valid_ratios_stays_valid) {
    const MixInterpolator curve(
        {anchor(0, SourceMixingRatio{0.5, 0.25}), anchor(10, SourceMixingRatio{0.1, 0.1})}, 3);

    for (double pos = 0.0; pos <= 10.0; pos += 0.5) {
        BOOST_CHECK(curve.evaluate(pos).valid());
    }
}

BOOST_AUTO_TEST_SUITE_END()
