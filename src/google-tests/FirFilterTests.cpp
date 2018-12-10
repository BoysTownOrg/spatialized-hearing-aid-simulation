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

TEST(FirFilterTestCase, emptyCoefficientsThrowsException) {
	EXPECT_THROW(FirFilter{ {} }, FirFilter::InvalidCoefficients);
}

static void assertCoefficientsYieldFilteredOutput(
	std::vector<float> b,
	std::vector<float> input,
	std::vector<float> output);

TEST(FirFilterTestCase, zeroIR) {
	assertCoefficientsYieldFilteredOutput({ 0 }, { 1, 2, 3 }, { 0, 0, 0 });
	assertCoefficientsYieldFilteredOutput({ 0, 0 }, { 1, 2, 3 }, { 0, 0, 0 });
	assertCoefficientsYieldFilteredOutput({ 0, 0, 0 }, { 1, 2, 3 }, { 0, 0, 0 });
}

void assertCoefficientsYieldFilteredOutput(
	std::vector<float> b,
	std::vector<float> input,
	std::vector<float> output)
{
	FirFilter filter{ std::move(b) };
	filter.process(&input[0], input.size());
	assertEqual(output, input, 1e-5f);
}

TEST(FirFilterTestCase, identityFilter) {
	assertCoefficientsYieldFilteredOutput({ 1 }, { 1, 2, 3 }, { 1, 2, 3 });
	assertCoefficientsYieldFilteredOutput({ 1, 0 }, { 1, 2, 3 }, { 1, 2, 3 });
	assertCoefficientsYieldFilteredOutput({ 1, 0, 0 }, { 1, 2, 3 }, { 1, 2, 3 });
}

TEST(FirFilterTestCase, doublingFilter) {
	assertCoefficientsYieldFilteredOutput({ 2 }, { 1, 2, 3 }, { 2, 4, 6 });
	assertCoefficientsYieldFilteredOutput({ 2, 0 }, { 1, 2, 3 }, { 2, 4, 6 });
	assertCoefficientsYieldFilteredOutput({ 2, 0, 0 }, { 1, 2, 3 }, { 2, 4, 6 });
}

TEST(FirFilterTestCase, simpleMovingSum) {
	assertCoefficientsYieldFilteredOutput({ 1, 1 }, { 1, 2, 3 }, { 1, 3, 5 });
	assertCoefficientsYieldFilteredOutput({ 1, 1, 0 }, { 1, 2, 3 }, { 1, 3, 5 });
	assertCoefficientsYieldFilteredOutput({ 1, 1, 0, 0 }, { 1, 2, 3 }, { 1, 3, 5 });
}

TEST(FirFilterTestCase, zeroIRWithSuccessiveCalls) {
	FirFilterFacade filter{ { 0 } };
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }), 1e-5f);
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }), 1e-5f);
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }), 1e-5f);
}

TEST(FirFilterTestCase, identityFilterWithSuccessiveCalls) {
	FirFilterFacade filter{ { 1 } };
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }), 1e-5f);
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }), 1e-5f);
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }), 1e-5f);
}

TEST(FirFilterTestCase, simpleMovingSumWithSuccessiveCalls) {
	FirFilterFacade filter{ { 1, 1 } };
	assertEqual({ 1, 3, 5 }, filter.process({ 1, 2, 3 }), 1e-5f);
	assertEqual({ 4, 3, 5 }, filter.process({ 1, 2, 3 }), 1e-5f);
	assertEqual({ 4, 3, 5 }, filter.process({ 1, 2, 3 }), 1e-5f);
}

TEST(FirFilterTestCase, simpleMovingSumWithIncreasingInputSize) {
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1, 3 }, filter.process({ 1, 2 }), 1e-5f);
	assertEqual({ 5, 7, 9 }, filter.process({ 3, 4, 5 }), 1e-5f);
	assertEqual({ 11, 13, 15, 17 }, filter.process({ 6, 7, 8, 9 }), 1e-5f);
}

