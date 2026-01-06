#pragma once

#include "Shape.h"
#include "../util/SharedPointer.h"

SHARED_PTR_CLASS_DECL(ShapeInstance);

class ShapeInstance : public Shape
{
public:

	static ShapeInstance_ptr Create(const Shape_ptr& shape);

	ShapeInstance(const Shape_ptr& shape);

	virtual void Init() override { } ;

	virtual void Render(const Scene_ptr& scene) const override;

	virtual AABBox BoundingBox() const override;

private:
	
	Shape_ptr base_shape;	
};

