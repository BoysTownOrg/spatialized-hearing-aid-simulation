#include <signal-processing/SignalProcessor.h>
#include <spatialized-hearing-aid-simulation/RefactoredModel.h>

class RefactoredSpatializedHearingAidSimulationFactory {
public:
	RefactoredSpatializedHearingAidSimulationFactory(ScalarFactory *) {}

	struct SimulationParameters {
		float scale;
	};
	std::shared_ptr<SignalProcessor> make(SimulationParameters) {
		return {};
	}
};

#include <gtest/gtest.h>

namespace {
	class ScalarFactoryStub : public ScalarFactory {
		float scalar_{};
		std::shared_ptr<SignalProcessor> processor{};
	public:
		void setProcessor(std::shared_ptr<SignalProcessor> p) noexcept {
			processor = std::move(p);
		}

		auto scalar() const {
			return scalar_;
		}

		std::shared_ptr<SignalProcessor> make(float x) override {
			scalar_ = x;
			return processor;
		}
	};

	class RefactoredSpatializedHearingAidSimulationFactoryTests : public ::testing::Test {
	protected:
		ScalarFactoryStub scalarFactory{};
		RefactoredSpatializedHearingAidSimulationFactory simulationFactory{ &scalarFactory };
	};

	TEST_F(RefactoredSpatializedHearingAidSimulationFactoryTests, makePassesScalarToFactory) {
		RefactoredSpatializedHearingAidSimulationFactory::SimulationParameters p;
		p.scale = 1;
		simulationFactory.make(p);
		EXPECT_EQ(1, scalarFactory.scalar());
	}
}