TEST(FirFilterTestCase, simpleMovingSumWithDecreasingInputSize) {
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1, 3, 5, 7 }, filter.process({ 1, 2, 3, 4 }), 1e-5f);
	assertEqual({ 9, 11, 13 }, filter.process({ 5, 6, 7 }), 1e-5f);
	assertEqual({ 15, 17 }, filter.process({ 8, 9 }), 1e-5f);
}

TEST(FirFilterTestCase, simpleMovingSumWithChangingInputSize) {
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1, 3, 5 }, filter.process({ 1, 2, 3 }), 1e-5f);
	assertEqual({ 7, 9, 11, 13 }, filter.process({ 4, 5, 6, 7 }), 1e-5f);
	assertEqual({ 15, 17 }, filter.process({ 8, 9 }), 1e-5f);
}

TEST(
	FirFilterTestCase, 
	simpleMovingSumWithInputSizeGoingAboveAndBelowCoefficientLength) 
{
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1 }, filter.process({ 1 }), 1e-5f);
	assertEqual({ 3, 5 }, filter.process({ 2, 3 }), 1e-5f);
	assertEqual({ 7, 9, 11 }, filter.process({ 4, 5, 6 }), 1e-5f);
	assertEqual({ 13, 15 }, filter.process({ 7, 8 }), 1e-5f);
	assertEqual({ 17 }, filter.process({ 9 }), 1e-5f);
}

TEST(FirFilterTestCase, secondOrderMovingSumWithSuccessiveCalls) {
	FirFilterFacade filter({ 1, 1, 1 });
	assertEqual({ 1 }, filter.process({ 1 }), 1e-5f);
	assertEqual({ 3, 6 }, filter.process({ 2, 3 }), 1e-5f);
	assertEqual({ 9, 12, 15 }, filter.process({ 4, 5, 6 }), 1e-5f);
	assertEqual({ 18, 21 }, filter.process({ 7, 8 }), 1e-5f);
	assertEqual({ 24 }, filter.process({ 9 }), 1e-5f);
}

TEST(FirFilterTestCase, secondOrderMovingSumOneCall) {
	FirFilterFacade filter({ 1, 1, 1 });
	assertEqual(
		{ 1, 3, 6, 9, 12, 15, 18, 21, 24 },
		filter.process({ 1, 2, 3, 4, 5, 6, 7, 8, 9 }), 1e-5f);
}

TEST(FirFilterTestCase, delayedIdentity) {
	FirFilterFacade filter({ 0, 0, 1 });
	assertEqual(
		{ 0, 0, 1, 2, 3, },
		filter.process({ 1, 2, 3, 4, 5 }), 1e-5f);
	assertEqual(
		{ 4, 5 },
		filter.process({ 0, 0 }), 1e-5f);
}

TEST(FirFilterTestCase, positiveCoefficients) {
	FirFilterFacade filter({ 5, 3, 4, 2, 1 });
	assertEqual({ 5, 13, 25, 39, 54 }, filter.process({ 1, 2, 3, 4, 5 }), 1e-5f);
	assertEqual({ 69, 84, 99, 114, 129 }, filter.process({ 6, 7, 8, 9, 10 }), 1e-5f);
}

TEST(FirFilterTestCase, negativeCoefficients) {
	FirFilterFacade filter({ -4, -2, -3, -5 });
	assertEqual({ -4, -10, -19 }, filter.process({ 1, 2, 3 }), 1e-4f);
	assertEqual(
		{ -33, -47, -61, -75, -89, -103, -117 }, 
		filter.process({ 4, 5, 6, 7, 8, 9, 10 }), 1e-4f);
}

TEST(FirFilterTestCase, positiveAndNegativeCoefficients) {
	FirFilterFacade filter({ -1, 1, 0, 1, -1 });
	assertEqual({ -1, -1, -1, 0, 0 }, filter.process({ 1, 2, 3, 4, 5 }), 1e-5f);
	assertEqual({ 0, 0 }, filter.process({ 6, 7 }), 1e-5f);
	assertEqual({ 0, 0, 0 }, filter.process({ 8, 9, 10 }), 1e-5f);
}