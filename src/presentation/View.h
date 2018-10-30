#pragma once

#include <common-includes/Interface.h>
#include <string>
#include <vector>

class Presenter;

class View {
public:
	INTERFACE_OPERATIONS(View);
	virtual void setPresenter(Presenter *) = 0;
	virtual void runEventLoop() = 0;
	virtual std::string browseForFile(std::vector<std::string> filters) = 0;
	virtual bool browseCancelled() = 0;
	virtual void setLeftDslPrescriptionFilePath(std::string) = 0;
	virtual void setRightDslPrescriptionFilePath(std::string) = 0;
	virtual void setAudioFilePath(std::string) = 0;
	virtual void setBrirFilePath(std::string) = 0;
	virtual std::string leftDslPrescriptionFilePath() const = 0;
	virtual std::string rightDslPrescriptionFilePath() const = 0;
	virtual std::string audioFilePath() const = 0;
	virtual std::string brirFilePath() const = 0;
	virtual std::string level_dB_Spl() const = 0;
	virtual std::string attack_ms() const = 0;
	virtual std::string release_ms() const = 0;
	virtual std::string windowSize() const = 0;
	virtual std::string chunkSize() const = 0;
	virtual void showErrorDialog(std::string message) = 0;
};
