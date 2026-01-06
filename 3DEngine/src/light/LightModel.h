#pragma once

#include <vector>

#include "../util/SharedPointer.h"

SHARED_PTR_CLASS_DECL(Camera);
SHARED_PTR_CLASS_DECL(PointLight);
SHARED_PTR_CLASS_DECL(SpotLight);
SHARED_PTR_CLASS_DECL(AmbientLight)
SHARED_PTR_CLASS_DECL(DirectionalLight);
SHARED_PTR_CLASS_DECL(LightModel);
SHARED_PTR_CLASS_DECL(UniformBuffer);
SHARED_PTR_CLASS_DECL(GLSLProgram);
SHARED_PTR_CLASS_DECL(Texture3D);
SHARED_PTR_CLASS_DECL(Texture2D);
SHARED_PTR_CLASS_DECL(Light);

class AABBox;

class LightModel
{
public:
	LightModel();

	~LightModel();

	UniformBuffer_ptr GetLightsBuffer() const;
	void UpdateUniformBuffer(const Camera_cptr& cam);

	bool IsValid();

	// Shadow mapping support
	void InitializeShadowMaps(const AABBox& sceneBounds, int resolution = 2048);
	std::vector<Light_ptr> GetShadowCastingLights() const;
	void UpdateShadowMatrices(const AABBox& sceneBounds);

public:

	std::vector<PointLight_ptr> pointLights;
	std::vector<SpotLight_ptr>	spotLights;
	DirectionalLight_ptr		directionalLight;
	AmbientLight_ptr			ambientLight;

protected:

	UniformBuffer_ptr lightsBuffer;
	bool valid;
};

