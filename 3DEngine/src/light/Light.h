#pragma once

#include <glm/vec3.hpp>
#include "../util/SharedPointer.h"

SHARED_PTR_CLASS_DECL(Light);
SHARED_PTR_CLASS_DECL(Scene);
SHARED_PTR_CLASS_DECL(Shape);

class Scene;

class Light
{
public:

	Light();
	virtual ~Light() = default;

	virtual const glm::vec3& Color();
	virtual void SetColor(const glm::vec3& pos);

	virtual Shape_ptr ModelRepresentation() const;

protected:
	glm::vec3 color;


};