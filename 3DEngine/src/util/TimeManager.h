#pragma once

#include <vector>
#include <memory>

class TimeObserver;
using TimeObserverPtr = std::shared_ptr<TimeObserver>;
using TimeObserverConstPtr = std::shared_ptr<const TimeObserver>;
using TimeObserverWeakPtr = std::weak_ptr<TimeObserver>;

class TimeManager
{
public:

	static TimeManager& Instance();
	static void Tick(double time);

	virtual void AddTimeObserver(const TimeObserverPtr& observer);

	virtual void RemoveTimeObserver(const TimeObserverPtr& observer);

protected:

	void HandleTick(double time);

	std::vector<TimeObserverWeakPtr> timeObservers;
	static TimeManager instance;

private:
	TimeManager();

};

