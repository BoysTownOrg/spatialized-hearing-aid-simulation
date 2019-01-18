#pragma once

#define INTERFACE_OPERATIONS(class_name) \
    virtual ~class_name() = default;\
    class_name() = default;\
    class_name(const class_name&) = default;\
    class_name& operator=(const class_name&) = default;\
    class_name(class_name&&) = default;\
    class_name& operator=(class_name&&) = default;