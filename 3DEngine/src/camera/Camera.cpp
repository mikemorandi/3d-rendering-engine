#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
	: viewMatrix(glm::mat4(1.f))
	, projectionMatrix(glm::mat4(1.f))
{

}

glm::mat4 Camera::ViewProjectionTransform() const
{
	return projectionMatrix * viewMatrix;
}

glm::mat4 Camera::ProjectionTransform() const
{
	return projectionMatrix;
}

void Camera::UpdateViewMatrix()
{
	Frustum& frustum = CameraFrustum();
	glm::vec3 target = frustum.position + frustum.frame.ViewDir();
	viewMatrix = glm::lookAt(frustum.position, target, frustum.frame.Up());
	NotifyObservers();
}

void Camera::AddObserver(const CameraObserverPtr & observer)
{
	observers.push_back(observer);
}

void Camera::NotifyObservers()
{	
	for (auto& obs : observers)	
		obs->CameraChanged();	
}
