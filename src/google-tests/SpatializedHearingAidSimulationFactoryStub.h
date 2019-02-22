#pragma once

#include "ArgumentCollection.h"
#include <spatialized-hearing-aid-simulation/ISpatializedHearingAidSimulationFactory.h>
#include <vector>

template<typename T>
class PoppableVector {
	std::vector<T> elements;
public:
	PoppableVector(typename std::vector<T>::size_type count) : elements(count) {}

	void set(std::vector<T> v) noexcept {
		elements = std::move(v);
	}

	T pop_front() {
		if (elements.empty())
			return {};
		auto item = elements.front();
		elements.erase(elements.begin());
		return item;
	}
};
class SpatializedHearingAidSimulationFactoryStub : 
	public ISpatializedHearingAidSimulationFactory 
{
	ArgumentCollection<HearingAidSimulation> fullSimulationHearingAid_{};
	ArgumentCollection<HearingAidSimulation> hearingAidSimulation_{};
	ArgumentCollection<Spatialization> fullSimulationSpatialization_{};
	ArgumentCollection<Spatialization> spatialization_{};
	ArgumentCollection<float> fullSimulationScale_{};
	ArgumentCollection<float> hearingAidSimulationScale_{};
	ArgumentCollection<float> spatializationScale_{};
	ArgumentCollection<float> withoutSimulationScale_{};
public:
	PoppableVector<std::shared_ptr<SignalProcessor>> fullSimulationProcessors;
	PoppableVector<std::shared_ptr<SignalProcessor>> hearingAidSimulationProcessors;
	PoppableVector<std::shared_ptr<SignalProcessor>> spatializationProcessors;
	PoppableVector<std::shared_ptr<SignalProcessor>> withoutSimulationProcessors;

	SpatializedHearingAidSimulationFactoryStub() :
		fullSimulationProcessors(2),
		hearingAidSimulationProcessors(2),
		spatializationProcessors(2),
		withoutSimulationProcessors(2) {}

	void setFullSimulationProcessors(std::vector<std::shared_ptr<SignalProcessor>> p) noexcept {
		fullSimulationProcessors.set(std::move(p));
	}

	void setHearingAidSimulationProcessors(std::vector<std::shared_ptr<SignalProcessor>> p) noexcept {
		hearingAidSimulationProcessors.set(std::move(p));
	}

	void setSpatializationProcessors(std::vector<std::shared_ptr<SignalProcessor>> p) noexcept {
		spatializationProcessors.set(std::move(p));
	}

	void setWithoutSimulationProcessors(std::vector<std::shared_ptr<SignalProcessor>> p) noexcept {
		withoutSimulationProcessors.set(std::move(p));
	}

	std::shared_ptr<SignalProcessor> makeFullSimulation(
		FullSimulation s, float x
	) override {
		fullSimulationHearingAid_.push_back(std::move(s.hearingAid));
		fullSimulationSpatialization_.push_back(std::move(s.spatialization));
		fullSimulationScale_.push_back(x);
		return fullSimulationProcessors.pop_front();
	}

	std::shared_ptr<SignalProcessor> makeHearingAidSimulation(
		HearingAidSimulation s, float x
	) override {
		hearingAidSimulation_.push_back(std::move(s));
		hearingAidSimulationScale_.push_back(x);
		return hearingAidSimulationProcessors.pop_front();
	}

	std::shared_ptr<SignalProcessor> makeSpatialization(
		Spatialization s, float x
	) override {
		spatialization_.push_back(std::move(s));
		spatializationScale_.push_back(x);
		return spatializationProcessors.pop_front();
	}

	std::shared_ptr<SignalProcessor> makeWithoutSimulation(
		float x
	) override {
		withoutSimulationScale_.push_back(x);
		return withoutSimulationProcessors.pop_front();
	}

	auto &fullSimulationSpatialization() const noexcept {
		return fullSimulationSpatialization_;
	}
		
	auto &fullSimulationHearingAid() const noexcept {
		return fullSimulationHearingAid_;
	}

	auto &hearingAidSimulation() const noexcept {
		return hearingAidSimulation_;
	}
	auto &spatialization() const noexcept {
		return spatialization_;
	}

	auto &fullSimulationScale() const noexcept {
		return fullSimulationScale_;
	}

	auto &hearingAidSimulationScale() const noexcept {
		return hearingAidSimulationScale_;
	}

	auto &spatializationScale() const noexcept {
		return spatializationScale_;
	}

	auto &withoutSimulationScale() const noexcept {
		return withoutSimulationScale_;
	}
};