#pragma once

#include <memory>
#include <glm/fwd.hpp>
#include "../core/gl.h"


class GeometryBuffer;
using GeometryBufferPtr = std::shared_ptr<GeometryBuffer>;
using GeometryBufferConstPtr = std::shared_ptr<const GeometryBuffer>;
using GeometryBufferWeakPtr = std::weak_ptr<GeometryBuffer>;
class GBufferShader;
using GBufferShaderPtr = std::shared_ptr<GBufferShader>;
using GBufferShaderConstPtr = std::shared_ptr<const GBufferShader>;
using GBufferShaderWeakPtr = std::weak_ptr<GBufferShader>;
class Scene;
using ScenePtr = std::shared_ptr<Scene>;
using SceneConstPtr = std::shared_ptr<const Scene>;
using SceneWeakPtr = std::weak_ptr<Scene>;


class GeometryBuffer
{
public:

	static GeometryBufferPtr Create(int width, int height);

	GeometryBuffer(int width, int height);
	
	virtual ~GeometryBuffer();

	/// 
	void StartGBufferComp(const ScenePtr& scene);
	
	
	void EndGBufferComp();


protected:

	enum TexTypes {Position, Normal};

	GLuint m_DeferredFboHandle;
	GLuint m_DepthBuffer;
	GLuint m_Textures[2];

	GBufferShaderPtr m_Shader;
};

