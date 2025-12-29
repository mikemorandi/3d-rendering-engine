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

class LightModel
{
public:
	LightModel();

	~LightModel();

	UniformBuffer_ptr GetLightsBuffer() const;
	void UpdateUniformBuffer(const Camera_cptr& cam);

	bool IsValid();

public:

	std::vector<PointLight_ptr> pointLights;
	std::vector<SpotLight_ptr>	spotLights;
	DirectionalLight_ptr		directionalLight;
	AmbientLight_ptr			ambientLight;
	Texture3D_ptr				pcfShadowRandomData;
	Texture2D_ptr				ditherData;

protected:

	UniformBuffer_ptr lightsBuffer;
	bool valid;
};

