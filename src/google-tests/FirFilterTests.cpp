#include "assert-utility.h"
#include <fir-filtering/FirFilter.h>
#include <gtest/gtest.h>

namespace {
	class FirFilterFacade {
		FirFilter filter_;
	public:
		using coefficients_type = FirFilter::coefficients_type;
		using signal_type = std::vector<FirFilter::signal_type::element_type>;

		explicit FirFilterFacade(coefficients_type b) :
			filter_{ std::move(b) } {}

		signal_type filter(signal_type x) {
			filter_.process(x);
			return x;
		}
	};

	TEST(FirFilterTests, emptyCoefficientsThrowsException) {
		EXPECT_THROW(FirFilter{ {} }, FirFilter::InvalidCoefficients);
	}

	TEST(FirFilterTests, groupDelayReturnsHalfFilterOrder) {
		FirFilter filter{ FirFilter::coefficients_type(256 + 1) };
		assertEqual(128, filter.groupDelay());
	}

	TEST(FirFilterTests, zeroIRZeroOrder) {
		FirFilterFacade facade{ { 0 } };
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, zeroIRFirstOrder) {
		FirFilterFacade facade{ { 0, 0 } };
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, zeroIRSecondOrder) {
		FirFilterFacade facade{ { 0, 0, 0 } };
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, identityFilterZeroOrder) {
		FirFilterFacade facade{ { 1 } };
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, identityFilterFirstOrder) {
		FirFilterFacade facade{ { 1, 0 } };
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, identityFilterSecondOrder) {
		FirFilterFacade facade{ { 1, 0, 0 } };
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, doublingFilterZeroOrder) {
		FirFilterFacade facade{ { 2 } };
		assertEqual({ 2, 4, 6 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, doublingFilterFirstOrder) {
		FirFilterFacade facade{ { 2, 0 } };
		assertEqual({ 2, 4, 6 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, doublingFilterSecondOrder) {
		FirFilterFacade facade{ { 2, 0, 0 } };
		assertEqual({ 2, 4, 6 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, movingSumFirstOrder) {
		FirFilterFacade facade{ { 1, 1 } };
		assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, movingSumSecondOrder) {
		FirFilterFacade facade{ { 1, 1, 0 } };
		assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, movingSumThirdOrder) {
		FirFilterFacade facade{ { 1, 1, 0, 0 } };
		assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, zeroIRWithSuccessiveCalls) {
		FirFilterFacade facade{ { 0 } };
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, identityFilterWithSuccessiveCalls) {
		FirFilterFacade facade{ { 1 } };
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, movingSumWithSuccessiveCalls) {
		FirFilterFacade facade{ { 1, 1 } };
		assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 4, 3, 5 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 4, 3, 5 }, facade.filter({ 1, 2, 3 }));
	}

	TEST(FirFilterTests, movingSumWithIncreasingInputSize) {
		FirFilterFacade facade{ { 1, 1 } };
		assertEqual({ 1, 3 }, facade.filter({ 1, 2 }));
		assertEqual({ 5, 7, 9 }, facade.filter({ 3, 4, 5 }));
		assertEqual({ 11, 13, 15, 17 }, facade.filter({ 6, 7, 8, 9 }));
	}

	TEST(FirFilterTests, movingSumWithDecreasingInputSize) {
		FirFilterFacade facade{ { 1, 1 } };
		assertEqual({ 1, 3, 5, 7 }, facade.filter({ 1, 2, 3, 4 }));
		assertEqual({ 9, 11, 13 }, facade.filter({ 5, 6, 7 }));
		assertEqual({ 15, 17 }, facade.filter({ 8, 9 }));
	}

	TEST(FirFilterTests, movingSumWithChangingInputSize) {
		FirFilterFacade facade{ { 1, 1 } };
		assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 7, 9, 11, 13 }, facade.filter({ 4, 5, 6, 7 }));
		assertEqual({ 15, 17 }, facade.filter({ 8, 9 }));
	}

	TEST(
		FirFilterTests,
		movingSumWithInputSizeGoingAboveAndBelowCoefficientLength
	) {
		FirFilterFacade facade{ { 1, 1 } };
		assertEqual({ 1 }, facade.filter({ 1 }));
		assertEqual({ 3, 5 }, facade.filter({ 2, 3 }));
		assertEqual({ 7, 9, 11 }, facade.filter({ 4, 5, 6 }));
		assertEqual({ 13, 15 }, facade.filter({ 7, 8 }));
		assertEqual({ 17 }, facade.filter({ 9 }));
	}

	TEST(FirFilterTests, secondOrderMovingSumWithSuccessiveCalls) {
		FirFilterFacade facade{ { 1, 1, 1 } };
		assertEqual({ 1 }, facade.filter({ 1 }));
		assertEqual({ 3, 6 }, facade.filter({ 2, 3 }));
		assertEqual({ 9, 12, 15 }, facade.filter({ 4, 5, 6 }));
		assertEqual({ 18, 21 }, facade.filter({ 7, 8 }));
		assertEqual({ 24 }, facade.filter({ 9 }));
	}

	TEST(FirFilterTests, secondOrderMovingSumOneCall) {
		FirFilterFacade facade{ { 1, 1, 1 } };
		assertEqual(
			{ 1, 3, 6, 9, 12, 15, 18, 21, 24 },
			facade.filter({ 1, 2, 3, 4, 5, 6, 7, 8, 9 })
		);
	}

	TEST(FirFilterTests, delayedIdentity) {
		FirFilterFacade facade{ { 0, 0, 1 } };
		assertEqual(
			{ 0, 0, 1, 2, 3, },
			facade.filter({ 1, 2, 3, 4, 5 })
		);
		assertEqual(
			{ 4, 5 },
			facade.filter({ 0, 0 })
		);
	}

	TEST(FirFilterTests, positiveCoefficients) {
		FirFilterFacade facade{ { 5, 3, 4, 2, 1 } };
		assertEqual({ 5, 13, 25, 39, 54 }, facade.filter({ 1, 2, 3, 4, 5 }));
		assertEqual({ 69, 84, 99, 114, 129 }, facade.filter({ 6, 7, 8, 9, 10 }));
	}

	TEST(FirFilterTests, negativeCoefficients) {
		FirFilterFacade facade{ { -4, -2, -3, -5 } };
		assertEqual({ -4, -10, -19 }, facade.filter({ 1, 2, 3 }));
		assertEqual(
			{ -33, -47, -61, -75, -89, -103, -117 },
			facade.filter({ 4, 5, 6, 7, 8, 9, 10 })
		);
	}

	TEST(FirFilterTests, positiveAndNegativeCoefficients) {
		FirFilterFacade facade{ { -1, 1, 0, 1, -1 } };
		assertEqual({ -1, -1, -1, 0, 0 }, facade.filter({ 1, 2, 3, 4, 5 }), 1e-6f);
		assertEqual({ 0, 0 }, facade.filter({ 6, 7 }), 1e-6f);
		assertEqual({ 0, 0, 0 }, facade.filter({ 8, 9, 10 }), 1e-6f);
	}
}