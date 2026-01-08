#pragma once

#include "Light.h"
#include "../util/SharedPointer.h"

#include <glm/vec4.hpp>
#include <memory>

class Shape;
using ShapePtr = std::shared_ptr<Shape>;
using ShapeConstPtr = std::shared_ptr<const Shape>;
using ShapeWeakPtr = std::weak_ptr<Shape>;

class PointLight;
using PointLightPtr = std::shared_ptr<PointLight>;
using PointLightConstPtr = std::shared_ptr<const PointLight>;
using PointLightWeakPtr = std::weak_ptr<PointLight>;

class ConstColorShader;
using ConstColorShaderPtr = std::shared_ptr<ConstColorShader>;
using ConstColorShaderConstPtr = std::shared_ptr<const ConstColorShader>;
using ConstColorShaderWeakPtr = std::weak_ptr<ConstColorShader>;


class PointLight : public Light
{
public:

	SHARED_PTR_FACTORY(PointLight);

	PointLight();

	virtual ~PointLight();

	virtual	const glm::vec4& Position() const;
	virtual void SetPosition(const glm::vec4& pos);
	virtual void SetColor(const glm::vec3& color) override;
	virtual ShapePtr ModelRepresentation() const override;

	virtual void SetAnimated(bool animated);
	virtual bool Animated() const;

	// Shadow mapping overrides (PointLight doesn't support shadows by default)
	virtual glm::mat4 GetLightViewMatrix() const override;
	virtual glm::mat4 GetLightProjectionMatrix() const override;

	bool animated;
	glm::vec4 position;
	ShapePtr visMesh;
};

