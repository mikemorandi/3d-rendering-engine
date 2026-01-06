#include "PerspectiveCamera.h"
#include "../rendering/Viewport.h"

#include <glm/gtc/matrix_transform.hpp>

using glm::vec4;
using glm::vec3;

PerspectiveCamera::PerspectiveCamera(float fov) 
{
	frustum.position =  vec3(0.0f, 0.0f, 1.0f);
	frustum.nearPlane = 0.01f;
	frustum.farPlane = 100.0f;
	frustum.fovY = fov;
	frustum.aspectRatio = 1.f;

	frustum.frame.ViewDir() = vec3(0.0f, 0.0f, -1.0f);
	frustum.frame.Up() = vec3(0.0f, 1.0f, 0.0f);
	frustum.frame.Side() = glm::cross(frustum.frame.ViewDir(), frustum.frame.Up());

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void PerspectiveCamera::UpdateProjectionMatrix()
{
	projectionMatrix = glm::perspective(glm::radians(frustum.fovY), frustum.aspectRatio, frustum.nearPlane, frustum.farPlane);
	NotifyObservers();
}

void PerspectiveCamera::SetAspectRatio(float aspectRatio)
{
	frustum.aspectRatio = aspectRatio;
	UpdateProjectionMatrix();
}

void PerspectiveCamera::SetFov(float fov)
{
	frustum.fovY = fov;
	UpdateProjectionMatrix();
}

void PerspectiveCamera::ViewportChanged(const Viewport_ptr& viewport)
{
	SetAspectRatio((float)viewport->width / viewport->height);
}

Frustum & PerspectiveCamera::CameraFrustum()
{
	return frustum;
}

const Frustum & PerspectiveCamera::CameraFrustum() const
{
	return frustum;
}
