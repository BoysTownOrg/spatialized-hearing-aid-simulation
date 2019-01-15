#include "assert-utility.h"
#include <fir-filtering/FirFilter.h>
#include <gtest/gtest.h>

class FirFilterFacade {
	FirFilter filter;
public:
	explicit FirFilterFacade(std::vector<float> b) :
		filter{ std::move(b) } {}

	std::vector<float> process(std::vector<float> x) {
		filter.process(x);
		return x;
	}
};

class FirFilterTestCase : public ::testing::TestCase {};

TEST(FirFilterTestCase, emptyCoefficientsThrowsException) {
	EXPECT_THROW(FirFilter{ {} }, FirFilter::InvalidCoefficients);
}

TEST(FirFilterTestCase, groupDelayReturnsHalfFilterOrder) {
	FirFilter filter{ std::vector<float>(256 + 1) };
	EXPECT_EQ(128, filter.groupDelay());
}

TEST(FirFilterTestCase, zeroIRZeroOrder) {
	FirFilterFacade filter{ { 0 } };
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, zeroIRFirstOrder) {
	FirFilterFacade filter{ { 0, 0 } };
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, zeroIRSecondOrder) {
	FirFilterFacade filter{ { 0, 0, 0 } };
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, identityFilterZeroOrder) {
	FirFilterFacade filter{ { 1 } };
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, identityFilterFirstOrder) {
	FirFilterFacade filter{ { 1, 0 } };
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, identityFilterSecondOrder) {
	FirFilterFacade filter{ { 1, 0, 0 } };
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, doublingFilterZeroOrder) {
	FirFilterFacade filter{ { 2 } };
	assertEqual({ 2, 4, 6 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, doublingFilterFirstOrder) {
	FirFilterFacade filter{ { 2, 0 } };
	assertEqual({ 2, 4, 6 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, doublingFilterSecondOrder) {
	FirFilterFacade filter{ { 2, 0, 0 } };
	assertEqual({ 2, 4, 6 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, movingSumFirstOrder) {
	FirFilterFacade filter{ { 1, 1 } };
	assertEqual({ 1, 3, 5 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, movingSumSecondOrder) {
	FirFilterFacade filter{ { 1, 1, 0 } };
	assertEqual({ 1, 3, 5 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, movingSumThirdOrder) {
	FirFilterFacade filter{ { 1, 1, 0, 0 } };
	assertEqual({ 1, 3, 5 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, zeroIRWithSuccessiveCalls) {
	FirFilterFacade filter{ { 0 } };
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 0, 0, 0 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, identityFilterWithSuccessiveCalls) {
	FirFilterFacade filter{ { 1 } };
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 1, 2, 3 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, movingSumWithSuccessiveCalls) {
	FirFilterFacade filter{ { 1, 1 } };
	assertEqual({ 1, 3, 5 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 4, 3, 5 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 4, 3, 5 }, filter.process({ 1, 2, 3 }));
}

TEST(FirFilterTestCase, movingSumWithIncreasingInputSize) {
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1, 3 }, filter.process({ 1, 2 }));
	assertEqual({ 5, 7, 9 }, filter.process({ 3, 4, 5 }));
	assertEqual({ 11, 13, 15, 17 }, filter.process({ 6, 7, 8, 9 }));
}

TEST(FirFilterTestCase, movingSumWithDecreasingInputSize) {
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1, 3, 5, 7 }, filter.process({ 1, 2, 3, 4 }));
	assertEqual({ 9, 11, 13 }, filter.process({ 5, 6, 7 }));
	assertEqual({ 15, 17 }, filter.process({ 8, 9 }));
}

TEST(FirFilterTestCase, movingSumWithChangingInputSize) {
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1, 3, 5 }, filter.process({ 1, 2, 3 }));
	assertEqual({ 7, 9, 11, 13 }, filter.process({ 4, 5, 6, 7 }));
	assertEqual({ 15, 17 }, filter.process({ 8, 9 }));
}

TEST(
	FirFilterTestCase, 
	movingSumWithInputSizeGoingAboveAndBelowCoefficientLength
) {
	FirFilterFacade filter({ 1, 1 });
	assertEqual({ 1 }, filter.process({ 1 }));
	assertEqual({ 3, 5 }, filter.process({ 2, 3 }));
	assertEqual({ 7, 9, 11 }, filter.process({ 4, 5, 6 }));
	assertEqual({ 13, 15 }, filter.process({ 7, 8 }));
	assertEqual({ 17 }, filter.process({ 9 }));
}

TEST(FirFilterTestCase, secondOrderMovingSumWithSuccessiveCalls) {
	FirFilterFacade filter({ 1, 1, 1 });
	assertEqual({ 1 }, filter.process({ 1 }));
	assertEqual({ 3, 6 }, filter.process({ 2, 3 }));
	assertEqual({ 9, 12, 15 }, filter.process({ 4, 5, 6 }));
	assertEqual({ 18, 21 }, filter.process({ 7, 8 }));
	assertEqual({ 24 }, filter.process({ 9 }));
}

TEST(FirFilterTestCase, secondOrderMovingSumOneCall) {
	FirFilterFacade filter({ 1, 1, 1 });
	assertEqual(
		{ 1, 3, 6, 9, 12, 15, 18, 21, 24 },
		filter.process({ 1, 2, 3, 4, 5, 6, 7, 8, 9 })
	);
}

TEST(FirFilterTestCase, delayedIdentity) {
	FirFilterFacade filter({ 0, 0, 1 });
	assertEqual(
		{ 0, 0, 1, 2, 3, },
		filter.process({ 1, 2, 3, 4, 5 })
	);
	assertEqual(
		{ 4, 5 },
		filter.process({ 0, 0 })
	);
}

TEST(FirFilterTestCase, positiveCoefficients) {
	FirFilterFacade filter({ 5, 3, 4, 2, 1 });
	assertEqual({ 5, 13, 25, 39, 54 }, filter.process({ 1, 2, 3, 4, 5 }));
	assertEqual({ 69, 84, 99, 114, 129 }, filter.process({ 6, 7, 8, 9, 10 }));
}

TEST(FirFilterTestCase, negativeCoefficients) {
	FirFilterFacade filter({ -4, -2, -3, -5 });
	assertEqual({ -4, -10, -19 }, filter.process({ 1, 2, 3 }));
	assertEqual(
		{ -33, -47, -61, -75, -89, -103, -117 }, 
		filter.process({ 4, 5, 6, 7, 8, 9, 10 })
	);
}

TEST(FirFilterTestCase, positiveAndNegativeCoefficients) {
	FirFilterFacade filter({ -1, 1, 0, 1, -1 });
	assertEqual({ -1, -1, -1, 0, 0 }, filter.process({ 1, 2, 3, 4, 5 }), 1e-6f);
	assertEqual({ 0, 0 }, filter.process({ 6, 7 }), 1e-6f);
	assertEqual({ 0, 0, 0 }, filter.process({ 8, 9, 10 }), 1e-6f);
}