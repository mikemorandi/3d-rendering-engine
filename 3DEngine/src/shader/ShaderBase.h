#pragma once

#include "GLSLProgram.h"
#include "../enums.h"
#include "../util/SharedPointer.h"

#include <map>
#include <memory>

class ShaderBase;
using ShaderBasePtr = std::shared_ptr<ShaderBase>;
using ShaderBaseConstPtr = std::shared_ptr<const ShaderBase>;
using ShaderBaseWeakPtr = std::weak_ptr<ShaderBase>;

class VertexAttributeInfo;
using VertexAttributeInfoPtr = std::shared_ptr<VertexAttributeInfo>;
using VertexAttributeInfoConstPtr = std::shared_ptr<const VertexAttributeInfo>;
using VertexAttributeInfoWeakPtr = std::weak_ptr<VertexAttributeInfo>;

class Scene;
using ScenePtr = std::shared_ptr<Scene>;
using SceneConstPtr = std::shared_ptr<const Scene>;
using SceneWeakPtr = std::weak_ptr<Scene>;

class Camera;
using CameraPtr = std::shared_ptr<Camera>;
using CameraConstPtr = std::shared_ptr<const Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;

class Material;
using MaterialPtr = std::shared_ptr<Material>;
using MaterialConstPtr = std::shared_ptr<const Material>;
using MaterialWeakPtr = std::weak_ptr<Material>;

class VertexAttributeInfo
{
public:
	SHARED_PTR_FACTORY(VertexAttributeInfo);

	//typedef std::pair<GLSLShader::VertexAttribute, int> AttribChannel;
	std::map<GLSLShader::VertexAttribute, int> mapping;

	VertexAttributeInfo() {};
};

class ShaderBase : public GLSLProgram
{
public:

	virtual ~ShaderBase();

	// Bring base class Use() into scope to avoid hiding
	using GLSLProgram::Use;

	/* \brief Queries the shader for the channel ids
	*  \return the channel id for the specified vertex attribute, -1 for inexisting attrib */
	GLint GetAttributeChannel(GLSLShader::VertexAttribute attribute);

	/* \brief Returns the necessary vertex attribute info the shader uses */
	virtual VertexAttributeInfoPtr GetVertexAttributeInfo() const;

	virtual bool Use(const ScenePtr& scene, const glm::mat4& modelTransform);

	const std::string& GetName() const;

protected:

	ShaderBase(const std::string& shaderName);

	virtual void Init();

	bool LoadShader(const std::string& vertexSource, const std::string& fragmentSource);
	bool LoadShader(const std::string& shaderName);	

	bool HandleIncludes(std::string& fragmentSource);

	virtual void UpdateTransforms(const CameraPtr& cam, const glm::mat4& modelTransform);

	GLint GetCurentProgram();

	void BeforeUniformSet();
	void AfterUniformSet();

	bool hasMVP, hasNM, hasMVM, hasVM, hasMM, hasPM;

	std::string shaderName;
	GLint currentProgram;

private:
	VertexAttributeInfoPtr vertexAttribInfo;
	
};

