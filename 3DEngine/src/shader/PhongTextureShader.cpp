#include "stdafx.h"

#include "PhongTextureShader.h"
#include "UniformBuffer.h"

#include "../camera/Camera.h"
#include "../scene/Scene.h"
#include "../light/PointLight.h"
#include "../shape/Skybox.h"
#include "../texture/Texture2D.h"
#include "../texture/CubeMapTexture.h"
#include "../materials/Material.h"

PhongTextureShader::PhongTextureShader()
: PhongShader("phongBumpShader")

{
	GLint maxTexUnits;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,&maxTexUnits);

	textureUnits[Albedo] = 0;
	textureUnits[BumpMap] = 1;
	textureUnits[Specular] = 2;
	textureUnits[Evironment] = 3;

	hasShadows = true;
	pcfShadows = true;

	hasMM = true;
}

PhongTextureShader::~PhongTextureShader()
{

}

bool PhongTextureShader::Use(const Scene_ptr& scene, const glm::mat4& modelTransform)
{	
	bool ok = ShaderBase::Use(scene, modelTransform);

	if (scene->skybox)
	{
		if (auto sbm = std::dynamic_pointer_cast<SkyboxMaterial>(scene->skybox->Material()))
		{
			sbm->texture->BindTexture(textureUnits[Evironment]);
			SetUniform("EnvMap.Exists", static_cast<bool>(scene->skybox));
			SetUniform("EnvMap.CubeTexture", textureUnits[Evironment]);
			SetUniform("CameraPosWorld", scene->activeCamera->CameraFrustum().position);
		}
	}

	SetLightAndModel(scene, static_cast<int>(textureUnits.size() ));

	if (textureMaterial)
	{
// 		SetUniform("Material.AmbientReflectivity", textureMaterial->ambientReflection);
// 		SetUniform("Material.DiffuseReflectivity", textureMaterial->diffuseReflection);
		SetUniform("Material.SpecularReflectivity", textureMaterial->glossyReflection);
		SetUniform("Material.Shininess", textureMaterial->shininess);
/*		SetUniform("Material.Opacity", textureMaterial->opacity);*/

		//Bind albedo texture to texture unit 0
		if (auto at = textureMaterial->albedoTexture)
		{
			at->BindTexture(textureUnits[Albedo]);
			SetUniform("Material.AlbedoTex", textureUnits[Albedo]);
		}

		bool hasBumpMap = false;
		bool hasSpecularMap = false;

		if (auto bt = textureMaterial->bumpTexture)
		{
			//Bind normal texture to texture unit 1
			bt->BindTexture(textureUnits[BumpMap]);
			SetUniform("Material.BumpmapTex", textureUnits[BumpMap]);
			SetUniform("Material.BumpTexIsNormalMap", textureMaterial->bumpBumpTexIsNormalMap);
			hasBumpMap = true;			
		}

		if (auto st = textureMaterial->specularTexture)
		{
			//Bind specular texture to texture unit 2
			st->BindTexture(textureUnits[Specular]);
			SetUniform("Material.SpecularTex", textureUnits[Specular]);
			hasSpecularMap = true;
		}

		SetUniform("Material.HasBumpMap", hasBumpMap);
		SetUniform("Material.HasSpecularMap", hasSpecularMap);
	}
	else
		ok = false;
	
	return ok;
}


void PhongTextureShader::UnUse()
{
	ShaderBase::UnUse();

	if (textureMaterial)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnits[Albedo]);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0 + textureUnits[BumpMap]);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0 + textureUnits[Specular]);
		glBindTexture(GL_TEXTURE_2D, 0);	
	}
}


bool PhongTextureShader::SetMaterial(const Material_cptr& material)
{
	if (TextureMaterial_cptr texm = std::dynamic_pointer_cast<const TextureMaterial>(material))
	{
		this->textureMaterial = texm;
		return true;
	}

	return false;
}

VertexAttributeInfo_ptr PhongTextureShader::GetVertexAttributeInfo() const
{
	VertexAttributeInfo_ptr vai = PhongShader::GetVertexAttributeInfo();

	if (!textureMaterial || (textureMaterial && !textureMaterial->bumpTexture) )
		vai->mapping[GLSLShader::Tangent] = -1;

	return vai;
}