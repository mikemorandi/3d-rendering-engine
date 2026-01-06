
#include "Texture.h"

#include "../error.h"
#include "../util/Util.h"


Texture::Texture(GLenum texTarget, Format format)
: Texture(texTarget,0,format)
{

}

Texture::Texture(GLenum texTarget, GLuint texHandle, Format format)
: texObject(texHandle)
, target(texTarget)
, textureFormat(format)
, wrapMode(TextureWrapMode::Repeat)
{

}

Texture::~Texture()
{
	if (glIsTexture(texObject))
		glDeleteTextures(1, &texObject);
}

void Texture::BindTexture(int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(target, texObject);
}

bool Texture::IsValid() const
{
	return glIsTexture(texObject) == GL_TRUE;
}

GLenum Texture::DataFormat(Format format) const
{
	switch (format)
	{
		case Format::Depth: 
			return GL_DEPTH_COMPONENT;
		case Format::RED: 
			return GL_RED;
		default: return GL_RGBA;
	}
}

GLenum Texture::DataType(Format format) const
{
	switch (format)
	{
		case Format::Depth: 
			return GL_FLOAT;
		case Format::RGBA32F: 
			return GL_FLOAT;
		default: return GL_UNSIGNED_BYTE;
	}
}

Texture::Format Texture::TextureFormat() const
{
	return textureFormat;
}