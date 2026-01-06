#include "SceneParser.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <filesystem>

#include "../error.h"

#include "../shader/IntrinsicColorShader.h"
#include "../shader/SkyboxShader.h"
#include "../shader/ConstColorShader.h"
#include "../shader/SHDiffuseShader.h"
#include "../shader/ShaderLibrary.h"

#include "../texture/Texture2D.h"
#include "../texture/CubeMapTexture.h"

#include "../materials/Material.h"

#include "../util/Util.h"
#include "../util/RawMesh.h"
#include "../util/ShCoeffParser.h"

#include "../camera/PerspectiveCamera.h"
#include "../scene/Scene.h"

#include "../shape/RenderMesh.h"
#include "../shape/ShapeInstance.h"
#include "../shape/Box.h"
#include "../shape/Triangle.h"
#include "../shape/Skybox.h"

#include "../light/AmbientLight.h"
#include "../light/SpotLight.h"
#include "../light/DirectionalLight.h"
#include "../config.h"

using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;
using tinyxml2::XMLNode;
using std::string;
using std::vector;

typedef std::pair<string,Material_ptr> ShaderKeyVal;

namespace fs = std::filesystem;

SceneParser::SceneParser()
{

}

Scene_ptr SceneParser::Scene()
{
	return generated_scene;
}

bool SceneParser::Parse(const std::string& xmlDocument)
{
	bool parseOk = true;
	
	tinyxml2::XMLDocument doc;
	if(doc.Parse(xmlDocument.c_str()) == tinyxml2::XML_SUCCESS)
	{	
		XMLElement* root = doc.RootElement();

		if( strcmp( root->Name(), "scene" ) == 0 )
		{
			string sceneName(root->Attribute("name"));
			std::cout << "Parsing scene \"" << sceneName << "\":" << std::endl;

			//Camera
			XMLElement* cameraElement;

			if((cameraElement = root->FirstChildElement("camera")))
			{
				Camera_ptr cam;
				parseOk &= ParseCamera(cam,cameraElement);
				try
				{
					generated_scene = Scene::Create(cam);
					generated_scene->name = sceneName;
				}
				catch (std::exception* e)
				{
					Error(e->what());
					return false;
				}		
			}
			else
			{
				return false;
			}

			//Materials
			XMLElement* materialsElement = root->FirstChildElement("materials");
			if( cameraElement != nullptr) 
			{
				if( ! ParseMaterials(materialsElement) )
					return false;
			}

			//Objects
			if (XMLElement* objectsElement = root->FirstChildElement("objects"))
			{
				if (!ParseObjects(objectsElement))
					return false;
			}

			//Lights
			XMLElement* lightsElement = root->FirstChildElement("lights");
			if( lightsElement != nullptr)
			{
				if( ! ParseLights(lightsElement) )
					return false;
			}

			//Skybox
			XMLElement* skyboxElement = root->FirstChildElement("skybox");
			if( skyboxElement != nullptr)
			{
				if( ! ParseSkybox(skyboxElement) )
					return false;
			}
		}
		else 
		{
			Error("Not a valid scene file");
			parseOk = false;
		}
	}
	else
	{
		Error("File could not be parsed" );
		parseOk = false;
	}

	return parseOk;
}

