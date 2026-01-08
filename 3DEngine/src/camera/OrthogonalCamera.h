#pragma once

#include "Camera.h"
#include <memory>

class OrthogonalCamera;
using OrthogonalCameraPtr = std::shared_ptr<OrthogonalCamera>;
using OrthogonalCameraConstPtr = std::shared_ptr<const OrthogonalCamera>;
using OrthogonalCameraWeakPtr = std::weak_ptr<OrthogonalCamera>;

class OrthogonalCamera : public Camera
{
public:
	OrthogonalCamera();
	virtual ~OrthogonalCamera();

	virtual void ViewportChanged(const ViewportPtr& viewport) override;

	virtual Frustum& CameraFrustum() override;

	virtual const Frustum& CameraFrustum() const override;

protected:

	void UpdateProjectionMatrix() override;

	OrthogonalFrustum frustum;
};

