
#include "InspectionCameraAdapter.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>


#include "Camera.h"

using glm::vec3;
using glm::vec4;

InspectionCameraAdapter::InspectionCameraAdapter(const CameraPtr& cam) 
	: CameraAdapter(cam)
	, orbitCenter(glm::vec3(0,0,0))
{

}

InspectionCameraAdapter::~InspectionCameraAdapter(void)
{
}


void InspectionCameraAdapter::OnMouseMove([[maybe_unused]] const glm::vec2& position)
{
}

void InspectionCameraAdapter::OnMouseDrag(const glm::vec2& screen_pos)
{	
	glm::vec2 screen_delta = screen_pos - lastScreenPos;
	float movement_len = glm::length(screen_delta);

	if (movement_len > 0.0f)
	{
		//std:: cout <<"sp, x=" << screenPos.x << " y=" << screenPos.y << std::endl ;

		auto& frustum = cam->CameraFrustum();

		if (lastButton == Input::MouseButton::LEFT)
		{
			const float pixel_rad_ratio = 0.004f;
			
			//Calculate axis of rotation
			glm::vec3 rotation_axis = glm::normalize(-screen_delta.x * frustum.frame.Up() + -screen_delta.y * frustum.frame.Side());

			glm::mat4 transform_mat = glm::translate(glm::mat4(1.0f), orbitCenter);
			transform_mat = glm::rotate(transform_mat, movement_len * pixel_rad_ratio, rotation_axis);
			transform_mat = glm::translate(transform_mat, -orbitCenter);

			//Transform camera position
			glm::vec3 new_pos = glm::vec3(transform_mat * glm::vec4(frustum.position, 1.0f));
			glm::vec3 new_up  = glm::vec3(transform_mat * glm::vec4(frustum.frame.Up(), 0.0f));
				// glm::vec3 new_dir = glm::vec3(transform_mat * glm::vec4(frustum.frame.ViewDir(), 0.0f));
	
			frustum.frame.Up() = new_up;
			frustum.frame.ViewDir() = glm::normalize(orbitCenter-new_pos);
			frustum.frame.Side() = glm::normalize(glm::cross(frustum.frame.ViewDir(), frustum.frame.Up()));
			frustum.position = new_pos;
			cam->UpdateViewMatrix();
		}
		else if (lastButton == Input::MouseButton::RIGHT)
		{
			float dist = glm::length(orbitCenter - frustum.position);
			float pixel_length_ratio = 0.001f * dist;

			glm::vec3 offset = frustum.frame.Side() * -screen_delta.x * pixel_length_ratio;
			offset += frustum.frame.Up() * screen_delta.y * pixel_length_ratio;

			frustum.position += offset;

			orbitCenter += offset;
			frustum.frame.ViewDir() = glm::normalize(orbitCenter - frustum.position);
			cam->UpdateViewMatrix();
		}

		lastScreenPos = screen_pos;
	}
}

void InspectionCameraAdapter::OnMouseClick(Input::MouseButton button, Input::Direction direction , const glm::vec2& screen_pos)
{
	//std:: cout << "click button=" << button << " state=" << state << " x=" << screenPos.x << " y=" << screenPos.y << std::endl;
	lastScreenPos = screen_pos;
	lastClickDirection = direction;
	lastButton = button;
}

void InspectionCameraAdapter::OnMouseWheel(Input::Direction direction, const glm::vec2& screen_pos)
{
	auto& frustum = cam->CameraFrustum();
	vec3 dir = orbitCenter - frustum.position;
	bool up = direction == Input::Direction::UP;

	float len = glm::length(dir);

	if((len <= frustum.nearPlane && up) || (len >= frustum.farPlane && !up))
		return;
	
	dir = glm::normalize(dir) * 0.1f * (up ? 1.0f : -1.0f);
	frustum.position += dir;
	cam->UpdateViewMatrix();

	lastScreenPos = screen_pos;
}