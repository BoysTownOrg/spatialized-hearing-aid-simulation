#pragma once

#include "AudioFrameReader.h"

class CalibrationComputer {
	using sample_type = AudioFrameReader::channel_type::element_type;
	using channel_type = std::vector<sample_type>;
    std::vector<channel_type> audioFileContents;
public:
	explicit CalibrationComputer(AudioFrameReader &reader) :
		audioFileContents(
			reader.channels(), 
			channel_type(gsl::narrow<channel_type::size_type>(reader.frames()))
		)
	{
		read(reader);
	}

	double signalScale(int channel, double level) {
		return std::pow(10.0, level / 20.0) / rms(audioFileContents.at(channel));
	}

private:
	void read(AudioFrameReader & reader) {
		std::vector<AudioFrameReader::channel_type> adapted;
		for (auto &channel : audioFileContents)
			adapted.push_back({ channel });
		reader.read(adapted);
		reader.reset();
	}

	template<typename T>
	T rms(std::vector<T> x) {
		return std::sqrt(
			std::accumulate(
				x.begin(),
				x.end(),
				T{ 0 },
				[](T a, T b) { return a += b * b; }
			) / x.size()
		);
	}
};

