#include <mat.h>
#include <gtest/gtest.h>

class MatFileReader {
	MATFile *file;
public:
	explicit MatFileReader(std::string filePath) :
		file{ matOpen(filePath.c_str(), "r") } {}
	~MatFileReader() {
		matClose(file);
	}
	mxArray *getVariable(std::string name) {
		return matGetVariable(file, name.c_str());
	}
	const MATFile *get() const {
		return file;
	}
};

class MatFileTestCase : public ::testing::TestCase {};

TEST(MatFileTestCase, canOpen) {
	MatFileReader reader{ "../example.mat" };
	EXPECT_FALSE(reader.get() == nullptr);
}

TEST(MatFileTestCase, tbd) {
	MatFileReader reader{ "../example.mat" };
	const auto s = reader.getVariable("s");
	EXPECT_FALSE(s == nullptr);
}
