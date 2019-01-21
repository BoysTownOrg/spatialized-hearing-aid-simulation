#include "assert-utility.h"
#include <fir-filtering/FirFilter.h>
#include <gtest/gtest.h>

namespace {
	class FirFilterFacade {
		FirFilter filter_;
	public:
		explicit FirFilterFacade(std::vector<float> b) :
			filter_{ std::move(b) } {}

		std::vector<float> filter(std::vector<float> x) {
			filter_.process(x);
			return x;
		}
	};

	TEST(FirFilterTests, emptyCoefficientsThrowsException) {
		EXPECT_THROW(FirFilter{ {} }, FirFilter::InvalidCoefficients);
	}

	TEST(FirFilterTests, groupDelayReturnsHalfFilterOrder) {
		FirFilter filter{ std::vector<float>(256 + 1) };
		EXPECT_EQ(128, filter.groupDelay());
	}

	TEST(FirFilterTests, zeroIRZeroOrder) {
		FirFilterFacade filter{ { 0 } };
		assertEqual({ 0, 0, 0 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, zeroIRFirstOrder) {
		FirFilterFacade filter{ { 0, 0 } };
		assertEqual({ 0, 0, 0 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, zeroIRSecondOrder) {
		FirFilterFacade filter{ { 0, 0, 0 } };
		assertEqual({ 0, 0, 0 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, identityFilterZeroOrder) {
		FirFilterFacade filter{ { 1 } };
		assertEqual({ 1, 2, 3 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, identityFilterFirstOrder) {
		FirFilterFacade filter{ { 1, 0 } };
		assertEqual({ 1, 2, 3 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, identityFilterSecondOrder) {
		FirFilterFacade filter{ { 1, 0, 0 } };
		assertEqual({ 1, 2, 3 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, doublingFilterZeroOrder) {
		FirFilterFacade filter{ { 2 } };
		assertEqual({ 2, 4, 6 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, doublingFilterFirstOrder) {
		FirFilterFacade filter{ { 2, 0 } };
		assertEqual({ 2, 4, 6 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, doublingFilterSecondOrder) {
		FirFilterFacade filter{ { 2, 0, 0 } };
		assertEqual({ 2, 4, 6 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, movingSumFirstOrder) {
		FirFilterFacade filter{ { 1, 1 } };
		assertEqual({ 1, 3, 5 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, movingSumSecondOrder) {
		FirFilterFacade filter{ { 1, 1, 0 } };
		assertEqual({ 1, 3, 5 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, movingSumThirdOrder) {
		FirFilterFacade filter{ { 1, 1, 0, 0 } };
		assertEqual({ 1, 3, 5 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, zeroIRWithSuccessiveCalls) {
		FirFilterFacade filter{ { 0 } };
		assertEqual({ 0, 0, 0 }, filter.filter({ 1, 2, 3 }));
		assertEqual({ 0, 0, 0 }, filter.filter({ 1, 2, 3 }));
		assertEqual({ 0, 0, 0 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, identityFilterWithSuccessiveCalls) {
		FirFilterFacade filter{ { 1 } };
		assertEqual({ 1, 2, 3 }, filter.filter({ 1, 2, 3 }));
		assertEqual({ 1, 2, 3 }, filter.filter({ 1, 2, 3 }));
		assertEqual({ 1, 2, 3 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, movingSumWithSuccessiveCalls) {
		FirFilterFacade filter{ { 1, 1 } };
		assertEqual({ 1, 3, 5 }, filter.filter({ 1, 2, 3 }));
		assertEqual({ 4, 3, 5 }, filter.filter({ 1, 2, 3 }));
		assertEqual({ 4, 3, 5 }, filter.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, movingSumWithIncreasingInputSize) {
		FirFilterFacade filter({ 1, 1 });
		assertEqual({ 1, 3 }, filter.filter({ 1, 2 }));
		assertEqual({ 5, 7, 9 }, filter.filter({ 3, 4, 5 }));
		assertEqual({ 11, 13, 15, 17 }, filter.filter({ 6, 7, 8, 9 }));
	}

	TEST(FirFilterTests, movingSumWithDecreasingInputSize) {
		FirFilterFacade filter({ 1, 1 });
		assertEqual({ 1, 3, 5, 7 }, filter.filter({ 1, 2, 3, 4 }));
		assertEqual({ 9, 11, 13 }, filter.filter({ 5, 6, 7 }));
		assertEqual({ 15, 17 }, filter.filter({ 8, 9 }));
	}

	TEST(FirFilterTests, movingSumWithChangingInputSize) {
		FirFilterFacade filter({ 1, 1 });
		assertEqual({ 1, 3, 5 }, filter.filter({ 1, 2, 3 }));
		assertEqual({ 7, 9, 11, 13 }, filter.filter({ 4, 5, 6, 7 }));
		assertEqual({ 15, 17 }, filter.filter({ 8, 9 }));
	}

	TEST(
		FirFilterTests,
		movingSumWithInputSizeGoingAboveAndBelowCoefficientLength
	) {
		FirFilterFacade filter({ 1, 1 });
		assertEqual({ 1 }, filter.filter({ 1 }));
		assertEqual({ 3, 5 }, filter.filter({ 2, 3 }));
		assertEqual({ 7, 9, 11 }, filter.filter({ 4, 5, 6 }));
		assertEqual({ 13, 15 }, filter.filter({ 7, 8 }));
		assertEqual({ 17 }, filter.filter({ 9 }));
	}

	TEST(FirFilterTests, secondOrderMovingSumWithSuccessiveCalls) {
		FirFilterFacade filter({ 1, 1, 1 });
		assertEqual({ 1 }, filter.filter({ 1 }));
		assertEqual({ 3, 6 }, filter.filter({ 2, 3 }));
		assertEqual({ 9, 12, 15 }, filter.filter({ 4, 5, 6 }));
		assertEqual({ 18, 21 }, filter.filter({ 7, 8 }));
		assertEqual({ 24 }, filter.filter({ 9 }));
	}

	TEST(FirFilterTests, secondOrderMovingSumOneCall) {
		FirFilterFacade filter({ 1, 1, 1 });
		assertEqual(
			{ 1, 3, 6, 9, 12, 15, 18, 21, 24 },
			filter.filter({ 1, 2, 3, 4, 5, 6, 7, 8, 9 })
		);
	}

	TEST(FirFilterTests, delayedIdentity) {
		FirFilterFacade filter({ 0, 0, 1 });
		assertEqual(
			{ 0, 0, 1, 2, 3, },
			filter.filter({ 1, 2, 3, 4, 5 })
		);
		assertEqual(
			{ 4, 5 },
			filter.filter({ 0, 0 })
		);
	}

	TEST(FirFilterTests, positiveCoefficients) {
		FirFilterFacade filter({ 5, 3, 4, 2, 1 });
		assertEqual({ 5, 13, 25, 39, 54 }, filter.filter({ 1, 2, 3, 4, 5 }));
		assertEqual({ 69, 84, 99, 114, 129 }, filter.filter({ 6, 7, 8, 9, 10 }));
	}

	TEST(FirFilterTests, negativeCoefficients) {
		FirFilterFacade filter({ -4, -2, -3, -5 });
		assertEqual({ -4, -10, -19 }, filter.filter({ 1, 2, 3 }));
		assertEqual(
			{ -33, -47, -61, -75, -89, -103, -117 },
			filter.filter({ 4, 5, 6, 7, 8, 9, 10 })
		);
	}

	TEST(FirFilterTests, positiveAndNegativeCoefficients) {
		FirFilterFacade filter({ -1, 1, 0, 1, -1 });
		assertEqual({ -1, -1, -1, 0, 0 }, filter.filter({ 1, 2, 3, 4, 5 }), 1e-6f);
		assertEqual({ 0, 0 }, filter.filter({ 6, 7 }), 1e-6f);
		assertEqual({ 0, 0, 0 }, filter.filter({ 8, 9, 10 }), 1e-6f);
	}
}