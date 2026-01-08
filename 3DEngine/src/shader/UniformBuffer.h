#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>

#include <glm/fwd.hpp>

#include "../core/gl.h"

class UniformBuffer;
using UniformBufferPtr = std::shared_ptr<UniformBuffer>;
using UniformBufferConstPtr = std::shared_ptr<const UniformBuffer>;
using UniformBufferWeakPtr = std::weak_ptr<UniformBuffer>;
class GLSLProgram;
using GLSLProgramPtr = std::shared_ptr<GLSLProgram>;
using GLSLProgramConstPtr = std::shared_ptr<const GLSLProgram>;
using GLSLProgramWeakPtr = std::weak_ptr<GLSLProgram>;

class UniformBuffer
{
public:
	///The glsl program and bufferName is needed to allocate the right amount of mem (queried)
	
	UniformBuffer(const GLSLProgramPtr program, std::string bufferName, const std::vector<std::string>& names);

	~UniformBuffer(void);

	void BindToShader(GLSLProgramPtr program, std::string bufferName);
	
	void SetElement(const std::string& name, float v);
	void SetElement(const std::string& name, const glm::vec3& v);
	void SetElement(const std::string& name, const glm::vec4& v);
	void SetElement(const std::string& name, const glm::mat4& v);

protected:
	

	void PrintUniforms(const GLSLProgramPtr program, const std::vector<std::string>& elemNames, GLuint* indices, GLint* eOffsets);
	inline void SetElement(const std::string& name, const void* ptr, const GLsizei numBytes);

	std::map<std::string,GLint> offsets;
	GLuint uboHandle;
};

