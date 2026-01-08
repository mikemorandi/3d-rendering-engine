#pragma once
#include "RenderMesh.h"
#include "../util/SharedPointer.h"
#include <memory>

class WireCube;
using WireCubePtr = std::shared_ptr<WireCube>;
using WireCubeConstPtr = std::shared_ptr<const WireCube>;
using WireCubeWeakPtr = std::weak_ptr<WireCube>;

class WireCube : public RenderMesh
{
public:
	SHARED_PTR_FACTORY(WireCube);

	virtual void Init() override;

	WireCube();
};

