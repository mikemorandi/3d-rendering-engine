#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

class Light;
using LightPtr = std::shared_ptr<Light>;
using LightConstPtr = std::shared_ptr<const Light>;
using LightWeakPtr = std::weak_ptr<Light>;

class Scene;
using ScenePtr = std::shared_ptr<Scene>;
using SceneConstPtr = std::shared_ptr<const Scene>;
using SceneWeakPtr = std::weak_ptr<Scene>;

class Shape;
using ShapePtr = std::shared_ptr<Shape>;
using ShapeConstPtr = std::shared_ptr<const Shape>;
using ShapeWeakPtr = std::weak_ptr<Shape>;

class Framebuffer;
using FramebufferPtr = std::shared_ptr<Framebuffer>;
using FramebufferConstPtr = std::shared_ptr<const Framebuffer>;
using FramebufferWeakPtr = std::weak_ptr<Framebuffer>;

class DepthTexture;
using DepthTexturePtr = std::shared_ptr<DepthTexture>;
using DepthTextureConstPtr = std::shared_ptr<const DepthTexture>;
using DepthTextureWeakPtr = std::weak_ptr<DepthTexture>;

class Scene;
class AABBox;

class Light
{
public:

	Light();
	virtual ~Light() = default;

	virtual const glm::vec3& Color();
	virtual void SetColor(const glm::vec3& pos);

	virtual ShapePtr ModelRepresentation() const;

	// Shadow mapping support
	virtual void SetupShadowMapping(int resolution = 2048);
	virtual void DestroyShadowMap();
	virtual glm::mat4 GetLightSpaceMatrix() const;
	virtual glm::mat4 GetLightViewMatrix() const = 0;
	virtual glm::mat4 GetLightProjectionMatrix() const = 0;
	virtual DepthTexturePtr GetShadowMap() const;
	virtual FramebufferPtr GetShadowFramebuffer() const;
	virtual bool CastsShadows() const;
	virtual void SetCastsShadows(bool enabled);
	virtual int GetShadowMapResolution() const;

protected:
	glm::vec3 color;

	// Shadow mapping members
	bool castsShadows;
	FramebufferPtr shadowFramebuffer;
	DepthTexturePtr shadowMap;
	glm::mat4 lightProjectionMatrix;
	glm::mat4 lightViewMatrix;
	int shadowMapResolution;
};