#pragma once

#include <memory>

class ViewportObserver;
using ViewportObserverPtr = std::shared_ptr<ViewportObserver>;
using ViewportObserverConstPtr = std::shared_ptr<const ViewportObserver>;
using ViewportObserverWeakPtr = std::weak_ptr<ViewportObserver>;
class Viewport;
using ViewportPtr = std::shared_ptr<Viewport>;
using ViewportConstPtr = std::shared_ptr<const Viewport>;
using ViewportWeakPtr = std::weak_ptr<Viewport>;

class ViewportObserver
{
public:
	virtual void ViewportChanged(const ViewportPtr& viewport) = 0;

};