#pragma once

#include <memory>

class TimeObserver;
using TimeObserverPtr = std::shared_ptr<TimeObserver>;
using TimeObserverConstPtr = std::shared_ptr<const TimeObserver>;
using TimeObserverWeakPtr = std::weak_ptr<TimeObserver>;

class TimeObserver
{
public:

	virtual void TimeUpdate(double time) = 0;
};