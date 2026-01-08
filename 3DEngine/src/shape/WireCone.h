#pragma once
#include "RenderMesh.h"
#include <memory>

class WireCone;
using WireConePtr = std::shared_ptr<WireCone>;
using WireConeConstPtr = std::shared_ptr<const WireCone>;
using WireConeWeakPtr = std::weak_ptr<WireCone>;

class WireCone : public RenderMesh
{
public:
	static WireConePtr Create(float opening_angle, float height);

	WireCone(float opening_angle, float height);

	~WireCone() {};

	virtual void Init() override;

protected:

	float openingAngle;
	float height;
};

