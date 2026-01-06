#pragma once

#include "Camera.h"

SHARED_PTR_CLASS_DECL(OrthogonalCamera);

class OrthogonalCamera : public Camera
{
public:
	OrthogonalCamera();
	virtual ~OrthogonalCamera();

	virtual void ViewportChanged(const Viewport_ptr& viewport) override;

	virtual Frustum& CameraFrustum() override;

	virtual const Frustum& CameraFrustum() const override;

protected:

	void UpdateProjectionMatrix() override;

	OrthogonalFrustum frustum;
};

