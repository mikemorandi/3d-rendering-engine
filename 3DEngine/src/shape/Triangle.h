#pragma once

#include "RenderMesh.h"
#include "../util/SharedPointer.h"
#include <memory>

class Triangle;
using TrianglePtr = std::shared_ptr<Triangle>;
using TriangleConstPtr = std::shared_ptr<const Triangle>;
using TriangleWeakPtr = std::weak_ptr<Triangle>;

class Triangle : public RenderMesh
{
public:

	SHARED_PTR_FACTORY(Triangle);

	virtual void Init();

	Triangle();
};

