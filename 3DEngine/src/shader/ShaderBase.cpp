#include "ShaderBase.h"

#include "../camera/Camera.h"
#include "../util/Util.h"
#include "../scene/Scene.h"
#include "../config.h"
#include "../error.h"

#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

ShaderBase::ShaderBase(const std::string& shaderName)
	: GLSLProgram()
	, hasMVP(true)
	, hasNM(true)
	, hasMVM(true)
	, hasVM(false)
	, hasMM(false)
	, hasPM(false)
	, shaderName(shaderName)
	, vertexAttribInfo(VertexAttributeInfo::Create())
{
	LoadShader(shaderName);
	Init();
}

ShaderBase::~ShaderBase()
{
}

void ShaderBase::Init()
{
	//Query available vertex attributes
	for (int attr_i = GLSLShader::Position; attr_i != GLSLShader::TextureCoord; attr_i++)
	{
		GLSLShader::VertexAttribute attr = static_cast<GLSLShader::VertexAttribute>(attr_i);
		int c = GetAttributeChannel(attr);
		if (c >= 0)
		{
			vertexAttribInfo->mapping[attr] = c;
		}
	}
}

GLint ShaderBase::GetCurentProgram()
{
	GLint currentProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
	return currentProgram;
}

const std::string& ShaderBase::GetName() const
{ 
	return shaderName; 
}

void ShaderBase::BeforeUniformSet()
{
	currentProgram = GetCurentProgram();

	if(static_cast<GLuint>(currentProgram) != programHandle)
		GLSLProgram::Use();

}

void ShaderBase::AfterUniformSet()
{
	if(static_cast<GLuint>(currentProgram) != programHandle)
		glUseProgram(currentProgram);
}

void ShaderBase::UpdateTransforms(const CameraPtr& cam, const glm::mat4& modelTransform)
{
	glm::mat4 modelViewMatrix = cam->viewMatrix * modelTransform;
	
	glm::mat4 mvpMatrix = cam->projectionMatrix * modelViewMatrix;
	glm::mat3 normalMatrix	= glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));
	
	BeforeUniformSet();

	if(hasMVP)
		SetUniform("MVP", mvpMatrix);
	if(hasNM)
		SetUniform("NormalMatrix", normalMatrix);		
	if(hasMVM)
		SetUniform("ModelViewMatrix", modelViewMatrix);		
	if(hasVM)
		SetUniform("ViewMatrix",  cam->viewMatrix);		
	if(hasMM)
		SetUniform("ModelMatrix",  modelTransform);		
	if(hasPM)
		SetUniform("ProjectionMatrix", cam->projectionMatrix);		
	
	
	AfterUniformSet();
}

GLint ShaderBase::GetAttributeChannel(GLSLShader::VertexAttribute attribute)
{
	
	switch(attribute)
	{
	case GLSLShader::Position:
		return glGetAttribLocation(programHandle, "VertexPosition");
		break;
	case GLSLShader::Normal:
		return  glGetAttribLocation(programHandle, "VertexNormal");
		break;
	case GLSLShader::Tangent:
		return  glGetAttribLocation(programHandle, "VertexTangent");
		break;
	case GLSLShader::Color:
		return glGetAttribLocation(programHandle, "VertexColor");
		break;
	case GLSLShader::TextureCoord:
		return glGetAttribLocation(programHandle, "VertexTexCoord");
		break;
	default:
		return -1;
	}
}

bool ShaderBase::LoadShader(const std::string& shaderName)
{
	Info("Loading shader " + shaderName );

	std::string vertexShaderSource = Util::LoadTextFile( Config::SHADER_BASE_PATH / fs::path(shaderName + ".vert"));
	std::string fragmentShaderSource = Util::LoadTextFile( Config::SHADER_BASE_PATH  / fs::path(shaderName + ".frag"));

	bool ok = HandleIncludes(vertexShaderSource);
	ok &= HandleIncludes(fragmentShaderSource);
	ok &= LoadShader(vertexShaderSource, fragmentShaderSource);

	return ok;
}



bool ShaderBase::HandleIncludes(std::string& shader_src)
{
	std::stringstream sstream(shader_src);
	std::ostringstream new_string;
	bool includes_found = false;

	const std::string incl_str("#include ");

	std::string line;
	while (sstream.good())
	{
		std::getline(sstream, line);

		if (line[0] == '#')
		{
			if (line.compare(0, incl_str.length(), incl_str) == 0)
			{
				std::string incl_file(line.begin() + incl_str.length(), line.end());

				std::string incl_content = Util::LoadTextFile(Config::SHADER_BASE_PATH / incl_file);

				new_string << incl_content;
				includes_found = true;
			}
			else
				new_string << line << std::endl;;
		}
		else
			new_string << line << std::endl;;

	}

	if (includes_found)
		shader_src = new_string.str();

	return true;
}


bool ShaderBase::LoadShader( const std::string& vertexSource, 
							 const std::string& fragmentSource)
{
	if (!this->CompileShaderFromString(vertexSource, GLSLShader::VERTEX))  
	{
		Error("--- Vertex Shader ---\n" + this->Log());
		return false;
	}

	if (!this->CompileShaderFromString(fragmentSource, GLSLShader::FRAGMENT)) 
	{
		Error("--- Fragment Shader ---\n" + this->Log());
		return false;
	}

	if (!this->Link()) {
		Error("--- Linker ---\n" + this->Log());
		return false;
	}

	return true;

}

bool ShaderBase::Use(const ScenePtr& scene, const glm::mat4& modelTransform)
{
	bool ok = GLSLProgram::Use();
	UpdateTransforms(scene->activeCamera,modelTransform);
	return ok;
}

VertexAttributeInfoPtr ShaderBase::GetVertexAttributeInfo() const 
{ 
	return vertexAttribInfo; 
}