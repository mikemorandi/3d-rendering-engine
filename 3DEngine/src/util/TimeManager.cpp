#include "TimeManager.h"
#include "../animation/TimeObserver.h"

#include <algorithm>

TimeManager TimeManager::instance = TimeManager();

TimeManager::TimeManager()
{
}

TimeManager& TimeManager::Instance()
{
	return TimeManager::instance;
}

void TimeManager::AddTimeObserver(const TimeObserver_ptr& observer)
{
	timeObservers.push_back(observer);
}

void TimeManager::RemoveTimeObserver(const TimeObserver_ptr& observer)
{
	auto item = std::find_if(timeObservers.begin(), timeObservers.end(), [&observer](TimeObserver_wptr p)
	{
		if (auto to = p.lock())
		{
			return to == observer;
		}
		return false;
	});
	if (item != timeObservers.end())
	{
		timeObservers.erase(item);
	}
}

void TimeManager::HandleTick(double time)
{
	for(auto& cit : timeObservers)
	{
		if(auto tObserver = cit.lock())
		{
			tObserver->TimeUpdate(time);
		}
	}
}

void TimeManager::Tick(double time)
{
	instance.HandleTick(time);
}