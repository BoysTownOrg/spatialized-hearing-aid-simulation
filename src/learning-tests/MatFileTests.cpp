#include <mat.h>
#include <gtest/gtest.h>

class MatFileTestCase : public ::testing::TestCase {};

TEST(MatFileTestCase, tbd) {
	const auto matFile = matOpen("../example.mat", "r");
	EXPECT_FALSE(matFile == nullptr);
}