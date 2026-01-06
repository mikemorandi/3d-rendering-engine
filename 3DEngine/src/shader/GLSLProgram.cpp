#include "GLSLProgram.h"
#include <string>
#include <glm/glm.hpp>

GLSLProgram::GLSLProgram() 
	: programHandle(0)
	, Linked(false) 
{
	programHandle = glCreateProgram();

	if( programHandle == 0) 
	{
		logString = "Unable to create shader program";
	}   
}

GLSLProgram::~GLSLProgram()
{
	if ( programHandle > 0 ) 
	{
		glDeleteProgram(programHandle);
	}
}

bool GLSLProgram::CompileShaderFromString( const std::string& source,  
										  GLSLShader::GLSLShaderType type)
{
	GLuint shaderHandle = 0;

	switch( type ) 
	{
	case GLSLShader::VERTEX:
		shaderHandle = glCreateShader(GL_VERTEX_SHADER);
		break;
	case GLSLShader::FRAGMENT:
		shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	case GLSLShader::GEOMETRY:
		shaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
		break;
	case GLSLShader::TESSELATION_CONTROL:
		shaderHandle = glCreateShader(GL_TESS_CONTROL_SHADER);
		break;
	case GLSLShader::TESSELATION_EVALUATION:
		shaderHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);
		break;
	default:
		return false;
	}

	const GLchar * c_code = source.c_str();
	glShaderSource( shaderHandle, 1, &c_code, nullptr );

	// Compile the shader
	glCompileShader(shaderHandle );

	// Check for errors
	GLint result;
	glGetShaderiv( shaderHandle, GL_COMPILE_STATUS, &result );

	if( GL_FALSE == result ) 
	{       
		logString = GetInfoLog(shaderHandle);
		return false;
	} 
	else 
	{
		// Compile succeeded, attach shader and return true
		glAttachShader(programHandle, shaderHandle);
		return true;
	}
}

bool GLSLProgram::Link()
{
	if( Linked ) 
		return true;

	if( programHandle <= 0 ) 
	{
		logString = "Invalid program handle";
		return false;
	}

	glLinkProgram(programHandle);

	int status = 0;
	glGetProgramiv( programHandle, GL_LINK_STATUS, &status);
	if( GL_TRUE != status ) 
	{
		logString = GetInfoLog(programHandle);
		return false;
	} else {
		Linked = true;
		return Linked;
	}
}

bool GLSLProgram::isLinked() const
{
	return Linked;
}

bool GLSLProgram::Validate()
{
	glValidateProgram(programHandle);
	GLint status;
	glGetProgramiv(programHandle, GL_VALIDATE_STATUS, &status);

	logString = GetInfoLog(programHandle);

	return GL_TRUE == status;
}

GLuint GLSLProgram::GetProgramHandle() const
{ 
	return programHandle; 
};

std::string GLSLProgram::Log() const
{
	return logString;
}

bool GLSLProgram::Use()
{
	if( programHandle <= 0 || (! Linked) ) 
		return false;

	glUseProgram(programHandle);

	return true;

}

void GLSLProgram::UnUse()
{
	glUseProgram(0);
}
void GLSLProgram::SetUniform(const GLchar *name, bool value)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0) 
	{
		glUniform1f(loc, value);
	} 
	else 
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}

void GLSLProgram::SetUniform(const GLchar *name, float value)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0) 
	{
		glUniform1f(loc, value);
	} 
	else 
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}

void GLSLProgram::SetUniform(const GLchar *name, int value)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0) 
	{
		glUniform1i(loc, value);
	} 
	else 
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}

void GLSLProgram::SetUniform(const GLchar *name, unsigned int value)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0) 
	{
		glUniform1i(loc, value);
	} 
	else 
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}

void GLSLProgram::SetUniform(const GLchar *name, const glm::vec2& v)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0) 
	{
		glUniform2f(loc, v.x, v.y);
	} 
	else 
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}

void GLSLProgram::SetUniform(const GLchar *name, const glm::vec3& v)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0) 
	{
		glUniform3f(loc, v.x, v.y, v.z);
	} 
	else 
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}

void GLSLProgram::SetUniform(const GLchar *name, const glm::ivec2& v)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0)
	{
		glUniform2i(loc, v.x, v.y);
	}
	else
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}


