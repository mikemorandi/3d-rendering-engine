
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "FirstPersonCameraAdapter.h"
#include "Camera.h"

using glm::mat4;
using glm::vec3;

FirstPersonCameraAdapter::FirstPersonCameraAdapter(const Camera_ptr& cam)
	: CameraAdapter(cam)
{
}


FirstPersonCameraAdapter::~FirstPersonCameraAdapter(void)
{
}

void FirstPersonCameraAdapter::OnKey(const Input::Key key, [[maybe_unused]] const Input::Modifier mod)
{
	//std::cout << "Key " << key << " pressed" << std::endl;

	switch(key)
	{
		case Input::Key::ARROW_UP: Walk(0.2f); break;
		case Input::Key::ARROW_DOWN: Walk(-0.2f);  break;
		case Input::Key::ARROW_LEFT: StepSidewards(-0.2f);  break;
		case Input::Key::ARROW_RIGHT: StepSidewards(0.2f);  break;
		default: break;  // Handle other keys
	}
}

void FirstPersonCameraAdapter::Walk([[maybe_unused]] float amount)
{
	// TODO: implement walking
	//vec3 dir = glm::normalize( cam->Target() - cam->Position() );
	//cam->SetPosition( cam->Position() + dir * amount );
}

void FirstPersonCameraAdapter::StepSidewards(float amount)
{
	auto& frustum = cam->CameraFrustum();
	vec3 step = frustum.frame.Side() * amount;
	frustum.position += step;
	cam->UpdateViewMatrix();
}

void FirstPersonCameraAdapter::Turn([[maybe_unused]] float degrees)
{
	// TODO: implement turning
}

void FirstPersonCameraAdapter::OnMouseMove(const glm::vec2& position)
{
	const float degreesPerPixelYaw = 0.5f;
	const float degreesPerPixelPitch = 0.2f;

	auto& frustum = cam->CameraFrustum();
	vec3 target;// = cam->Target();

	glm::vec2 screenDelta = position - lastScreenPos;
	float yaw = -screenDelta.x * degreesPerPixelYaw;
	float pitch = -screenDelta.y * degreesPerPixelPitch;

	//Transformation: translate to origin, rotate about axis, translate back
	glm::mat4 transformM = glm::translate(glm::mat4(1.0f),target);
	transformM = glm::rotate(transformM, glm::radians(pitch) , frustum.frame.Side());
	transformM = glm::rotate(transformM, glm::radians(yaw), frustum.frame.Up());
	transformM = glm::translate(transformM,-target);

	//Transform camera position
	// TODO: Apply transformation to camera
	// glm::vec4 newDir = transformM * glm::vec4(frustum.frame.ViewDir(), 0.0f);
	// glm::vec4 newUp = transformM * glm::vec4(frustum.frame.Up(), 0.0f);
	lastScreenPos = position;
}

void FirstPersonCameraAdapter::OnMouseDrag(const glm::vec2& screenPos)
{	
	lastScreenPos = screenPos;
}

void FirstPersonCameraAdapter::OnMouseClick([[maybe_unused]] Input::MouseButton button, [[maybe_unused]] Input::Direction state , const glm::vec2& screenPos)
{
	lastScreenPos = screenPos;
}

void FirstPersonCameraAdapter::OnMouseWheel([[maybe_unused]] Input::Direction direction, [[maybe_unused]] const glm::vec2& screenPos)
{
}
