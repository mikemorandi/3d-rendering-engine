
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "FirstPersonCameraAdapter.h"
#include "Camera.h"

using glm::mat4;
using glm::vec3;

FirstPersonCameraAdapter::FirstPersonCameraAdapter(const CameraPtr& cam)
	: CameraAdapter(cam)
	, firstMouseMove(true)
	, movingForward(false)
	, movingBackward(false)
	, movingLeft(false)
	, movingRight(false)
	, moveSpeed(2.0f)
{
}


FirstPersonCameraAdapter::~FirstPersonCameraAdapter(void)
{
}

void FirstPersonCameraAdapter::OnKey(const Input::Key key, [[maybe_unused]] const Input::Modifier mod)
{
	// OnKey is still called for single key presses, but continuous movement
	// is now handled by OnKeyStateChange and TimeUpdate
	(void)key;
}

void FirstPersonCameraAdapter::OnKeyStateChange(const Input::Key key, const Input::KeyState state)
{
	bool pressed = (state == Input::KeyState::PRESSED);

	switch(key)
	{
		case Input::Key::ARROW_UP:
		case Input::Key::W:
			movingForward = pressed;
			break;
		case Input::Key::ARROW_DOWN:
		case Input::Key::S:
			movingBackward = pressed;
			break;
		case Input::Key::ARROW_LEFT:
		case Input::Key::A:
			movingLeft = pressed;
			break;
		case Input::Key::ARROW_RIGHT:
		case Input::Key::D:
			movingRight = pressed;
			break;
		default:
			break;
	}
}

void FirstPersonCameraAdapter::TimeUpdate(double time)
{
	static double lastTime = time;
	double deltaTime = time - lastTime;
	lastTime = time;

	// Clamp delta time to prevent huge jumps
	if (deltaTime > 0.1)
		deltaTime = 0.1;

	float moveAmount = moveSpeed * static_cast<float>(deltaTime);

	bool moved = false;

	if (movingForward)
	{
		Walk(moveAmount);
		moved = true;
	}
	if (movingBackward)
	{
		Walk(-moveAmount);
		moved = true;
	}
	if (movingLeft)
	{
		StepSidewards(-moveAmount);
		moved = true;
	}
	if (movingRight)
	{
		StepSidewards(moveAmount);
		moved = true;
	}

	// Only update view matrix once if we moved
	if (moved)
	{
		// Walk and StepSidewards already call UpdateViewMatrix, but we're being
		// inefficient. For now, this works. Could optimize later.
	}
}

void FirstPersonCameraAdapter::Walk(float amount)
{
	auto& frustum = cam->CameraFrustum();
	vec3 step = frustum.frame.ViewDir() * amount;
	frustum.position += step;
	cam->UpdateViewMatrix();
}

void FirstPersonCameraAdapter::StepSidewards(float amount)
{
	auto& frustum = cam->CameraFrustum();
	vec3 step = frustum.frame.Side() * amount;
	frustum.position += step;
	cam->UpdateViewMatrix();
}

void FirstPersonCameraAdapter::Turn(float degrees)
{
	auto& frustum = cam->CameraFrustum();

	// Rotate the view direction and side vectors around the up axis
	mat4 rotationMatrix = glm::rotate(mat4(1.0f), glm::radians(degrees), frustum.frame.Up());

	vec3 newViewDir = vec3(rotationMatrix * glm::vec4(frustum.frame.ViewDir(), 0.0f));
	vec3 newSide = vec3(rotationMatrix * glm::vec4(frustum.frame.Side(), 0.0f));

	frustum.frame.ViewDir() = glm::normalize(newViewDir);
	frustum.frame.Side() = glm::normalize(newSide);

	cam->UpdateViewMatrix();
}

void FirstPersonCameraAdapter::OnMouseMove(const glm::vec2& position)
{
	// Skip delta calculation on first mouse move to prevent jump
	if (firstMouseMove)
	{
		lastScreenPos = position;
		firstMouseMove = false;
		return;
	}

	const float degreesPerPixelYaw = 0.5f;
	const float degreesPerPixelPitch = 0.2f;

	auto& frustum = cam->CameraFrustum();

	glm::vec2 screenDelta = position - lastScreenPos;
	float yaw = -screenDelta.x * degreesPerPixelYaw;
	float pitch = -screenDelta.y * degreesPerPixelPitch;

	// First-person camera rotates around the viewer's position
	// Apply yaw rotation around the up axis
	glm::mat4 yawRotation = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), frustum.frame.Up());
	vec3 newViewDir = glm::vec3(yawRotation * glm::vec4(frustum.frame.ViewDir(), 0.0f));
	vec3 newSide = glm::vec3(yawRotation * glm::vec4(frustum.frame.Side(), 0.0f));

	// Apply pitch rotation around the side axis
	glm::mat4 pitchRotation = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), newSide);
	newViewDir = glm::vec3(pitchRotation * glm::vec4(newViewDir, 0.0f));
	vec3 newUp = glm::vec3(pitchRotation * glm::vec4(frustum.frame.Up(), 0.0f));

	// Update the frustum frame with normalized vectors
	frustum.frame.ViewDir() = glm::normalize(newViewDir);
	frustum.frame.Up() = glm::normalize(newUp);
	frustum.frame.Side() = glm::normalize(newSide);

	cam->UpdateViewMatrix();
	lastScreenPos = position;
}

void FirstPersonCameraAdapter::OnMouseDrag(const glm::vec2& screenPos)
{
	lastScreenPos = screenPos;
	firstMouseMove = false;
}

void FirstPersonCameraAdapter::OnMouseClick([[maybe_unused]] Input::MouseButton button, [[maybe_unused]] Input::Direction state , const glm::vec2& screenPos)
{
	lastScreenPos = screenPos;
	firstMouseMove = false;
}

void FirstPersonCameraAdapter::OnMouseWheel([[maybe_unused]] Input::Direction direction, [[maybe_unused]] const glm::vec2& screenPos)
{
}