void GLSLProgram::SetUniform(const GLchar *name, const glm::ivec3& v)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0)
	{
		glUniform3i(loc, v.x, v.y, v.z);
	}
	else
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}

void GLSLProgram::SetUniform(const GLchar *name, const glm::ivec4& v)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0)
	{
		glUniform4i(loc, v.x, v.y, v.z, v.w);
	}
	else
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}

void GLSLProgram::SetUniform(const char *name, const glm::vec4& v)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0) 
	{
		glUniform4f(loc, v.x, v.y, v.z, v.w);
	} 
	else 
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}

void GLSLProgram::SetUniform(const GLchar *name, const glm::mat3& m)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0) 
	{
		glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
	} 
	else 
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}

void GLSLProgram::SetUniform(const GLchar *name, const glm::mat4& m)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc >= 0) 
	{
		glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
	} 
	else 
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
	}
}

void GLSLProgram::SetUniformArray(const GLchar *name, 
								  GLfloat *v, 
								  int elementSize, 
								  int count)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc == -1) 
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
		return;
	}

	switch (elementSize) {
	case 1:
		glUniform1fv(loc, count, v);
		break;
	case 2:
		glUniform2fv(loc, count, v);
		break;
	case 3:
		glUniform3fv(loc, count, v);
		break;
	case 4:
		glUniform4fv(loc, count, v);
		break;
	}
}

void GLSLProgram::SetUniformArray(const GLchar *name,
	GLint *v,
	int elementSize,
	int count)
{
	GLint loc = glGetUniformLocation(programHandle, name);
	if (loc == -1)
	{
#if _DEBUG
		Error(std::string("Error setting parameter '") + name + "'");
#endif
		return;
	}

	switch (elementSize) {
	case 1:
		glUniform1iv(loc, count, v);
		break;
	case 2:
		glUniform2iv(loc, count, v);
		break;
	case 3:
		glUniform3iv(loc, count, v);
		break;
	case 4:
		glUniform4iv(loc, count, v);
		break;
	}
}


std::vector<std::string> GLSLProgram::GetVertexAttributes()
{
	GLint nAttribs, maxLength, charsWritten, size;
	GLenum type;

	glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &nAttribs);
	glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

	auto currentAttrib = std::unique_ptr<GLchar[]>(new GLchar[maxLength]);

	std::vector<std::string> attribNames(nAttribs);

	for (int i = 0; i < nAttribs; i++)
	{
		glGetActiveAttrib(programHandle, i, maxLength, &charsWritten, 
			&size, &type, currentAttrib.get());

		attribNames.push_back( std::string(currentAttrib.get()) );
	}

	return attribNames;

}

std::vector<std::string> GLSLProgram::GetUniformAttributes() const
{
	GLint nAttribs, maxLength, charsWritten, size;
	GLenum type;

	glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS,&nAttribs);
	glGetProgramiv(programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH,&maxLength);

	auto currentAttrib = std::unique_ptr<GLchar[]>(new GLchar[maxLength]);

	std::vector<std::string> attribNames(nAttribs);

	for (int i = 0; i < nAttribs; i++)
	{
		glGetActiveUniform( programHandle, i, maxLength, &charsWritten, 
			&size, &type, currentAttrib .get());

		attribNames.push_back( std::string(currentAttrib.get()) );
	}

	return attribNames;

}

bool GLSLProgram::BindAttribLocation( GLuint index,  std::string& name)
{
	if(Linked) 
		return false;

	glBindAttribLocation( programHandle, index, name.c_str() );
	return true;

}

bool GLSLProgram::BindFragDataLocation( GLuint colorNumber,  std::string& name)
{
	if(Linked) 
		return false;

	glBindFragDataLocation( programHandle, colorNumber, name.c_str() );
	return true;

}

std::string GLSLProgram::GetInfoLog(GLuint handle) 
{

	GLboolean isShader = glIsShader(handle) ;

	// Compile failed, store log and return false
	int length = 0;
	std::string  logString = "";

	if(GL_TRUE == isShader)
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length );
	else
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &length );

	if(length > 0 ) 
	{
		auto c_log = std::unique_ptr<GLchar[]>(new GLchar[length]);
		int written = 0;

		if(isShader)
			glGetShaderInfoLog(handle, length, &written, c_log.get());
		else
			glGetProgramInfoLog(handle, length, &written, c_log.get());

		logString = c_log.get();
	}

	return logString;
}