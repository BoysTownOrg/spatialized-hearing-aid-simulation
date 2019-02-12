#include "CalibrationComputer.h"

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

CalibrationComputer::CalibrationComputer(AudioFrameReader & reader) :
	audioFileContents(
		reader.channels(),
		channel_type(gsl::narrow<channel_type::size_type>(reader.frames()))
	)
{
	read(reader);
}

double CalibrationComputer::signalScale(int channel, double level) {
	return validChannel(channel)
		? std::pow(10.0, level / 20.0) / rms(audioFileContents.at(channel))
		: 0;
}

bool CalibrationComputer::validChannel(int channel) {
	return gsl::narrow<channel_type::size_type>(channel) < audioFileContents.size();
}

void CalibrationComputer::read(AudioFrameReader & reader) {
	std::vector<AudioFrameReader::channel_type> adapted;
	for (auto &channel : audioFileContents)
		adapted.push_back({ channel });
	reader.read(adapted);
	reader.reset();
}
