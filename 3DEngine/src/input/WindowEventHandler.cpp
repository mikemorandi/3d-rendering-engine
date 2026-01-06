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

void WindowEventHandler::AddViewportObserver(const ViewportObserver_wptr& observer)
{
	viewportObservers.push_back(observer);
}

void WindowEventHandler::Resize(int width, int height)
{
	Viewport_ptr vp = Viewport::Create(width, height);
	instance.OnViewportChanged(vp);
}

void WindowEventHandler::ViewportChanged(Viewport_ptr& viewport)
{
	instance.OnViewportChanged(viewport);
}

void WindowEventHandler::OnViewportChanged(Viewport_ptr& viewport)
{
	viewport->Apply();

	//Notify observers
	for (ViewportObserver_wptr& vpo : viewportObservers)
	{
		if(auto vpObserver = vpo.lock()) 
			vpObserver->ViewportChanged(viewport);
	}
}