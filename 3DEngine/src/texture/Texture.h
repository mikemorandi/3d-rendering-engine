#pragma once

#include <memory>
#include "../core/gl.h"

class Texture;
using TexturePtr = std::shared_ptr<Texture>;
using TextureConstPtr = std::shared_ptr<const Texture>;
using TextureWeakPtr = std::weak_ptr<Texture>;

class Texture 
{
public:

	enum class Format : unsigned int
	{ 
		RED = static_cast<unsigned int>(GL_RED),
		RGB = static_cast<unsigned int>(GL_RGB8),
		RGBA = static_cast<unsigned int>(GL_RGBA8),
		RGBA32F = static_cast<unsigned int>(GL_RGBA32F), /// < 4 components, 32-bit floating point
		Depth = static_cast<unsigned int>(GL_DEPTH_COMPONENT)
	};

	enum class TextureWrapMode : unsigned int
	{
		ClampToEdge = static_cast<unsigned int>(GL_CLAMP_TO_EDGE),
		ClampToBorder = static_cast<unsigned int>(GL_CLAMP_TO_BORDER),
		Repeat = static_cast<unsigned int>(GL_REPEAT),
		MirroredRepeat = static_cast<unsigned int>(GL_MIRRORED_REPEAT),
	};

	virtual ~Texture();

	void BindTexture(int textureUnit);

	/// Returns true, if the texture is a valid OpenGL Texture2D
	[[nodiscard]] bool IsValid() const;

	[[nodiscard]] Format TextureFormat() const;

	[[nodiscard]] GLuint Handle() const { return texObject; };

	/// Sets data for this texture (must be bound). Make sure data is large enough for the textures dimensions
	[[nodiscard]] virtual bool SetData(void* data) = 0;


protected:

	Texture(GLenum texTarget, GLuint texHandle, Format format);

	Texture(GLenum texTarget, Format format);

	GLenum DataFormat(Format format) const;

	GLenum DataType(Format format) const;

	GLuint texObject;

	GLenum target;

	Format textureFormat;

	TextureWrapMode wrapMode;
};

