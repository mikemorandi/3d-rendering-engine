#pragma once
#include "RenderMesh.h"
#include "../util/SharedPointer.h"
#include <memory>

class Box;
using BoxPtr = std::shared_ptr<Box>;
using BoxConstPtr = std::shared_ptr<const Box>;
using BoxWeakPtr = std::weak_ptr<Box>;

class Box : public RenderMesh
{
public:
	SHARED_PTR_FACTORY(Box);

	virtual void Init();

	Box();
};

