#pragma once

#include "../util/SharedPointer.h"
#include "../math/BoundingBox.h"

SHARED_PTR_CLASS_DECL(ShadowCastingLight);
SHARED_PTR_CLASS_DECL(Shadow);
SHARED_PTR_CLASS_DECL(Camera);

class ShadowCastingLight
{
public:

	virtual ~ShadowCastingLight() = default;

	virtual Shadow_ptr Shadow() const;

	virtual void UpdateShadow() = 0;

	virtual void SetSceneBoundingBox(const AABBox& box);

	const AABBox& SceneBoundingBox() const;

	virtual void SetCamera(const Camera_cptr& cam);

protected:	

	Camera_cptr camera;
	Shadow_ptr shadow;
	AABBox sceneBbox;
};

