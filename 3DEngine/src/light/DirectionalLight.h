#pragma once

#include "Light.h"

SHARED_PTR_CLASS_DECL(DirectionalLight);

class AABBox;

class DirectionalLight : public Light, public std::enable_shared_from_this<DirectionalLight>
{
public:

	static DirectionalLight_ptr Create(const glm::vec3& direction);

	virtual const glm::vec3& Direction() const;
	virtual void SetDirection(const glm::vec3& direction);

	Shape_ptr ModelRepresentation() const override;

	// Shadow mapping overrides
	virtual glm::mat4 GetLightViewMatrix() const override;
	virtual glm::mat4 GetLightProjectionMatrix() const override;
	void UpdateLightMatrices(const AABBox& sceneBounds);

protected:

	DirectionalLight(const glm::vec3& direction);

	void UpdateVisMesh();

	glm::vec3 direction;
	Shape_ptr visMesh;

};

