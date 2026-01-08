#pragma once

#include "../input/ViewportObserver.h"

#include "Frustum.h"

#include <glm/mat4x4.hpp>
#include <vector>

// Forward declarations
class Camera;
using CameraPtr = std::shared_ptr<Camera>;
using CameraConstPtr = std::shared_ptr<const Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;

class CameraObserver;
using CameraObserverPtr = std::shared_ptr<CameraObserver>;
using CameraObserverConstPtr = std::shared_ptr<const CameraObserver>;
using CameraObserverWeakPtr = std::weak_ptr<CameraObserver>;

class CameraObserver
{
public:
	virtual void CameraChanged() = 0;
};

class Camera : public ViewportObserver
{
public:

	glm::mat4 ViewProjectionTransform() const;

	glm::mat4 ProjectionTransform() const;

	void UpdateViewMatrix();
	virtual void UpdateProjectionMatrix() = 0;

	virtual Frustum& CameraFrustum() = 0;

	virtual const Frustum& CameraFrustum() const = 0;
		 
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	void AddObserver(const CameraObserverPtr& observer);

protected:

	Camera();

	void NotifyObservers();

	std::vector<CameraObserverPtr> observers;
};

