#pragma once

#include "GLSLProgram.h"
#include "../enums.h"

#include <map>

SHARED_PTR_CLASS_DECL(ShaderBase)
SHARED_PTR_CLASS_DECL(VertexAttributeInfo)
SHARED_PTR_CLASS_DECL(Scene)
SHARED_PTR_CLASS_DECL(Camera)
SHARED_PTR_CLASS_DECL(Material)

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
	virtual VertexAttributeInfo_ptr GetVertexAttributeInfo() const;

	virtual bool Use(const Scene_ptr& scene, const glm::mat4& modelTransform);

	const std::string& GetName() const;

protected:

	ShaderBase(const std::string& shaderName);

	virtual void Init();

	bool LoadShader(const std::string& vertexSource, const std::string& fragmentSource);
	bool LoadShader(const std::string& shaderName);	

	bool HandleIncludes(std::string& fragmentSource);

	virtual void UpdateTransforms(const Camera_ptr& cam, const glm::mat4& modelTransform);

	GLint GetCurentProgram();
	
	void BeforeUniformSet();
	void AfterUniformSet();

	bool hasMVP, hasNM, hasMVM, hasVM, hasMM, hasPM;

	std::string shaderName;
	GLint currentProgram;

private:
	VertexAttributeInfo_ptr vertexAttribInfo;
	
};

