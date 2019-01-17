#include "AudioProcessorImpl.h"

void AudioProcessorImpl::initialize(Initialization initialization) {
	processing.attack_ms = initialization.attack_ms;
	processing.release_ms = initialization.release_ms;
	processing.brirFilePath = initialization.brirFilePath;
	processing.leftDslPrescriptionFilePath = initialization.leftDslPrescriptionFilePath;
	processing.rightDslPrescriptionFilePath = initialization.rightDslPrescriptionFilePath;
	processing.chunkSize = initialization.chunkSize;
	processing.windowSize = initialization.windowSize;
	processing.max_dB_Spl = initialization.max_dB_Spl;
	processing.channelScalars.resize(2);
	try {
		processorFactory->make(processing);
	}
	catch (const RefactoredAudioFrameProcessorFactory::CreateError &e) {
		throw InitializationFailure{ e.what() };
	}
	processing.channelScalars.clear();
}

void AudioProcessorImpl::prepare(Preparation p) {
	reader = makeReader(p.audioFilePath);
	const auto desiredRms = std::pow(10.0, (p.level_dB_Spl - processing.max_dB_Spl) / 20.0);
	RmsComputer rms{ *reader };
	for (int i = 0; i < reader->channels(); ++i)
		processing.channelScalars.push_back(desiredRms / rms.compute(i));
	processor = makeProcessor(processing);
	reader->reset();
}
