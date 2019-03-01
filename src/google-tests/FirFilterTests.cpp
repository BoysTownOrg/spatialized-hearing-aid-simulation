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

	class FirFilterTests : public ::testing::Test {
	protected:
		template<typename T>
		void assertConstructorWithEmptyCoefficientsThrowsException() {
			EXPECT_THROW(FirFilter<T>{ {} }, typename FirFilter<T>::InvalidCoefficients);
		}

		template<typename T>
		void assertGroupDelayReturnsHalfFilterOrder() {
			FirFilter<T> filter{ typename FirFilter<T>::coefficients_type(256 + 1) };
			assertEqual(128, filter.groupDelay());
		}
		
		template<typename T>
		void zeroIRZeroOrder() {
			FirFilterFacade<T> facade{ { 0 } };
			assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void zeroIRFirstOrder() {
			FirFilterFacade<T> facade{ { 0, 0 } };
			assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void zeroIRSecondOrder() {
			FirFilterFacade<T> facade{ { 0, 0, 0 } };
			assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void identityFilterZeroOrder() {
			FirFilterFacade<T> facade{ { 1 } };
			assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void identityFilterFirstOrder() {
			FirFilterFacade<T> facade{ { 1, 0 } };
			assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void identityFilterSecondOrder() {
			FirFilterFacade<T> facade{ { 1, 0, 0 } };
			assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void doublingFilterZeroOrder() {
			FirFilterFacade<T> facade{ { 2 } };
			assertEqual({ 2, 4, 6 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void doublingFilterFirstOrder() {
			FirFilterFacade<T> facade{ { 2, 0 } };
			assertEqual({ 2, 4, 6 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void doublingFilterSecondOrder() {
			FirFilterFacade<T> facade{ { 2, 0, 0 } };
			assertEqual({ 2, 4, 6 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void movingSumFirstOrder() {
			FirFilterFacade<T> facade{ { 1, 1 } };
			assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void movingSumSecondOrder() {
			FirFilterFacade<T> facade{ { 1, 1, 0 } };
			assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void movingSumThirdOrder() {
			FirFilterFacade<T> facade{ { 1, 1, 0, 0 } };
			assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void zeroIRWithSuccessiveCalls() {
			FirFilterFacade<T> facade{ { 0 } };
			assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
			assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
			assertEqual({ 0, 0, 0 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void identityFilterWithSuccessiveCalls() {
			FirFilterFacade<T> facade{ { 1 } };
			assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
			assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
			assertEqual({ 1, 2, 3 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void movingSumWithSuccessiveCalls() {
			FirFilterFacade<T> facade{ { 1, 1 } };
			assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
			assertEqual({ 4, 3, 5 }, facade.filter({ 1, 2, 3 }));
			assertEqual({ 4, 3, 5 }, facade.filter({ 1, 2, 3 }));
		}
		
		template<typename T>
		void movingSumWithIncreasingInputSize() {
			FirFilterFacade<T> facade{ { 1, 1 } };
			assertEqual({ 1, 3 }, facade.filter({ 1, 2 }));
			assertEqual({ 5, 7, 9 }, facade.filter({ 3, 4, 5 }));
			assertEqual({ 11, 13, 15, 17 }, facade.filter({ 6, 7, 8, 9 }));
		}
		
		template<typename T>
		void movingSumWithDecreasingInputSize() {
			FirFilterFacade<T> facade{ { 1, 1 } };
			assertEqual({ 1, 3, 5, 7 }, facade.filter({ 1, 2, 3, 4 }));
			assertEqual({ 9, 11, 13 }, facade.filter({ 5, 6, 7 }));
			assertEqual({ 15, 17 }, facade.filter({ 8, 9 }));
		}
		
		template<typename T>
		void movingSumWithChangingInputSize() {
			FirFilterFacade<T> facade{ { 1, 1 } };
			assertEqual({ 1, 3, 5 }, facade.filter({ 1, 2, 3 }));
			assertEqual({ 7, 9, 11, 13 }, facade.filter({ 4, 5, 6, 7 }));
			assertEqual({ 15, 17 }, facade.filter({ 8, 9 }));
		}
		
		template<typename T>
		void movingSumWithInputSizeGoingAboveAndBelowCoefficientLength() {
			FirFilterFacade<T> facade{ { 1, 1 } };
			assertEqual({ 1 }, facade.filter({ 1 }));
			assertEqual({ 3, 5 }, facade.filter({ 2, 3 }));
			assertEqual({ 7, 9, 11 }, facade.filter({ 4, 5, 6 }));
			assertEqual({ 13, 15 }, facade.filter({ 7, 8 }));
			assertEqual({ 17 }, facade.filter({ 9 }));
		}
		
		template<typename T>
		void secondOrderMovingSumWithSuccessiveCalls() {
			FirFilterFacade<T> facade{ { 1, 1, 1 } };
			assertEqual({ 1 }, facade.filter({ 1 }));
			assertEqual({ 3, 6 }, facade.filter({ 2, 3 }));
			assertEqual({ 9, 12, 15 }, facade.filter({ 4, 5, 6 }));
			assertEqual({ 18, 21 }, facade.filter({ 7, 8 }));
			assertEqual({ 24 }, facade.filter({ 9 }));
		}
		
		template<typename T>
		void secondOrderMovingSumOneCall() {
			FirFilterFacade<T> facade{ { 1, 1, 1 } };
			assertEqual(
				{ 1, 3, 6, 9, 12, 15, 18, 21, 24 },
				facade.filter({ 1, 2, 3, 4, 5, 6, 7, 8, 9 })
			);
		}
		
		template<typename T>
		void delayedIdentity() {
			FirFilterFacade<T> facade{ { 0, 0, 1 } };
			assertEqual(
				{ 0, 0, 1, 2, 3, },
				facade.filter({ 1, 2, 3, 4, 5 })
			);
			assertEqual(
				{ 4, 5 },
				facade.filter({ 0, 0 })
			);
		}
		
		template<typename T>
		void positiveCoefficients() {
			FirFilterFacade<T> facade{ { 5, 3, 4, 2, 1 } };
			assertEqual({ 5, 13, 25, 39, 54 }, facade.filter({ 1, 2, 3, 4, 5 }));
			assertEqual({ 69, 84, 99, 114, 129 }, facade.filter({ 6, 7, 8, 9, 10 }));
		}
		
		template<typename T>
		void negativeCoefficients() {
			FirFilterFacade<T> facade{ { -4, -2, -3, -5 } };
			assertEqual({ -4, -10, -19 }, facade.filter({ 1, 2, 3 }));
			assertEqual(
				{ -33, -47, -61, -75, -89, -103, -117 },
				facade.filter({ 4, 5, 6, 7, 8, 9, 10 })
			);
		}

		template<typename T>
		constexpr T precision_order(T i) {
			return 1 / std::pow(T{ 10 }, i);
		}
		
		template<typename T>
		void positiveAndNegativeCoefficients() {
			FirFilterFacade<T> facade{ { -1, 1, 0, 1, -1 } };
			assertEqual({ -1, -1, -1, 0, 0 }, facade.filter({ 1, 2, 3, 4, 5 }), precision_order<T>(6));
			assertEqual({ 0, 0 }, facade.filter({ 6, 7 }), precision_order<T>(6));
			assertEqual({ 0, 0, 0 }, facade.filter({ 8, 9, 10 }), precision_order<T>(6));
		}
	};

	TEST_F(FirFilterTests, constructorWithEmptyCoefficientsThrowsException) {
		assertConstructorWithEmptyCoefficientsThrowsException<float>();
		assertConstructorWithEmptyCoefficientsThrowsException<double>();
	}

	TEST_F(FirFilterTests, groupDelayReturnsHalfFilterOrder) {
		assertGroupDelayReturnsHalfFilterOrder<float>();
		assertGroupDelayReturnsHalfFilterOrder<double>();
	}

	TEST_F(FirFilterTests, zeroIRZeroOrder) {
		zeroIRZeroOrder<float>();
		zeroIRZeroOrder<double>();
	}

	TEST_F(FirFilterTests, zeroIRFirstOrder) {
		zeroIRFirstOrder<float>();
		zeroIRFirstOrder<double>();
	}

	TEST_F(FirFilterTests, zeroIRSecondOrder) {
		zeroIRSecondOrder<float>();
		zeroIRSecondOrder<double>();
	}

	TEST_F(FirFilterTests, identityFilterZeroOrder) {
		identityFilterZeroOrder<float>();
		identityFilterZeroOrder<double>();
	}

	TEST_F(FirFilterTests, identityFilterFirstOrder) {
		identityFilterFirstOrder<float>();
		identityFilterFirstOrder<double>();
	}

	TEST_F(FirFilterTests, identityFilterSecondOrder) {
		identityFilterSecondOrder<float>();
		identityFilterSecondOrder<double>();
	}

	TEST_F(FirFilterTests, doublingFilterZeroOrder) {
		doublingFilterZeroOrder<float>();
		doublingFilterZeroOrder<double>();
	}

	TEST_F(FirFilterTests, doublingFilterFirstOrder) {
		doublingFilterFirstOrder<float>();
		doublingFilterFirstOrder<double>();
	}

	TEST_F(FirFilterTests, doublingFilterSecondOrder) {
		doublingFilterSecondOrder<float>();
		doublingFilterSecondOrder<double>();
	}

	TEST_F(FirFilterTests, movingSumFirstOrder) {
		movingSumFirstOrder<float>();
		movingSumFirstOrder<double>();
	}

	TEST_F(FirFilterTests, movingSumSecondOrder) {
		movingSumSecondOrder<float>();
		movingSumSecondOrder<double>();
	}

	TEST_F(FirFilterTests, movingSumThirdOrder) {
		movingSumThirdOrder<float>();
		movingSumThirdOrder<double>();
	}

	TEST_F(FirFilterTests, zeroIRWithSuccessiveCalls) {
		zeroIRWithSuccessiveCalls<float>();
		zeroIRWithSuccessiveCalls<double>();
	}

	TEST_F(FirFilterTests, identityFilterWithSuccessiveCalls) {
		identityFilterWithSuccessiveCalls<float>();
		identityFilterWithSuccessiveCalls<double>();
	}

	TEST_F(FirFilterTests, movingSumWithSuccessiveCalls) {
		movingSumWithSuccessiveCalls<float>();
		movingSumWithSuccessiveCalls<double>();
	}

	TEST_F(FirFilterTests, movingSumWithIncreasingInputSize) {
		movingSumWithIncreasingInputSize<float>();
		movingSumWithIncreasingInputSize<double>();
	}

	TEST_F(FirFilterTests, movingSumWithDecreasingInputSize) {
		movingSumWithDecreasingInputSize<float>();
		movingSumWithDecreasingInputSize<double>();
	}

	TEST_F(FirFilterTests, movingSumWithChangingInputSize) {
		movingSumWithChangingInputSize<float>();
		movingSumWithChangingInputSize<double>();
	}

	TEST_F(
		FirFilterTests,
		movingSumWithInputSizeGoingAboveAndBelowCoefficientLength
	) {
		movingSumWithInputSizeGoingAboveAndBelowCoefficientLength<float>();
		movingSumWithInputSizeGoingAboveAndBelowCoefficientLength<double>();
	}

	TEST_F(FirFilterTests, secondOrderMovingSumWithSuccessiveCalls) {
		secondOrderMovingSumWithSuccessiveCalls<float>();
		secondOrderMovingSumWithSuccessiveCalls<double>();
	}

	TEST_F(FirFilterTests, secondOrderMovingSumOneCall) {
		secondOrderMovingSumOneCall<float>();
		secondOrderMovingSumOneCall<double>();
	}

	TEST_F(FirFilterTests, delayedIdentity) {
		delayedIdentity<float>();
		delayedIdentity<double>();
	}

	TEST_F(FirFilterTests, positiveCoefficients) {
		positiveCoefficients<float>();
		positiveCoefficients<double>();
	}

	TEST_F(FirFilterTests, negativeCoefficients) {
		negativeCoefficients<float>();
		negativeCoefficients<double>();
	}

	TEST_F(FirFilterTests, positiveAndNegativeCoefficients) {
		positiveAndNegativeCoefficients<float>();
		positiveAndNegativeCoefficients<double>();
	}
}