#include "assert-utility.h"
#include <fir-filtering/FirFilter.h>
#include <gtest/gtest.h>

namespace {
	template<typename T>
	class FirFilterFacade {
		FirFilter<T> filter_;
	public:
		using coefficients_type = typename FirFilter<T>::coefficients_type;
		using signal_type = std::vector<T>;

		explicit FirFilterFacade(coefficients_type b) :
			filter_{ std::move(b) } {}

		signal_type filter(signal_type x) {
			filter_.process(x);
			return x;
		}
	};

	class FirFilterTests : public ::testing::Test {};

	TEST_F(FirFilterTests, emptyCoefficientsThrowsException) {
		EXPECT_THROW(FirFilter<float>{ {} }, FirFilter<float>::InvalidCoefficients);
	}

	TEST_F(FirFilterTests, groupDelayReturnsHalfFilterOrder) {
		FirFilter<float> filter{ FirFilter<float>::coefficients_type(256 + 1) };
		assertEqual(128, filter.groupDelay());
	}

	TEST_F(FirFilterTests, zeroIRZeroOrder) {
		FirFilterFacade<float> facade{ { 0 } };
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, zeroIRFirstOrder) {
		FirFilterFacade<float> facade{ { 0, 0 } };
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, zeroIRSecondOrder) {
		FirFilterFacade<float> facade{ { 0, 0, 0 } };
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, identityFilterZeroOrder) {
		FirFilterFacade<float> facade{ { 1 } };
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, identityFilterFirstOrder) {
		FirFilterFacade<float> facade{ { 1, 0 } };
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, identityFilterSecondOrder) {
		FirFilterFacade<float> facade{ { 1, 0, 0 } };
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, doublingFilterZeroOrder) {
		FirFilterFacade<float> facade{ { 2 } };
		assertEqual({ 2, 4, 6 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, doublingFilterFirstOrder) {
		FirFilterFacade<float> facade{ { 2, 0 } };
		assertEqual({ 2, 4, 6 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, doublingFilterSecondOrder) {
		FirFilterFacade<float> facade{ { 2, 0, 0 } };
		assertEqual({ 2, 4, 6 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, movingSumFirstOrder) {
		FirFilterFacade<float> facade{ { 1, 1 } };
		assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, movingSumSecondOrder) {
		FirFilterFacade<float> facade{ { 1, 1, 0 } };
		assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, movingSumThirdOrder) {
		FirFilterFacade<float> facade{ { 1, 1, 0, 0 } };
		assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, zeroIRWithSuccessiveCalls) {
		FirFilterFacade<float> facade{ { 0 } };
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, identityFilterWithSuccessiveCalls) {
		FirFilterFacade<float> facade{ { 1 } };
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, movingSumWithSuccessiveCalls) {
		FirFilterFacade<float> facade{ { 1, 1 } };
		assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 4, 3, 5 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 4, 3, 5 }, facade.filter({ 1, 2, 3 }));
	}

	TEST_F(FirFilterTests, movingSumWithIncreasingInputSize) {
		FirFilterFacade<float> facade{ { 1, 1 } };
		assertEqual({ 1, 3 }, facade.filter({ 1, 2 }));
		assertEqual({ 5, 7, 9 }, facade.filter({ 3, 4, 5 }));
		assertEqual({ 11, 13, 15, 17 }, facade.filter({ 6, 7, 8, 9 }));
	}

	TEST_F(FirFilterTests, movingSumWithDecreasingInputSize) {
		FirFilterFacade<float> facade{ { 1, 1 } };
		assertEqual({ 1, 3, 5, 7 }, facade.filter({ 1, 2, 3, 4 }));
		assertEqual({ 9, 11, 13 }, facade.filter({ 5, 6, 7 }));
		assertEqual({ 15, 17 }, facade.filter({ 8, 9 }));
	}

	TEST_F(FirFilterTests, movingSumWithChangingInputSize) {
		FirFilterFacade<float> facade{ { 1, 1 } };
		assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
		assertEqual({ 7, 9, 11, 13 }, facade.filter({ 4, 5, 6, 7 }));
		assertEqual({ 15, 17 }, facade.filter({ 8, 9 }));
	}

	TEST_F(
		FirFilterTests,
		movingSumWithInputSizeGoingAboveAndBelowCoefficientLength
	) {
		FirFilterFacade<float> facade{ { 1, 1 } };
		assertEqual({ 1 }, facade.filter({ 1 }));
		assertEqual({ 3, 5 }, facade.filter({ 2, 3 }));
		assertEqual({ 7, 9, 11 }, facade.filter({ 4, 5, 6 }));
		assertEqual({ 13, 15 }, facade.filter({ 7, 8 }));
		assertEqual({ 17 }, facade.filter({ 9 }));
	}

	TEST_F(FirFilterTests, secondOrderMovingSumWithSuccessiveCalls) {
		FirFilterFacade<float> facade{ { 1, 1, 1 } };
		assertEqual({ 1 }, facade.filter({ 1 }));
		assertEqual({ 3, 6 }, facade.filter({ 2, 3 }));
		assertEqual({ 9, 12, 15 }, facade.filter({ 4, 5, 6 }));
		assertEqual({ 18, 21 }, facade.filter({ 7, 8 }));
		assertEqual({ 24 }, facade.filter({ 9 }));
	}

	TEST_F(FirFilterTests, secondOrderMovingSumOneCall) {
		FirFilterFacade<float> facade{ { 1, 1, 1 } };
		assertEqual(
			{ 1, 3, 6, 9, 12, 15, 18, 21, 24 },
			facade.filter({ 1, 2, 3, 4, 5, 6, 7, 8, 9 })
		);
	}

	TEST_F(FirFilterTests, delayedIdentity) {
		FirFilterFacade<float> facade{ { 0, 0, 1 } };
		assertEqual(
			{ 0, 0, 1, 2, 3, },
			facade.filter({ 1, 2, 3, 4, 5 })
		);
		assertEqual(
			{ 4, 5 },
			facade.filter({ 0, 0 })
		);
	}

	TEST_F(FirFilterTests, positiveCoefficients) {
		FirFilterFacade<float> facade{ { 5, 3, 4, 2, 1 } };
		assertEqual({ 5, 13, 25, 39, 54 }, facade.filter({ 1, 2, 3, 4, 5 }));
		assertEqual({ 69, 84, 99, 114, 129 }, facade.filter({ 6, 7, 8, 9, 10 }));
	}

	TEST_F(FirFilterTests, negativeCoefficients) {
		FirFilterFacade<float> facade{ { -4, -2, -3, -5 } };
		assertEqual({ -4, -10, -19 }, facade.filter({ 1, 2, 3 }));
		assertEqual(
			{ -33, -47, -61, -75, -89, -103, -117 },
			facade.filter({ 4, 5, 6, 7, 8, 9, 10 })
		);
	}

	TEST_F(FirFilterTests, positiveAndNegativeCoefficients) {
		FirFilterFacade<float> facade{ { -1, 1, 0, 1, -1 } };
		assertEqual({ -1, -1, -1, 0, 0 }, facade.filter({ 1, 2, 3, 4, 5 }), 1e-6f);
		assertEqual({ 0, 0 }, facade.filter({ 6, 7 }), 1e-6f);
		assertEqual({ 0, 0, 0 }, facade.filter({ 8, 9, 10 }), 1e-6f);
	}
}