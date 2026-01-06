#include "Shape.h"


Shape::Shape()
	: worldTransform(glm::mat4(1.f))
	, boundingBoxDirty(true)
	, castsShadows(true)  // By default, shapes cast shadows
{

}

Shape::Shape(const Shape& rhs)
	: Shape()
{
	worldTransform = rhs.worldTransform;
	bboxModelSpace = rhs.bboxModelSpace;
	material = rhs.material;
	castsShadows = rhs.castsShadows;
}

Shape::~Shape()
{
}

AABBox Shape::BoundingBox() const
{
	if (boundingBoxDirty)
	{
		cachedWorldBBox = (worldTransform * bboxModelSpace);
		boundingBoxDirty = false;
	}
	return cachedWorldBBox;
}

void Shape::SetWorldTransform(const glm::mat4 & t)
{
	worldTransform = t;
	boundingBoxDirty = true;
}

const glm::mat4 & Shape::WorldTransform() const
{
	return worldTransform;
}
