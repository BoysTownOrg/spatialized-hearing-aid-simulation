#include "assert-utility.h"
#include <fir-filtering/FirFilter.h>
#include <gtest/gtest.h>

class FirFilterFacade {
	FirFilter filter;
public:
	explicit FirFilterFacade(std::vector<float> b) :
		filter{ std::move(b) } {}
	std::vector<float> process(std::vector<float> x) {
		filter.process(&x[0], x.size());
		return x;
	}
};

class FirFilterTestCase : public ::testing::TestCase {};

TEST(FirFilterTestCase, testEmptyCoefficientsThrowsException) {
	EXPECT_THROW(FirFilter{ {} }, FirFilter::InvalidCoefficients);
}

static void assertFilterOutput(
	FirFilter filter,
	std::vector<float> input,
	std::vector<float> output);

TEST(FirFilterTestCase, testZeroIR) {
	assertFilterOutput(FirFilter{ { 0 } }, { 1, 2, 3 }, { 0, 0, 0 });
	assertFilterOutput(FirFilter{ { 0, 0 } }, { 1, 2, 3 }, { 0, 0, 0 });
	assertFilterOutput(FirFilter{ { 0, 0, 0 } }, { 1, 2, 3 }, { 0, 0, 0 });
}

void assertFilterOutput(
	FirFilter filter,
	std::vector<float> input,
	std::vector<float> output)
{
	filter.process(&input[0], input.size());
	assertEqual(output, input);
}

TEST(FirFilterTestCase, testIdentityFilter) {
	assertFilterOutput(FirFilter{ { 1 } }, { 1, 2, 3 }, { 1, 2, 3 });
	assertFilterOutput(FirFilter{ { 1, 0 } }, { 1, 2, 3 }, { 1, 2, 3 });
	assertFilterOutput(FirFilter{ { 1, 0, 0 } }, { 1, 2, 3 }, { 1, 2, 3 });
}

TEST(FirFilterTestCase, testDoublingFilter) {
	assertFilterOutput(FirFilter{ { 2 } }, { 1, 2, 3 }, { 2, 4, 6 });
	assertFilterOutput(FirFilter{ { 2, 0 } }, { 1, 2, 3 }, { 2, 4, 6 });
	assertFilterOutput(FirFilter{ { 2, 0, 0 } }, { 1, 2, 3 }, { 2, 4, 6 });
}

TEST(FirFilterTestCase, testSimpleMovingSum) {
	assertFilterOutput(FirFilter{ { 1, 1 } }, { 1, 2, 3 }, { 1, 3, 5 });
	assertFilterOutput(FirFilter{ { 1, 1, 0 } }, { 1, 2, 3 }, { 1, 3, 5 });
	assertFilterOutput(FirFilter{ { 1, 1, 0, 0 } }, { 1, 2, 3 }, { 1, 3, 5 });
}

TEST(FirFilterTestCase, testZeroIRWithSuccessiveCalls) {
	FirFilterFacade filter{ { 0 } };
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, testIdentityFilterWithSuccessiveCalls) {
	FirFilterFacade filter{ { 1 } };
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, testSimpleMovingSumWithSuccessiveCalls) {
	FirFilterFacade filter{ { 1, 1 } };
	assertEqual({ 1, 3, 5 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 4, 3, 5 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 4, 3, 5 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, testSimpleMovingSumWithIncreasingInputSize) {
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1, 3 }, filter.process({ 1, 2 }));
	assertEqual({ 5, 7, 9 }, filter.process({ 3, 4, 5 }));
	assertEqual({ 11, 13, 15, 17 }, filter.process({ 6, 7, 8, 9 }));
}

TEST(FirFilterTestCase, testSimpleMovingSumWithDecreasingInputSize) {
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1, 3, 5, 7 }, filter.process({ 1, 2, 3, 4 }));
	assertEqual({ 9, 11, 13 }, filter.process({ 5, 6, 7 }));
	assertEqual({ 15, 17 }, filter.process({ 8, 9 }));
}

TEST(FirFilterTestCase, testSimpleMovingSumWithChangingInputSize) {
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1, 3, 5 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 7, 9, 11, 13 }, filter.process({ 4, 5, 6, 7 }));
	assertEqual({ 15, 17 }, filter.process({ 8, 9 }));
}

TEST(FirFilterTestCase, testSimpleMovingSumWithInputSizeGoingAboveAndBelowCoefficientLength) {
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1 }, filter.process({ 1 }));
	assertEqual({ 3, 5 }, filter.process({ 2, 3 }));
	assertEqual({ 7, 9, 11 }, filter.process({ 4, 5, 6 }));
	assertEqual({ 13, 15 }, filter.process({ 7, 8 }));
	assertEqual({ 17 }, filter.process({ 9 }));
}

TEST(FirFilterTestCase, test2ndOrderMovingSumWithSuccessiveCalls) {
	FirFilterFacade filter({ 1, 1, 1 });
	assertEqual({ 1 }, filter.process({ 1 }));
	assertEqual({ 3, 6 }, filter.process({ 2, 3 }));
	assertEqual({ 9, 12, 15 }, filter.process({ 4, 5, 6 }));
	assertEqual({ 18, 21 }, filter.process({ 7, 8 }));
	assertEqual({ 24 }, filter.process({ 9 }));
}

TEST(FirFilterTestCase, test2ndOrderMovingSumOneCall) {
	FirFilterFacade filter({ 1, 1, 1 });
	assertEqual(
		{ 1, 3, 6, 9, 12, 15, 18, 21, 24 },
		filter.process({ 1, 2, 3, 4, 5, 6, 7, 8, 9 }));
}

TEST(FirFilterTestCase, testDelayedIdentity) {
	FirFilterFacade filter({ 0, 0, 1 });
	assertEqual(
		{ 0, 0, 1, 2, 3, },
		filter.process({ 1, 2, 3, 4, 5 }));
	assertEqual(
		{ 4, 5 },
		filter.process({ 0, 0 }));
}

TEST(FirFilterTestCase, testPositiveCoefficients) {
	FirFilterFacade filter({ 5, 3, 4, 2, 1 });
	assertEqual({ 5, 13, 25, 39, 54 }, filter.process({ 1, 2, 3, 4, 5 }));
	assertEqual({ 69, 84, 99, 114, 129 }, filter.process({ 6, 7, 8, 9, 10 }));
}

TEST(FirFilterTestCase, testNegativeCoefficients) {
	FirFilterFacade filter({ -4, -2, -3, -5 });
	assertEqual({ -4, -10, -19 }, filter.process({ 1, 2, 3 }));
	assertEqual({ -33, -47, -61, -75, -89, -103, -117 }, filter.process({ 4, 5, 6, 7, 8, 9, 10 }));
}

TEST(FirFilterTestCase, testPositiveAndNegativeCoefficients) {
	FirFilterFacade filter({ -1, 1, 0, 1, -1 });
	assertEqual({ -1, -1, -1, 0, 0 }, filter.process({ 1, 2, 3, 4, 5 }));
	assertEqual({}, filter.process({}));
	assertEqual({ 0, 0 }, filter.process({ 6, 7 }));
	assertEqual({ 0, 0, 0 }, filter.process({ 8, 9, 10 }));
}