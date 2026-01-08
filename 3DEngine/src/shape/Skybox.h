#pragma once
#include "Box.h"
#include <memory>
#include <string>

class Skybox;
using SkyboxPtr = std::shared_ptr<Skybox>;
using SkyboxConstPtr = std::shared_ptr<const Skybox>;
using SkyboxWeakPtr = std::weak_ptr<Skybox>;
class SkyboxMaterial;
using SkyboxMaterialPtr = std::shared_ptr<SkyboxMaterial>;
using SkyboxMaterialConstPtr = std::shared_ptr<const SkyboxMaterial>;
using SkyboxMaterialWeakPtr = std::weak_ptr<SkyboxMaterial>;

class Skybox : public Box
{
public:

	Skybox(const SkyboxMaterialPtr& material);
	virtual ~Skybox();

};

