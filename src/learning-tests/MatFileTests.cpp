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
	mxClassID classId() const {
		return mxGetClassID(data);
	}
	int fieldCount() const {
		return mxGetNumberOfFields(data);
	}
	const mxArray *getField(mwIndex index, std::string name) const {
		return mxGetField(data, index, name.c_str());
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
	EXPECT_EQ(mxSTRUCT_CLASS, s.classId());
	EXPECT_EQ(2, s.fieldCount());
	const auto a = s.getField(0, "a");
	EXPECT_FALSE(a == nullptr);
	const auto c = s.getField(0, "c");
	EXPECT_TRUE(c == nullptr);
}

TEST(MatFileTestCase, getPr) {
	MatFileReader reader{ "../example.mat" };
	const auto s = reader.getVariable("s");
	const auto b = s.getField(0, "b");
	const auto contents = mxGetPr(b);
	EXPECT_EQ(2, contents[0]);
	EXPECT_EQ(3, contents[1]);
}
