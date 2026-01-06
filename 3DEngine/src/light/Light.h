#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "../util/SharedPointer.h"

SHARED_PTR_CLASS_DECL(Light);
SHARED_PTR_CLASS_DECL(Scene);
SHARED_PTR_CLASS_DECL(Shape);
SHARED_PTR_CLASS_DECL(Framebuffer);
SHARED_PTR_CLASS_DECL(DepthTexture);

class Scene;
class AABBox;

class Light
{
public:

	Light();
	virtual ~Light() = default;

	virtual const glm::vec3& Color();
	virtual void SetColor(const glm::vec3& pos);

	virtual Shape_ptr ModelRepresentation() const;

	// Shadow mapping support
	virtual void SetupShadowMapping(int resolution = 2048);
	virtual void DestroyShadowMap();
	virtual glm::mat4 GetLightSpaceMatrix() const;
	virtual glm::mat4 GetLightViewMatrix() const = 0;
	virtual glm::mat4 GetLightProjectionMatrix() const = 0;
	virtual DepthTexture_ptr GetShadowMap() const;
	virtual Framebuffer_ptr GetShadowFramebuffer() const;
	virtual bool CastsShadows() const;
	virtual void SetCastsShadows(bool enabled);
	virtual int GetShadowMapResolution() const;

protected:
	glm::vec3 color;

	// Shadow mapping members
	bool castsShadows;
	Framebuffer_ptr shadowFramebuffer;
	DepthTexture_ptr shadowMap;
	glm::mat4 lightProjectionMatrix;
	glm::mat4 lightViewMatrix;
	int shadowMapResolution;
};