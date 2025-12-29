#pragma once

#include "Texture.h"

#include <string>
#include <filesystem>
#include <glm/vec2.hpp>


SHARED_PTR_CLASS_DECL(Texture2D);

class Texture2D : public Texture
{
public:

	static Texture2D_ptr Create(const std::filesystem::path& texturePath);

	static Texture2D_ptr Create(int width, int height, Format format = Format::RGBA);

	static Texture2D_ptr Create(int width, int height, const void* data, Format format = Format::RGBA);

	Texture2D(const std::filesystem::path& texturePath);

	Texture2D(int width, int height, Format format);

	Texture2D(int width, int height, const void* data, Format format);

	Texture2D(GLuint texHandle);

	/// Creates a new texture with the passed texture id
	/// The OpenGL texture is deleted when this instance is destroyed
	static Texture2D_ptr Create(GLuint textHandle);

	const glm::ivec2& Dimensions() const;

	virtual bool SetData(void* data) override;

protected:

	void SetParameters();

	glm::ivec2 dimensions;
};

