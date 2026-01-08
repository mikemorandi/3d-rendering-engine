#pragma once

#include "Texture.h"
#include <memory>

#include <string>
#include <glm/vec3.hpp>



class Texture3D;
using Texture3DPtr = std::shared_ptr<Texture3D>;
using Texture3DConstPtr = std::shared_ptr<const Texture3D>;
using Texture3DWeakPtr = std::weak_ptr<Texture3D>;

class Texture3D : public Texture
{
public:

	static Texture3DPtr Create(int width, int height, int depth, Format format = Format::RGBA);

	static Texture3DPtr Create(int width, int height, int depth, void* data, Format format = Format::RGBA);

	Texture3D(int width, int height, int depth, Format format, void* data);

	const glm::ivec3& Dimensions() const;

	virtual bool SetData(void* data) override;

protected:

	glm::ivec3 dimensions;

};

