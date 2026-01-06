#include "OrthogonalCamera.h"
#include "../rendering/Viewport.h"

#include <glm/gtc/matrix_transform.hpp>

using glm::vec4;
using glm::vec3;

OrthogonalCamera::OrthogonalCamera() 
{
	frustum.position = vec3(0.0f, 0.0f, 1.0f);
	frustum.nearPlane = 0.01f;
	frustum.farPlane = 10.0f;
	frustum.left = 0.f;
	frustum.top = 0.f;

	frustum.frame.ViewDir() = vec3(0.0f, 0.0f, -1.0f);
	frustum.frame.Up() = vec3(0.0f, 1.0f, 0.0f);
	frustum.frame.Side() = glm::cross(frustum.frame.ViewDir(), frustum.frame.Up());

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

OrthogonalCamera::~OrthogonalCamera()
{

}

void OrthogonalCamera::UpdateProjectionMatrix()
{
	projectionMatrix = glm::ortho(frustum.left, frustum.right, frustum.bottom, frustum.top,
								  frustum.nearPlane, frustum.farPlane);
	NotifyObservers();
}

void OrthogonalCamera::ViewportChanged(const Viewport_ptr& viewport)
{
	frustum.right = static_cast<float>(viewport->width);
	frustum.bottom = static_cast<float>(viewport->height);
	UpdateProjectionMatrix();
}

Frustum & OrthogonalCamera::CameraFrustum()
{
	return frustum;
}

const Frustum & OrthogonalCamera::CameraFrustum() const
{
	return frustum;
}
