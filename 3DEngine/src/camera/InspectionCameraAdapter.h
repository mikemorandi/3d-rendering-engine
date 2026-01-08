#pragma once

#include "../input/MouseObserver.h"
#include "CameraAdapter.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Camera;
/**
* \class InspectionCameraAdapter
* \brief Mouse adapter class that rotates around a reference point
* \author Michael Morandi
*
* A mouse adapter class that rotates around a reference point by holding down
* the left mouse button and dragging. The camera then rotates around its target point
* holding down the right mouse button pans rhe view
*/
class InspectionCameraAdapter : public CameraAdapter, public MouseObserver
{
public:
	InspectionCameraAdapter(const CameraPtr& cam);
	virtual ~InspectionCameraAdapter();

	virtual void OnMouseMove(const glm::vec2& position) override;
	virtual void OnMouseDrag(const glm::vec2& position) override;
	virtual void OnMouseClick(Input::MouseButton button, Input::Direction state, const glm::vec2& position) override;
	virtual void OnMouseWheel(Input::Direction direction, const glm::vec2& position) override;

protected:

	glm::vec2				lastScreenPos;
	glm::vec3				orbitCenter;
	Input::MouseButton		lastButton;
	Input::Direction		lastClickDirection;

};

