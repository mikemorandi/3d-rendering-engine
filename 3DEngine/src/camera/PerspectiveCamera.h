#pragma once

#include "Camera.h"
#include <memory>

class PerspectiveCamera;
using PerspectiveCameraPtr = std::shared_ptr<PerspectiveCamera>;
using PerspectiveCameraConstPtr = std::shared_ptr<const PerspectiveCamera>;
using PerspectiveCameraWeakPtr = std::weak_ptr<PerspectiveCamera>;

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(float fov);
	virtual ~PerspectiveCamera() = default;

	void SetFov(float fov);
	void SetAspectRatio(float aspectRatio);

	virtual void ViewportChanged(const ViewportPtr& viewport) override;

	virtual Frustum& CameraFrustum() override;

	virtual const Frustum& CameraFrustum() const override;

protected:

	void UpdateProjectionMatrix() override;

	PerspectiveFrustum frustum;
};

