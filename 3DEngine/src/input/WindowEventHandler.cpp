#include "WindowEventHandler.h"
#include "ViewportObserver.h"
#include "../rendering/Viewport.h"


WindowEventHandler WindowEventHandler::instance = WindowEventHandler();

WindowEventHandler::WindowEventHandler()
{
}


WindowEventHandler& WindowEventHandler::Instance()
{
	return instance;
}

void WindowEventHandler::AddViewportObserver(const ViewportObserverWeakPtr& observer)
{
	viewportObservers.push_back(observer);
}

void WindowEventHandler::Resize(int width, int height)
{
	ViewportPtr vp = Viewport::Create(width, height);
	instance.OnViewportChanged(vp);
}

void WindowEventHandler::ViewportChanged(ViewportPtr& viewport)
{
	instance.OnViewportChanged(viewport);
}

void WindowEventHandler::OnViewportChanged(ViewportPtr& viewport)
{
	viewport->Apply();

	//Notify observers
	for (ViewportObserverWeakPtr& vpo : viewportObservers)
	{
		if(auto vpObserver = vpo.lock()) 
			vpObserver->ViewportChanged(viewport);
	}
}