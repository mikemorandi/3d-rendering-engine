
#include "DirectionalLight.h"

#include "../shape/Box.h"
#include "../util/Util.h"
#include "../materials/Material.h"
#include "../math/BoundingBox.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

DirectionalLightPtr DirectionalLight::Create(const glm::vec3& direction)
{
	DirectionalLightPtr ptr = DirectionalLightPtr(new DirectionalLight(direction));
	return ptr;
}

DirectionalLight::DirectionalLight(const glm::vec3& direction)
{
	this->direction = glm::normalize(direction);

	visMesh = Util::CreateWireBox();

	ConstantColorMaterialPtr mat = ConstantColorMaterial::Create();
	mat->color = color;
	visMesh->SetMaterial(mat);
	visMesh->SetWorldTransform(glm::scale(glm::mat4(1.0), glm::vec3(0.f)));
}

void DirectionalLight::UpdateVisMesh()
{
	visMesh->SetWorldTransform(glm::scale(glm::mat4(1.0), glm::vec3(0.5f)));
}

const glm::vec3& DirectionalLight::Direction() const
{
	return direction;
}

void DirectionalLight::SetDirection(const glm::vec3& dir)
{
	direction = glm::normalize(dir);
}

ShapePtr DirectionalLight::ModelRepresentation() const
{
	return visMesh;
}

glm::mat4 DirectionalLight::GetLightViewMatrix() const
{
	return lightViewMatrix;
}

glm::mat4 DirectionalLight::GetLightProjectionMatrix() const
{
	return lightProjectionMatrix;
}

void DirectionalLight::UpdateLightMatrices(const AABBox& sceneBounds)
{
	// For directional lights, we use orthographic projection
	// Position the light far enough to cover the scene
	glm::vec3 center = sceneBounds.p;  // p is the center of the bounding box
	float radius = glm::length(sceneBounds.d);

	// Place light position along the negative direction from scene center
	glm::vec3 lightPos = center - direction * (radius * 2.0f);

	// Create view matrix looking at the scene center
	glm::vec3 up = glm::abs(direction.y) > 0.999f ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
	lightViewMatrix = glm::lookAt(lightPos, center, up);

	// Transform scene bounds to light space to get tight orthographic frustum
	glm::vec3 minBounds(std::numeric_limits<float>::max());
	glm::vec3 maxBounds(std::numeric_limits<float>::lowest());

	// Transform all 8 corners of the bounding box to light space
	for (int x = 0; x < 2; ++x)
	{
		for (int y = 0; y < 2; ++y)
		{
			for (int z = 0; z < 2; ++z)
			{
				glm::vec3 corner = sceneBounds.p + sceneBounds.d * glm::vec3(x * 2.0f - 1.0f, y * 2.0f - 1.0f, z * 2.0f - 1.0f);
				glm::vec4 lightSpaceCorner = lightViewMatrix * glm::vec4(corner, 1.0f);

				minBounds = glm::min(minBounds, glm::vec3(lightSpaceCorner));
				maxBounds = glm::max(maxBounds, glm::vec3(lightSpaceCorner));
			}
		}
	}

	// Add some padding to avoid edge clipping
	float padding = radius * 0.1f;
	minBounds -= glm::vec3(padding);
	maxBounds += glm::vec3(padding);

	// Create orthographic projection matrix
	lightProjectionMatrix = glm::ortho(
		minBounds.x, maxBounds.x,
		minBounds.y, maxBounds.y,
		-maxBounds.z, -minBounds.z  // Negate Z for proper depth direction
	);
}
