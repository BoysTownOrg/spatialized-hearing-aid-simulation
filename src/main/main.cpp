#include "Chapro.h"
#include "FltkView.h"
#include "LibsndfileReader.h"
#include "PortAudioDevice.h"
#include "NlohmannJsonParser.h"
#include "SpatializedHearingAidSimulationFactory.h"
#include <audio-stream-processing/ChannelCopier.h>
#include <audio-file-reading/AudioFileInMemory.h>
#include <binaural-room-impulse-response/BrirAdapter.h>
#include <dsl-prescription/PrescriptionAdapter.h>
#include <presentation/Presenter.h>
#include <playing-audio/AudioPlayer.h>
#include <playing-audio/AudioProcessingLoader.h>
#include <recognition-test/RecognitionTestModel.h>
#include <stimulus-list/RandomizedStimulusList.h>
#include <stimulus-list/FileFilterDecorator.h>

#include <Windows.h>

class WindowsFileSearch {
	HANDLE hFind;
public:
	WindowsFileSearch(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) :
		hFind{FindFirstFileA(lpFileName, lpFindFileData)}
	{
	}

	bool FindNextFileA(LPWIN32_FIND_DATAA lpFindFileData) {
		return ::FindNextFileA(hFind, lpFindFileData);
	}

	~WindowsFileSearch() noexcept {
		FindClose(hFind);
	}

	WindowsFileSearch(WindowsFileSearch &&) = delete;
	WindowsFileSearch &operator=(WindowsFileSearch &&) = delete;
	WindowsFileSearch(const WindowsFileSearch &) = delete;
	WindowsFileSearch &operator=(const WindowsFileSearch &) = delete;
};

class WindowsDirectoryReader : public DirectoryReader {
	// https://docs.microsoft.com/en-us/windows/desktop/fileio/listing-the-files-in-a-directory
	std::vector<std::string> filesIn(std::string directory) override {
		directory += "\\*";
		WIN32_FIND_DATA ffd;
		WindowsFileSearch search{ directory.c_str(), &ffd };
		std::vector<std::string> files{};
		do {
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			else
				files.push_back(ffd.cFileName);
		} while (search.FindNextFileA(&ffd) != 0);
		return files;
	}
};

#include <random>

class MersenneTwisterRandomizer : public Randomizer {
    std::mt19937 engine{std::random_device{}()};
public:
    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        std::shuffle(begin, end, engine);
    }
};

#include <fstream>

class FileSystemDocumenter : public Documenter {
	std::ofstream file;
public:
	void initialize(std::string filePath) override {
		file.open(std::move(filePath));
	}

	void write(std::string s) override {
		file << std::move(s);
	}

	bool failed() override {
		return file.fail();
	}

	std::string errorMessage() override {
		char buffer[256];
		strerror_s(buffer, sizeof buffer, errno);
		return buffer;
	}

	void documentTestParameters(TestParameters) override
	{
	}
};

int main() {
	WindowsDirectoryReader reader{};
	FileFilterDecorator decorator{&reader, ".wav"};
	MersenneTwisterRandomizer randomizer{};
	RandomizedStimulusList list{&decorator, &randomizer};
	PortAudioDevice device{};
	ChannelCopierFactory frameReaderFactory{ 
		std::make_shared<AudioFileInMemoryFactory>(
			std::make_shared<LibsndfileReaderFactory>()
		) 
	};
	SpatializedHearingAidSimulationFactory processorFactory{
		std::make_shared<ChaproFactory>(),
		std::make_shared<PrescriptionAdapter>(
			std::make_shared<NlohmannJsonParserFactory>()
		),
		std::make_shared<BrirAdapter>(
			std::make_shared<LibsndfileReaderFactory>()
		)
	};
	
	AudioProcessingLoader loader{&frameReaderFactory, &processorFactory};
	AudioPlayer player{&device, &loader};
	FileSystemDocumenter documenter;
	RecognitionTestModel model{ &list, &player, &documenter };
	FltkView view{};
	Presenter presenter{ &model, &view };
	presenter.run();
}
