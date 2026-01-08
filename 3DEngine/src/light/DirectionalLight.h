#pragma once

#include "Light.h"
#include <memory>

class DirectionalLight;
using DirectionalLightPtr = std::shared_ptr<DirectionalLight>;
using DirectionalLightConstPtr = std::shared_ptr<const DirectionalLight>;
using DirectionalLightWeakPtr = std::weak_ptr<DirectionalLight>;

class AABBox;

class DirectionalLight : public Light, public std::enable_shared_from_this<DirectionalLight>
{
public:

	static DirectionalLightPtr Create(const glm::vec3& direction);

	virtual const glm::vec3& Direction() const;
	virtual void SetDirection(const glm::vec3& direction);

	ShapePtr ModelRepresentation() const override;

	// Shadow mapping overrides
	virtual glm::mat4 GetLightViewMatrix() const override;
	virtual glm::mat4 GetLightProjectionMatrix() const override;
	void UpdateLightMatrices(const AABBox& sceneBounds);

protected:

	DirectionalLight(const glm::vec3& direction);

	void UpdateVisMesh();

	glm::vec3 direction;
	ShapePtr visMesh;

};

