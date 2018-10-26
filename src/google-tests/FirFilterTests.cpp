#include "assert-utility.h"
#include <fir-filtering/FirFilter.h>
#include <gtest/gtest.h>

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
	FirFilter filter({ 0 });
	std::vector<float> x = { 1, 2, 3 };
	filter.process(&x[0], x.size());
	assertEqual({ 0, 0, 0 }, x);
	filter.process(&x[0], x.size());
	assertEqual({ 0, 0, 0 }, x);
	filter.process(&x[0], x.size());
	assertEqual({ 0, 0, 0 }, x);
}

/*

TEST(FirFilterTestCase, testIdentityFilterWithSuccessiveCalls) {
	FirFilter filter({ 1 });
	assertAreEqual({ 1, 2, 3 }, filter({ 1, 2, 3 }));
	assertAreEqual({ 4, 5, 6 }, filter({ 4, 5, 6 }));
	assertAreEqual({ 1, 2, 3 }, filter({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, testSimpleMovingSumWithSuccessiveCalls) {
	FirFilter filter({ 1, 1 });
	assertAreEqual({ 1, 3, 5 }, filter({ 1, 2, 3 }));
	assertAreEqual({ 4, 3, 5 }, filter({ 1, 2, 3 }));
	assertAreEqual({ 4, 3, 5 }, filter({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, testSimpleMovingSumWithIncreasingInputSize) {
	FirFilter filter({ 1, 1 });
	assertAreEqual({ 1, 3 }, filter({ 1, 2 }));
	assertAreEqual({ 5, 7, 9 }, filter({ 3, 4, 5 }));
	assertAreEqual({ 11, 13, 15, 17 }, filter({ 6, 7, 8, 9 }));
}

TEST(FirFilterTestCase, testSimpleMovingSumWithDecreasingInputSize) {
	FirFilter filter({ 1, 1 });
	assertAreEqual({ 1, 3, 5, 7 }, filter({ 1, 2, 3, 4 }));
	assertAreEqual({ 9, 11, 13 }, filter({ 5, 6, 7 }));
	assertAreEqual({ 15, 17 }, filter({ 8, 9 }));
}

TEST(FirFilterTestCase, testSimpleMovingSumWithChangingInputSize) {
	FirFilter filter({ 1, 1 });
	assertAreEqual({ 1, 3, 5 }, filter({ 1, 2, 3 }));
	assertAreEqual({ 7, 9, 11, 13 }, filter({ 4, 5, 6, 7 }));
	assertAreEqual({ 15, 17 }, filter({ 8, 9 }));
}

TEST(FirFilterTestCase, testSimpleMovingSumWithInputSizeGoingAboveAndBelowCoefficientLength) {
	FirFilter filter({ 1, 1 });
	assertAreEqual({ 1 }, filter({ 1 }));
	assertAreEqual({ 3, 5 }, filter({ 2, 3 }));
	assertAreEqual({ 7, 9, 11 }, filter({ 4, 5, 6 }));
	assertAreEqual({ 13, 15 }, filter({ 7, 8 }));
	assertAreEqual({ 17 }, filter({ 9 }));
}

TEST(FirFilterTestCase, test2ndOrderMovingSumWithSuccessiveCalls) {
	FirFilter filter({ 1, 1, 1 });
	assertAreEqual({ 1 }, filter({ 1 }));
	assertAreEqual({ 3, 6 }, filter({ 2, 3 }));
	assertAreEqual({ 9, 12, 15 }, filter({ 4, 5, 6 }));
	assertAreEqual({ 18, 21 }, filter({ 7, 8 }));
	assertAreEqual({ 24 }, filter({ 9 }));
}

TEST(FirFilterTestCase, test2ndOrderMovingSumOneCall) {
	FirFilter filter({ 1, 1, 1 });
	assertAreEqual(
		{ 1, 3, 6, 9, 12, 15, 18, 21, 24 },
		filter({ 1, 2, 3, 4, 5, 6, 7, 8, 9 }));
}

TEST(FirFilterTestCase, testDelayedIdentity) {
	FirFilter filter({ 0, 0, 1 });
	assertAreEqual(
		{ 0, 0, 1, 2, 3, },
		filter({ 1, 2, 3, 4, 5 }));
	assertAreEqual(
		{ 4, 5 },
		filter({ 0, 0 }));
}

TEST(FirFilterTestCase, testPositiveCoefficients) {
	FirFilter filter({ 5, 3, 4, 2, 1 });
	assertAreEqual({ 5, 13, 25, 39, 54 }, filter({ 1, 2, 3, 4, 5 }));
	assertAreEqual({ 69, 84, 99, 114, 129 }, filter({ 6, 7, 8, 9, 10 }));
}

TEST(FirFilterTestCase, testNegativeCoefficients) {
	FirFilter filter({ -4, -2, -3, -5 });
	assertAreEqual({ -4, -10, -19 }, filter({ 1, 2, 3 }));
	assertAreEqual({ -33, -47, -61, -75, -89, -103, -117 }, filter({ 4, 5, 6, 7, 8, 9, 10 }));
}

TEST(FirFilterTestCase, testPositiveAndNegativeCoefficients) {
	FirFilter filter({ -1, 1, 0, 1, -1 });
	assertAreEqual({ -1, -1, -1, 0, 0 }, filter({ 1, 2, 3, 4, 5 }));
	assertAreEqual({}, filter({}));
	assertAreEqual({ 0, 0 }, filter({ 6, 7 }));
	assertAreEqual({ 0, 0, 0 }, filter({ 8, 9, 10 }));
}*/