#include <audio-file-reading/AudioFileReader.h>
#include <dsl-prescription/ConfigurationFileParser.h>
#include <binaural-room-impulse-response/BinauralRoomImpulseResponse.h>
#include <gsl/gsl>

class BrirAdapter {
	std::shared_ptr<AudioFileReaderFactory> factory;
public:
	explicit BrirAdapter(std::shared_ptr<AudioFileReaderFactory> factory) :
		factory{ std::move(factory) }
	{
	}

	BinauralRoomImpulseResponse read(std::string filePath) {
		BinauralRoomImpulseResponse brir{};
		const auto reader = factory->make(filePath);
		using vector_type = decltype(brir.left);
		vector_type buffer(gsl::narrow<vector_type::size_type>(reader->frames() * reader->channels()));
		reader->readFrames(&buffer[0], buffer.size());
		bool oddSample = false;
		std::partition_copy(
			buffer.begin(),
			buffer.end(),
			std::back_inserter(brir.left),
			std::back_inserter(brir.right),
			[&oddSample](double) { return oddSample = !oddSample; });
		brir.sampleRate = reader->sampleRate();
		return brir;
	}
};

#include "assert-utility.h"
#include "FakeAudioFileReader.h"
#include <binaural-room-impulse-response/BinauralRoomImpulseResponse.h>
#include <gtest/gtest.h>

class BrirAdapterTestCase : public ::testing::TestCase {};

TEST(BrirAdapterTestCase, interpretsAudioFileAsBrir) {
	const auto reader = std::make_shared<FakeAudioFileReader>( std::vector<float>{ 1, 2, 3, 4 } );
	reader->setChannels(2);
	reader->setSampleRate(5);
	BrirAdapter adapter{ std::make_shared<FakeAudioFileReaderFactory>(reader) };
	const auto brir = adapter.read("");
	EXPECT_EQ(5, brir.sampleRate);
	assertEqual({ 1, 3 }, brir.left);
	assertEqual({ 2, 4 }, brir.right);
}