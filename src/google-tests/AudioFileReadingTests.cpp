#include <gtest/gtest.h>

class OneSampleAtATimeAudioFileReader {
	std::vector<double> input;
	std::size_t readIndex{};
public:
	explicit OneSampleAtATimeAudioFileReader(std::vector<double> input) :
		input(std::move(input)) {}
	bool empty() {
		return readIndex == input.size();
	}
	long long read(double *x, int) {
		if (empty() || x == nullptr)
			return 0;
		*x = input[readIndex++];
		return 1;
	}
	int format() {
		return {};
	}
	int sampleRate() {
		return {};
	}
	bool fail() {
		return {};
	}
	std::string errorMessage() {
		return {};
	}
	int channels() {
		return {};
	}
	double readSample() {
		double x;
		read(&x, 0);
		return x;
	}
};

class OneSampleAtATimeAudioFileReaderTestCase : public ::testing::TestCase {};

TEST(OneSampleAtATimeAudioFileReaderTestCase, readSamplesTillEmpty) {
	OneSampleAtATimeAudioFileReader reader{ { 1, 2, 3 } };
	EXPECT_FALSE(reader.empty());
	reader.readSample();
	EXPECT_FALSE(reader.empty());
	reader.readSample();
	EXPECT_FALSE(reader.empty());
	reader.readSample();
	EXPECT_TRUE(reader.empty());
}

TEST(OneSampleAtATimeAudioFileReaderTestCase, readEachSample) {
	OneSampleAtATimeAudioFileReader reader{ { 1, 2, 3 } };
	EXPECT_EQ(1, reader.readSample());
	EXPECT_EQ(2, reader.readSample());
	EXPECT_EQ(3, reader.readSample());
}

TEST(OneSampleAtATimeAudioFileReaderTestCase, readReturnsSamplesRead) {
	OneSampleAtATimeAudioFileReader reader{ { 1, 2, 3 } };
	double x{};
	EXPECT_EQ(1, reader.read(&x, 0));
	EXPECT_EQ(1, reader.read(&x, 0));
	EXPECT_EQ(1, reader.read(&x, 0));
	EXPECT_EQ(0, reader.read(&x, 0));
}

TEST(OneSampleAtATimeAudioFileReaderTestCase, readReturnsZeroWhenNull) {
	OneSampleAtATimeAudioFileReader reader{ { 1, 2, 3 } };
	EXPECT_EQ(0, reader.read(nullptr, 0));
}

class AudioFileReadingTestCase : public ::testing::TestCase {};

/*
TEST(AudioFileReadingTestCase, tbd) {
	const auto reader = std::make_shared<MockAudioFileReader>();
	InMemoryAudioFile audioFile{ reader };
}*/