#include <gtest/gtest.h>

class RefactoredSpatializedHearingAidSimulationFactoryTests : public ::testing::Test {
protected:
};



TEST_F(RefactoredSpatializedHearingAidSimulationFactoryTests, makePassesScalarToFactory) {
	SimulationParameters p;
	p.scale = 1;
	simulationFactory.make(p);
	EXPECT_EQ(1, scalarFactory.scalars().at(0));
}