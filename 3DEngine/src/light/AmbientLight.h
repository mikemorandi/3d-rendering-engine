#pragma once

#include "Light.h"

#include <glm/fwd.hpp>

SHARED_PTR_CLASS_DECL(AmbientLight);


class AmbientLight : public Light
{
public:
	
	SHARED_PTR_FACTORY(AmbientLight)

	AmbientLight();

	virtual ~AmbientLight();

	// Shadow mapping overrides (AmbientLight doesn't support shadows)
	virtual glm::mat4 GetLightViewMatrix() const override;
	virtual glm::mat4 GetLightProjectionMatrix() const override;

};

