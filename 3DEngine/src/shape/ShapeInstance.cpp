#include "ShapeInstance.h"

ShapeInstancePtr ShapeInstance::Create(const ShapePtr & shape)
{
	return std::make_shared<ShapeInstance>(shape);
}

void ShapeInstance::Render(const ScenePtr & scene) const
{
	base_shape->SetWorldTransform(worldTransform);
	base_shape->Render(scene);
}

AABBox ShapeInstance::BoundingBox() const
{
	return base_shape->BoundingBox();
}

ShapeInstance::ShapeInstance(const ShapePtr& shape)
{
	base_shape = shape;
	SetMaterial(shape->Material());
}
