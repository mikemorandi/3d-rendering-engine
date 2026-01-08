#pragma once

#include "Texture.h"
#include <memory>

#include <string>
#include <filesystem>
#include <glm/vec2.hpp>


class Texture2D;
using Texture2DPtr = std::shared_ptr<Texture2D>;
using Texture2DConstPtr = std::shared_ptr<const Texture2D>;
using Texture2DWeakPtr = std::weak_ptr<Texture2D>;

class Texture2D : public Texture
{
public:

	static Texture2DPtr Create(const std::filesystem::path& texturePath);

	static Texture2DPtr Create(int width, int height, Format format = Format::RGBA);

	static Texture2DPtr Create(int width, int height, const void* data, Format format = Format::RGBA);

	Texture2D(const std::filesystem::path& texturePath);

	Texture2D(int width, int height, Format format);

	Texture2D(int width, int height, const void* data, Format format);

	Texture2D(GLuint texHandle);

	/// Creates a new texture with the passed texture id
	/// The OpenGL texture is deleted when this instance is destroyed
	static Texture2DPtr Create(GLuint textHandle);

	const glm::ivec2& Dimensions() const;

	virtual bool SetData(void* data) override;

protected:

	void SetParameters();

	glm::ivec2 dimensions;
};

