#pragma once

#include <glm/fwd.hpp>
#include "CameraAdapter.h"
#include "../input/MouseObserver.h"
#include "../input/KeyboardObserver.h"
#include "../animation/TimeObserver.h"

class KeyboardObserver;

class FirstPersonCameraAdapter : public CameraAdapter, public MouseObserver, public KeyboardObserver, public TimeObserver
{
public:
	FirstPersonCameraAdapter(const CameraPtr& cam);
	virtual ~FirstPersonCameraAdapter();

	virtual void OnMouseMove(const glm::vec2& position) override;
	virtual void OnMouseDrag(const glm::vec2& position) override;
	virtual void OnMouseClick(Input::MouseButton button, Input::Direction state, const  glm::vec2& position) override;
	virtual void OnMouseWheel(Input::Direction direction, const glm::vec2& position) override;
	virtual void OnKey(const Input::Key key, const Input::Modifier mod) override;
	virtual void OnKeyStateChange(const Input::Key key, const Input::KeyState state) override;

	virtual void TimeUpdate(double time) override;

protected:

	void Walk(float amount);
	void StepSidewards(float amount);
	void Turn(float degrees);

	glm::vec2 lastScreenPos;
	bool firstMouseMove;

	// Movement state for smooth continuous movement
	bool movingForward;
	bool movingBackward;
	bool movingLeft;
	bool movingRight;
	float moveSpeed; // units per second

};

