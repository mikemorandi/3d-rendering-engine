#include "Texture2D.h"

#include "../error.h"
#include "../texture/ImageUtil.h"

#include <stdexcept>

Texture2DPtr Texture2D::Create(const std::filesystem::path& texturePath)
{
	return std::make_shared<Texture2D>(texturePath);
}

Texture2DPtr Texture2D::Create(int width, int height, Format format)
{
	return std::make_shared<Texture2D>(width, height, format);
}

Texture2DPtr Texture2D::Create(GLuint texHandle)
{
	return std::make_shared<Texture2D>(texHandle);
}

Texture2DPtr Texture2D::Create(int width, int height, const void* data, Format format)
{
	return std::make_shared<Texture2D>(width,height,data,format);
}

const glm::ivec2& Texture2D::Dimensions() const
{
	return dimensions;
}

Texture2D::Texture2D(GLuint texHandle)
	: Texture(GL_TEXTURE_2D, texHandle, Format::RGB)
{
	if (GL_TRUE == glIsTexture(texHandle))
	{
		GLint boundTex = 0;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTex);
		if (static_cast<GLuint>(boundTex) != texHandle)
			glBindTexture(GL_TEXTURE_2D, texHandle);
		
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &dimensions.x);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &dimensions.y);
		
		if (static_cast<GLuint>(boundTex) != texHandle)	
			glBindTexture(GL_TEXTURE_2D, boundTex);
	}
	else
		throw std::runtime_error("Passed texture handle is not valid");
}

Texture2D::Texture2D(int width, int height, Format format)
	: Texture(GL_TEXTURE_2D, format)
	, dimensions( glm::ivec2(width,height))
{
	GLint internalFormat = static_cast<GLint>(format);
	GLenum dataFormat = DataFormat(format);
	GLenum dataType = DataType(format);

	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);
	SetParameters();
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, dataType, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D(int width, int height, const void* data, Format format)
	: Texture(GL_TEXTURE_2D, format)
	, dimensions(glm::ivec2(width, height))
{
	GLint internalFormat = static_cast<GLint>(format);
	GLenum dataFormat = DataFormat(format);

	glGenTextures(1, &texObject);
	glBindTexture(GL_TEXTURE_2D, texObject);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, 8, 8, 0, dataFormat, GL_UNSIGNED_BYTE, data);

	wrapMode = Texture::TextureWrapMode::Repeat;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, static_cast<GLint>(wrapMode));

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D(const std::filesystem::path& texturePath)
: Texture(GL_TEXTURE_2D, Format::RGB)
{
	if (ImageDataPtr imageData = ImageUtil::LoadImage(texturePath))
	{
		if (imageData->components == 4)
			this->textureFormat = Format::RGBA;

		this->dimensions.x = imageData->width;
		this->dimensions.y = imageData->height;

		glGenTextures(1, &texObject);
		glBindTexture(GL_TEXTURE_2D, texObject);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageData->width, imageData->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData->pixelData.get());

		SetParameters();
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else {
		throw std::invalid_argument("Texture could not be loaded from the provided path");
	}
}

void Texture2D::SetParameters()
{	
	//Mip mapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapMode));
}


bool Texture2D::SetData(void* data)
{
	const int border = 0;
	GLint internalFormat = static_cast<GLint>(textureFormat);
	GLenum dataFormat = DataFormat(textureFormat);
	GLenum dataType = DataType(textureFormat);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, dimensions.x, dimensions.y, border, dataFormat, dataType, data);
	return true;
}