#pragma once

#define INTERFACE_OPERATIONS(class_name) \
    virtual ~class_name() = default;\
    class_name() = default;\
    class_name(const class_name&) = delete;\
    class_name& operator=(const class_name&) = delete;\
    class_name(class_name&&) = delete;\
    class_name& operator=(class_name&&) = delete;

class Presenter;

class View {
public:
	INTERFACE_OPERATIONS(View);
	virtual void setPresenter(Presenter *) = 0;
};