bool SceneParser::ParseMaterials(XMLElement* materialsGroupElement)
{	
	if (XMLElement* materialElement = materialsGroupElement->FirstChildElement("material"))
	{
		ShaderLibrary_ptr sl = ShaderLibrary::Instance();

		do
		{
			string materialType = materialElement->Attribute("shader");
			string materialName = materialElement->Attribute("name");

			Material_ptr material;

			if (materialType == "phongOld")
			{
				// Legacy format - convert vec3 reflection values to scalar coefficients
				PhongMaterial_ptr phongMat = PhongMaterial::Create();
				XMLElement* subElem;

				if ((subElem = materialElement->FirstChildElement("ambientReflect")))
				{
					glm::vec3 ambient;
					GetColorVector3(subElem, ambient);
					phongMat->ambientReflect = (ambient.r + ambient.g + ambient.b) / 3.0f;
				}

				if ((subElem = materialElement->FirstChildElement("diffuseReflect")))
				{
					glm::vec3 diffuse;
					GetColorVector3(subElem, diffuse);
					phongMat->diffuseReflect = (diffuse.r + diffuse.g + diffuse.b) / 3.0f;
					phongMat->color = diffuse; // Use diffuse as base color
				}

				if ((subElem = materialElement->FirstChildElement("glossyReflect")))
				{
					glm::vec3 glossy;
					GetColorVector3(subElem, glossy);
					phongMat->glossyReflect = (glossy.r + glossy.g + glossy.b) / 3.0f;
				}

				if ((subElem = materialElement->FirstChildElement("shininess")))
				{
					int shininessInt;
					GetIntAttrib(subElem, "value", shininessInt);
					phongMat->shininess = static_cast<float>(shininessInt);
				}

				material = phongMat;
			}
			else if (materialType == "color")
			{
				material = IntrinsicColorMaterial::Create();
			}
			else if (materialType == "const")
			{
				ConstantColorMaterial_ptr mat = ConstantColorMaterial::Create();

				XMLElement* subElem;
				if ((subElem = materialElement->FirstChildElement("color")))
					GetColorVector3(subElem, mat->color);

				material = mat;
			}
			else if (materialType == "phong")
			{
				PhongMaterial_ptr mat;
				XMLElement* subElem;

				// Check if this material has textures - if so, create TextureMaterial
				if ((subElem = materialElement->FirstChildElement("texture")))
				{
					TextureMaterial_ptr texMat = TextureMaterial::Create();

					// Load albedo/diffuse texture
					fs::path texFile(subElem->Attribute("file"));
					texMat->albedoTexture = Texture2D::Create(Config::TEXTURE_BASE_PATH / texFile);

					// Load bump/normal map if specified
					if (XMLElement* bumpElem = materialElement->FirstChildElement("bumpMap"))
					{
						fs::path bumpMapFile(bumpElem->Attribute("file"));
						const char* typeAttr = bumpElem->Attribute("type");
						std::string type = typeAttr ? typeAttr : "bump";

						texMat->bumpTexture = Texture2D::Create(Config::TEXTURE_BASE_PATH / bumpMapFile);
						texMat->bumpBumpTexIsNormalMap = (type == "normal");
					}

					// Load specular map if specified
					if (XMLElement* specElem = materialElement->FirstChildElement("specularMap"))
					{
						fs::path specMapFile(specElem->Attribute("file"));
						texMat->specularTexture = Texture2D::Create(Config::TEXTURE_BASE_PATH / specMapFile);
					}

					mat = texMat;
				}
				else
				{
					mat = PhongMaterial::Create();
				}

				// Load common phong material properties
				if ((subElem = materialElement->FirstChildElement("color")))
					GetColorVector3(subElem, mat->color);

				if ((subElem = materialElement->FirstChildElement("ambientReflect")))
				{
					glm::vec3 ambient;
					GetColorVector3(subElem, ambient);
					mat->ambientReflect = (ambient.r + ambient.g + ambient.b) / 3.0f;
				}

				if ((subElem = materialElement->FirstChildElement("diffuseReflect")))
				{
					glm::vec3 diffuse;
					GetColorVector3(subElem, diffuse);
					mat->diffuseReflect = (diffuse.r + diffuse.g + diffuse.b) / 3.0f;
				}

				if ((subElem = materialElement->FirstChildElement("glossyReflect")))
				{
					glm::vec3 glossy;
					GetColorVector3(subElem, glossy);
					mat->glossyReflect = (glossy.r + glossy.g + glossy.b) / 3.0f;
				}

				if ((subElem = materialElement->FirstChildElement("shininess")))
					GetFloatAttrib(subElem, "value", mat->shininess);

				if ((subElem = materialElement->FirstChildElement("opacity")))
					GetFloatAttrib(subElem, "value", mat->opacity);

				material = mat;
			}
			else if (materialType == "diffuseSH")
			{
				auto shMaterial = ShDiffuseMaterial::Create();

				bool parse_ok = false;

				XMLElement* subElem;
				if ((subElem = materialElement->FirstChildElement("shcoeffs")))
				{
					if (auto shfile = subElem->Attribute("file"))
					{
						auto data = Util::LoadTextFile(Config::DATA_BASE_PATH / fs::path("sh") / fs::path(shfile));
						shMaterial->shCoeffs = ShCoeffParser::Parse(data);
						material = shMaterial;
						parse_ok = true;
					}
				}

				if (!parse_ok)
				{
					Error("Could not load SH shader params");
				}

			}
			else
			{
				Error("Material " + materialType + " not supported");
				return false;
			}

			material->SetName(materialName);
			materials.insert(ShaderKeyVal(materialElement->Attribute("name"), material));

		} while ((materialElement = materialElement->NextSiblingElement("material")));
	}
	

	return true;
}

bool SceneParser::ParseSkybox(XMLElement* skyboxElem)	
{
	CubeMapTexture_ptr texture;

	const char* path;
	if((path = skyboxElem->Attribute("cubeMapFile")))
	{
		//Single file cube map
		texture.reset(new CubeMapTexture(Config::TEXTURE_BASE_PATH / fs::path(path)));
	}
	else if((path = skyboxElem->Attribute("cubeMapFolder")))
	{
		//Cube map consisting of 6 files
		string type = skyboxElem->Attribute("type");
		texture.reset(new CubeMapTexture(Config::TEXTURE_BASE_PATH / path, type));
	}
	else
	{
		return false;
	}

	SkyboxMaterial_ptr sbMat = SkyboxMaterial::Create();
	sbMat->texture = texture;

	Skybox_ptr sb = Skybox_ptr(new Skybox(sbMat));
	sb->Init();
	generated_scene->SetSkybox(sb);

	return true;
}

