#pragma once

#include <vector>
#include <memory>

class Camera;
using CameraPtr = std::shared_ptr<Camera>;
using CameraConstPtr = std::shared_ptr<const Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;

class PointLight;
using PointLightPtr = std::shared_ptr<PointLight>;
using PointLightConstPtr = std::shared_ptr<const PointLight>;
using PointLightWeakPtr = std::weak_ptr<PointLight>;

class SpotLight;
using SpotLightPtr = std::shared_ptr<SpotLight>;
using SpotLightConstPtr = std::shared_ptr<const SpotLight>;
using SpotLightWeakPtr = std::weak_ptr<SpotLight>;

class AmbientLight;
using AmbientLightPtr = std::shared_ptr<AmbientLight>;
using AmbientLightConstPtr = std::shared_ptr<const AmbientLight>;
using AmbientLightWeakPtr = std::weak_ptr<AmbientLight>;

class DirectionalLight;
using DirectionalLightPtr = std::shared_ptr<DirectionalLight>;
using DirectionalLightConstPtr = std::shared_ptr<const DirectionalLight>;
using DirectionalLightWeakPtr = std::weak_ptr<DirectionalLight>;

class LightModel;
using LightModelPtr = std::shared_ptr<LightModel>;
using LightModelConstPtr = std::shared_ptr<const LightModel>;
using LightModelWeakPtr = std::weak_ptr<LightModel>;

class UniformBuffer;
using UniformBufferPtr = std::shared_ptr<UniformBuffer>;
using UniformBufferConstPtr = std::shared_ptr<const UniformBuffer>;
using UniformBufferWeakPtr = std::weak_ptr<UniformBuffer>;

class GLSLProgram;
using GLSLProgramPtr = std::shared_ptr<GLSLProgram>;
using GLSLProgramConstPtr = std::shared_ptr<const GLSLProgram>;
using GLSLProgramWeakPtr = std::weak_ptr<GLSLProgram>;

class Texture3D;
using Texture3DPtr = std::shared_ptr<Texture3D>;
using Texture3DConstPtr = std::shared_ptr<const Texture3D>;
using Texture3DWeakPtr = std::weak_ptr<Texture3D>;

class Texture2D;
using Texture2DPtr = std::shared_ptr<Texture2D>;
using Texture2DConstPtr = std::shared_ptr<const Texture2D>;
using Texture2DWeakPtr = std::weak_ptr<Texture2D>;

class Light;
using LightPtr = std::shared_ptr<Light>;
using LightConstPtr = std::shared_ptr<const Light>;
using LightWeakPtr = std::weak_ptr<Light>;

class AABBox;

class LightModel
{
public:
	LightModel();

	~LightModel();

	UniformBufferPtr GetLightsBuffer() const;
	void UpdateUniformBuffer(const CameraConstPtr& cam);

	bool IsValid();

	// Shadow mapping support
	void InitializeShadowMaps(const AABBox& sceneBounds, int resolution = 2048);
	std::vector<LightPtr> GetShadowCastingLights() const;
	void UpdateShadowMatrices(const AABBox& sceneBounds);

public:

	std::vector<PointLightPtr> pointLights;
	std::vector<SpotLightPtr>	spotLights;
	DirectionalLightPtr		directionalLight;
	AmbientLightPtr			ambientLight;

protected:

	UniformBufferPtr lightsBuffer;
	bool valid;
};

