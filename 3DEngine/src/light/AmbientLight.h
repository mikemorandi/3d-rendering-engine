#pragma once

#include "Light.h"
#include "../util/SharedPointer.h"
#include <memory>

#include <glm/fwd.hpp>

class AmbientLight;
using AmbientLightPtr = std::shared_ptr<AmbientLight>;
using AmbientLightConstPtr = std::shared_ptr<const AmbientLight>;
using AmbientLightWeakPtr = std::weak_ptr<AmbientLight>;


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

