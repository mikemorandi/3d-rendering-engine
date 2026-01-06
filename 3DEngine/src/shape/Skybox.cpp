#include "Skybox.h"
#include "../texture/CubeMapTexture.h"
#include "../materials/Material.h"

#include <glm/gtc/matrix_transform.hpp>

Skybox::Skybox(const SkyboxMaterial_ptr& material) 
	: Box()
{
	SetWorldTransform(glm::scale(glm::mat4(1.0),glm::vec3(50,50,50)));
	this->material = material;

	name = "SkyBox";
}

Skybox::~Skybox()
{
	
}
