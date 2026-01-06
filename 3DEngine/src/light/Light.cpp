#include "Light.h"
#include "../texture/Framebuffer.h"
#include "../texture/DepthTexture.h"

Light::Light()
	: color(glm::vec3(1,1,1))
	, castsShadows(false)
	, lightProjectionMatrix(1.0f)
	, lightViewMatrix(1.0f)
	, shadowMapResolution(2048)
{

}

const glm::vec3& Light::Color()
{
	return color;
}

void Light::SetColor(const glm::vec3& color)
{
	this->color = color;
}

Shape_ptr Light::ModelRepresentation() const
{
	return Shape_ptr();
}

void Light::SetupShadowMapping(int resolution)
{
	shadowMapResolution = resolution;

	// Create depth texture for shadow map
	shadowMap = DepthTexture::Create(resolution, resolution);
	if (!shadowMap)
	{
		castsShadows = false;
		return;
	}

	// Create framebuffer for shadow rendering
	shadowFramebuffer = Framebuffer::Create();
	if (!shadowFramebuffer)
	{
		shadowMap.reset();
		castsShadows = false;
		return;
	}

	// Attach depth texture to framebuffer
	shadowFramebuffer->Bind();
	shadowFramebuffer->SetDrawToColorBufferEnabled(false);
	if (!shadowFramebuffer->Attach(shadowMap, Framebuffer::Attachment::Depth))
	{
		shadowMap.reset();
		shadowFramebuffer.reset();
		castsShadows = false;
		return;
	}

	if (!shadowFramebuffer->IsComplete())
	{
		shadowMap.reset();
		shadowFramebuffer.reset();
		castsShadows = false;
		return;
	}

	shadowFramebuffer->Unbind();
	castsShadows = true;
}

void Light::DestroyShadowMap()
{
	shadowMap.reset();
	shadowFramebuffer.reset();
	castsShadows = false;
}

glm::mat4 Light::GetLightSpaceMatrix() const
{
	return lightProjectionMatrix * lightViewMatrix;
}

DepthTexture_ptr Light::GetShadowMap() const
{
	return shadowMap;
}

Framebuffer_ptr Light::GetShadowFramebuffer() const
{
	return shadowFramebuffer;
}

bool Light::CastsShadows() const
{
	return castsShadows && shadowMap && shadowFramebuffer;
}

void Light::SetCastsShadows(bool enabled)
{
	if (enabled && !castsShadows)
	{
		SetupShadowMapping(shadowMapResolution);
	}
	else if (!enabled && castsShadows)
	{
		DestroyShadowMap();
	}
}

int Light::GetShadowMapResolution() const
{
	return shadowMapResolution;
}