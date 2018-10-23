#pragma once

#define INTERFACE_OPERATIONS(class_name) \
    virtual ~class_name() = default;\
    class_name() = default;\
    class_name(const class_name&) = delete;\
    class_name& operator=(const class_name&) = delete;\
    class_name(class_name&&) = delete;\
    class_name& operator=(class_name&&) = delete;

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
	virtual void setDslPrescriptionFilePath(std::string) = 0;
	virtual void setAudioFilePath(std::string) = 0;
	virtual void setBrirFilePath(std::string) = 0;
};