bool SceneParser::ParseObjects(XMLElement* objects)
{
	//Parse children of 'objects'
	if(XMLElement* objeElem = objects->FirstChildElement())
	{
		std::vector<Shape_ptr> shapes;
		std::map<std::string, Shape_ptr> instanceableShapes;		

		do
		{
			string type = objeElem->Name();
			Shape_ptr shape;
			Material_ptr material;			

			if (const char* materialName = objeElem->Attribute("material"))
			{
				if (materials[materialName])
					material = materials[materialName];
				else
					Warn("The specified material " + string(materialName) + " is not defined");
			}

			bool needs_tangents = false;
			if (auto tex_mat = std::dynamic_pointer_cast<TextureMaterial>(material))
			{
				needs_tangents = tex_mat->bumpTexture && tex_mat->bumpBumpTexIsNormalMap;
			}

			if(type == "mesh")
			{
				fs::path file = objeElem->Attribute("file");

				fs::path modelPath = Config::MODELS_BASE_PATH / file;
				shape = Util::LoadModel(modelPath, needs_tangents);
				if (!shape)
					return false;
			}
			else if(type == "box")
			{
				shape = Box::Create();
			}
			else if (type == "triangle")
			{
				shape = Triangle::Create();
			}
			else if (type == "instance")
			{
				if (const char* templName = objeElem->Attribute("template"))
				{
					auto template_shape = instanceableShapes[templName];
					if (template_shape)					
						shape = ShapeInstance::Create(template_shape);					
				}
			}
			else 
			{
				Error("Object of type " + type + " not supported");
			}

			if(shape)	
			{
				shape->Init();

				//See if a material is specified, if yes mtllib overrides when dealing with a mesh
				if (material && shape->Material())
					Warn("The shape already has a material assigned, ignoring specified material");			
				else			
					shape->SetMaterial(material);			

				// Parse transform node
				if(XMLElement* transformsElem = objeElem->FirstChildElement("transform"))
				{
					glm::mat4 tMatrix(1.f);
					ParseTransforms(tMatrix,transformsElem);
					shape->SetWorldTransform(tMatrix);
				}

				bool instanceable = false;
				if (GetBoolAttrib(objeElem, "instanceable", instanceable) && instanceable)
				{
					if (const char* templName = objeElem->Attribute("template"))				
						instanceableShapes[templName] = shape;
				}
				else
					shapes.push_back(shape);	
			}

		} 
		while ((objeElem = objeElem->NextSiblingElement()));

		generated_scene->AddShapes(shapes);
	}
	
	return true;
}

bool SceneParser::ParseTransforms(glm::mat4& tMatrix, tinyxml2::XMLElement* transformElem)
{
	bool success = true;
	
	if (XMLElement* transform = transformElem->FirstChildElement())
	{
		do
		{
			std::string type = transform->Name();

			if (type == "translate")
			{
				glm::vec3 transl(1.0f);
				if (GetVector3(transform, transl))
				{
					tMatrix = glm::translate(tMatrix, transl);
				}
				else
				{
					Error("Could not parse translation");
					success = false;
				}
			}
			else if (type == "rotate")
			{
				glm::vec3 axis(1.0f);
				float angle;
				if (GetVector3(transform, axis) && GetFloatAttrib(transform, "angle", angle))
				{
					tMatrix = glm::rotate(tMatrix, glm::radians(angle), axis);
				}
				else
				{
					Error("Could not parse rotation");
					success = false;
				}
			}
			else if (type == "scale")
			{
				glm::vec3 factors(1.0f);
				if (GetVector3(transform, factors))
				{
					tMatrix = glm::scale(tMatrix, factors);
				}
				else
				{
					Error("Could not parse scale");
					success = false;
				}
			}

		} while ((transform = transform->NextSiblingElement()));
	}
	else
		success = false;

	return success;

}

bool SceneParser::ParseCamera(Camera_ptr& cam, tinyxml2::XMLElement* camElement)
{
	bool success = true;
	std::string type = camElement->Attribute("type");

	if(type == "perspective")
	{
		float fov;

		if( ! ( success &= GetFloatAttrib(camElement,"fov",fov) )  )
			return false;

		cam.reset(new PerspectiveCamera(fov));
	}
	else //unknown camera type
	{
		Error("Camera type " + type + " not supported");
		return false;
	}

	Frustum& frustrum = cam->CameraFrustum();

	glm::vec3 pos, target, up;
	success &= GetVector3( camElement->FirstChildElement("position"),pos);
	success &= GetVector3( camElement->FirstChildElement("target"),target);
	success &= GetVector3( camElement->FirstChildElement("up"),up);

	frustrum.position = pos;
	frustrum.frame.ViewDir() = glm::normalize(target - frustrum.position);
	frustrum.frame.Up() = up;
	frustrum.frame.Side() = glm::normalize(glm::cross(frustrum.frame.ViewDir(),up));
	cam->UpdateViewMatrix();

	return success;

}

