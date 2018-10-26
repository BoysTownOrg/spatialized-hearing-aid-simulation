#pragma once
#include <common-includes/Interface.h>

class MonoProcessor {
public:
	INTERFACE_OPERATIONS(MonoProcessor);
	virtual void process(float *, int) = 0;
};

#include <memory>

class StereoProcessor {
	std::shared_ptr<MonoProcessor> left;
	std::shared_ptr<MonoProcessor> right;
public:
	StereoProcessor(
		std::shared_ptr<MonoProcessor> left,
		std::shared_ptr<MonoProcessor> right
	) :
		left{ std::move(left) },
		right{ std::move(right) } {}

	void process(float *xLeft, float *xRight, int frameCount) {
		left->process(xLeft, frameCount);
		right->process(xRight, frameCount);
	}
};

