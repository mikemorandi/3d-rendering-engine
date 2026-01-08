#pragma once

#include <vector>
#include <memory>

class ViewportObserver;
using ViewportObserverPtr = std::shared_ptr<ViewportObserver>;
using ViewportObserverConstPtr = std::shared_ptr<const ViewportObserver>;
using ViewportObserverWeakPtr = std::weak_ptr<ViewportObserver>;
class Viewport;
using ViewportPtr = std::shared_ptr<Viewport>;
using ViewportConstPtr = std::shared_ptr<const Viewport>;
using ViewportWeakPtr = std::weak_ptr<Viewport>;

class WindowEventHandler
{
public:
	static WindowEventHandler& Instance();
	static void Resize(int width, int height);
	static void ViewportChanged(ViewportPtr& viewport);

	void AddViewportObserver(const ViewportObserverWeakPtr& observer);

protected:

	WindowEventHandler();

	void OnViewportChanged(ViewportPtr& viewport);

	std::vector<ViewportObserverWeakPtr> viewportObservers;
	static WindowEventHandler instance;
};

