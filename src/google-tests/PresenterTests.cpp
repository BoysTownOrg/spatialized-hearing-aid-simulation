#include <gtest/gtest.h>

class PresenterTestCase : public ::testing::TestCase {};

TEST(PresenterTestCase, constructorSetsItself) {
	const auto view = std::make_shared<MockView>();
	Presenter presenter{ std::make_shared<MockModel>(), view };
	EXPECT_EQ(&presenter, view->presenter());
}