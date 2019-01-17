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
