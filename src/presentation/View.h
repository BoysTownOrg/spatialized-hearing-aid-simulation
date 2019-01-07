#pragma once

#include <common-includes/Interface.h>
#include <string>
#include <vector>

class EventListener {
public:
	INTERFACE_OPERATIONS(EventListener);
	virtual void browseForLeftDslPrescription() = 0;
	virtual void browseForRightDslPrescription() = 0;
	virtual void browseForAudio() = 0;
	virtual void browseForBrir() = 0;
	virtual void play() = 0;
};

class View {
public:
	INTERFACE_OPERATIONS(View);
	virtual void subscribe(EventListener *listener) = 0;
	virtual void runEventLoop() = 0;
	virtual std::string browseForFile(std::vector<std::string> filters) = 0;
	virtual std::string browseForDirectory() = 0;
	virtual bool browseCancelled() = 0;
	virtual void setLeftDslPrescriptionFilePath(std::string) = 0;
	virtual void setRightDslPrescriptionFilePath(std::string) = 0;
	virtual void setAudioDirectory(std::string) = 0;
	virtual void setBrirFilePath(std::string) = 0;
	virtual std::string leftDslPrescriptionFilePath() const = 0;
	virtual std::string rightDslPrescriptionFilePath() const = 0;
	virtual std::string audioDirectory() const = 0;
	virtual std::string brirFilePath() const = 0;
	virtual std::string audioDevice() const = 0;
	virtual std::string level_dB_Spl() const = 0;
	virtual std::string attack_ms() const = 0;
	virtual std::string release_ms() const = 0;
	virtual std::string windowSize() const = 0;
	virtual std::string chunkSize() const = 0;
	virtual void showErrorDialog(std::string message) = 0;
	virtual void populateAudioDeviceMenu(std::vector<std::string> items) = 0;
};