bool SceneParser::ParseLights(tinyxml2::XMLElement* lightsGroupElement)
{
	if(XMLElement* lightElem = lightsGroupElement->FirstChildElement("light"))
	{
		do
		{
			string lightType = lightElem->Attribute("type");

			if (lightType == "point")
			{
				PointLight_ptr plight = PointLight::Create();
				glm::vec3 pos, color;

				GetVector3(lightElem->FirstChildElement("position"), pos);
				GetColorVector3(lightElem->FirstChildElement("color"), color);

				plight->SetPosition(glm::vec4(pos, 1.0));
				plight->SetColor(color);

				generated_scene->AddLight(plight);

			}
			else if (lightType == "spot")
			{

				glm::vec3 pos, color, direction;
				float cutoff, exponent = 5;
				GetVector3(lightElem->FirstChildElement("position"), pos);
				GetVector3(lightElem->FirstChildElement("direction"), direction);
				GetColorVector3(lightElem->FirstChildElement("color"), color);

				GetFloatAttrib(lightElem, "cutoff", cutoff);
				GetFloatAttrib(lightElem, "exponent", exponent);
				bool animated;
				GetBoolAttrib(lightElem, "animated", animated);
				SpotLight_ptr slight = SpotLight::Create(direction, cutoff, exponent);

				slight->SetPosition(glm::vec4(pos, 1.0));
				slight->SetColor(color);
				slight->SetAnimated(animated);

				generated_scene->AddLight(slight);
			}
			else if (lightType == "directional")
			{
				if (generated_scene->lightModel->directionalLight)
					Warn("Multiple directional lights, in the scene. Only one is suported");

				glm::vec3 color, direction;
				GetColorVector3(lightElem->FirstChildElement("color"), color);
				GetVector3(lightElem->FirstChildElement("direction"), direction);

				DirectionalLight_ptr dirLight = DirectionalLight::Create(glm::normalize(direction));
				dirLight->SetColor(color);
				generated_scene->SetLight(dirLight);

			}
			else if (lightType == "ambient")
			{
				glm::vec3 color(1.0f);
				GetColorVector3(lightElem->FirstChildElement("color"), color);

				AmbientLight_ptr ambLight = AmbientLight::Create();
				ambLight->SetColor(color);
				generated_scene->SetLight(ambLight);				
			}
			else
			{
				Error("Light " + lightType + " not supported");
				return false;
			}
		} while ((lightElem = lightElem->NextSiblingElement("light")));
	}
	
	return true;
}

bool SceneParser::GetVector3(XMLElement* element, glm::vec3& vec)
{
	if(element == nullptr)
		return false;
	
	bool success = true;

	success &= GetFloatAttrib(element,"x",vec.x);
	success &= GetFloatAttrib(element,"y",vec.y);
	success &= GetFloatAttrib(element,"z",vec.z);

	return success;
}

bool SceneParser::GetColorVector3(XMLElement* element, glm::vec3& vec)
{
	if(element == nullptr)
		return false;
	
	bool success = true;

	success &= GetFloatAttrib(element,"r",vec.x);
	success &= GetFloatAttrib(element,"g",vec.y);
	success &= GetFloatAttrib(element,"b",vec.z);

	return success;
}

bool SceneParser::GetFloatAttrib(XMLElement* element, const char* attribName, float& value)
{
	if(element == nullptr)
		return false;

	const char* attrValue = element->Attribute(attribName);
	if(attrValue == nullptr)
		return false;

	std::istringstream isstr(attrValue);
	return !(isstr >> value).fail();
}

bool SceneParser::GetIntAttrib(XMLElement* element, const char* attribName, int& value)
{
	if(element == nullptr)
		return false;

	const char* attrValue = element->Attribute(attribName);
	if(attrValue == nullptr)
		return false;

	std::istringstream isstr(attrValue);
	return !(isstr >> value).fail();
}

bool SceneParser::GetBoolAttrib(XMLElement* element, const char* attrib_name, bool& value)
{
	bool ok = false;

	if (element != nullptr)
	{
		if (auto attr = element->Attribute(attrib_name))
		{
			if (std::string(attr).compare("true") == 0)
			{ 
				value = true;
				ok = true;
			}
				
			else if (std::string(attr).compare("false") == 0)
			{
				value = false;
				ok = true;
			}
		}
	}

	return ok;
}
