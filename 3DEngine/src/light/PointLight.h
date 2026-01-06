#pragma once

#include "Light.h"

#include <glm/vec4.hpp>

SHARED_PTR_CLASS_DECL(Shape)
SHARED_PTR_CLASS_DECL(PointLight);
SHARED_PTR_CLASS_DECL(ConstColorShader);


class PointLight : public Light
{
public:

	SHARED_PTR_FACTORY(PointLight);

	PointLight();

	virtual ~PointLight();

	virtual	const glm::vec4& Position() const;
	virtual void SetPosition(const glm::vec4& pos);
	virtual void SetColor(const glm::vec3& color) override;
	virtual Shape_ptr ModelRepresentation() const override;

	virtual void SetAnimated(bool animated);
	virtual bool Animated() const;

	// Shadow mapping overrides (PointLight doesn't support shadows by default)
	virtual glm::mat4 GetLightViewMatrix() const override;
	virtual glm::mat4 GetLightProjectionMatrix() const override;

	bool animated;
	glm::vec4 position;
	Shape_ptr visMesh;
};

