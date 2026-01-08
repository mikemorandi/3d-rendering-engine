#pragma once

#include "Shape.h"
#include <memory>

class ShapeInstance;
using ShapeInstancePtr = std::shared_ptr<ShapeInstance>;
using ShapeInstanceConstPtr = std::shared_ptr<const ShapeInstance>;
using ShapeInstanceWeakPtr = std::weak_ptr<ShapeInstance>;

class ShapeInstance : public Shape
{
public:

	static ShapeInstancePtr Create(const ShapePtr& shape);

	ShapeInstance(const ShapePtr& shape);

	virtual void Init() override { } ;

	virtual void Render(const ScenePtr& scene) const override;

	virtual AABBox BoundingBox() const override;

private:
	
	ShapePtr base_shape;	
};

