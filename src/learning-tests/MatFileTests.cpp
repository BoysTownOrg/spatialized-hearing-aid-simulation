#include <mat.h>
#include <gtest/gtest.h>

class MatlabArray {
	mxArray *data;
public:
	explicit MatlabArray(mxArray *data) :
		data{ data } {}
	const mxArray *get() const {
		return data;
	}
	~MatlabArray() {
		mxDestroyArray(data);
	}
};

class MatFileReader {
	MATFile *file;
public:
	explicit MatFileReader(std::string filePath) :
		file{ matOpen(filePath.c_str(), "r") } {}
	~MatFileReader() {
		matClose(file);
	}
	MatlabArray getVariable(std::string name) {
		return MatlabArray{ matGetVariable(file, name.c_str()) };
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

TEST(MatFileTestCase, getVariable) {
	MatFileReader reader{ "../example.mat" };
	const auto s = reader.getVariable("s");
	EXPECT_FALSE(s.get() == nullptr);
}
