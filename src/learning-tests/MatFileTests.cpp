#include <mat.h>
#include <gtest/gtest.h>

class MatFileTestCase : public ::testing::TestCase {};

TEST(MatFileTestCase, canOpen) {
	const auto matFile = matOpen("../example.mat", "r");
	EXPECT_FALSE(matFile == nullptr);
}

TEST(MatFileTestCase, tbd) {
	const auto matFile = matOpen("../example.mat", "r");
	const auto s = matGetVariable(matFile, "s");
	EXPECT_FALSE(s == nullptr);
}
