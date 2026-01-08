#pragma once

#include <memory>

#include <tinyxml2.h>
#include <map>
#include <string>
#include <glm/fwd.hpp>

class InputHandlerFactory;

class Camera;
using CameraPtr = std::shared_ptr<Camera>;
using CameraConstPtr = std::shared_ptr<const Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;
class Scene;
using ScenePtr = std::shared_ptr<Scene>;
using SceneConstPtr = std::shared_ptr<const Scene>;
using SceneWeakPtr = std::weak_ptr<Scene>;
class Material;
using MaterialPtr = std::shared_ptr<Material>;
using MaterialConstPtr = std::shared_ptr<const Material>;
using MaterialWeakPtr = std::weak_ptr<Material>;

class SceneParser
{
public:
	SceneParser();

	[[nodiscard]] bool Parse(const std::string& xml_document);
	ScenePtr Scene();

protected:

	[[nodiscard]] inline bool GetIntAttrib(tinyxml2::XMLElement* element, const char* attrib_name, int& value);
	[[nodiscard]] inline bool GetBoolAttrib(tinyxml2::XMLElement* element, const char* attrib_name, bool& value);
	[[nodiscard]] inline bool GetFloatAttrib(tinyxml2::XMLElement* element, const char* attrib_name, float& value);
	inline bool GetVector3(tinyxml2::XMLElement* element, glm::vec3& vec);
	inline bool GetColorVector3(tinyxml2::XMLElement* element, glm::vec3& vec);

	bool ParseMaterials(tinyxml2::XMLElement* material_element);
	bool ParseObjects(tinyxml2::XMLElement* material_element);
	bool ParseSkybox(tinyxml2::XMLElement* skybox);
	bool ParseCamera(CameraPtr& cam, tinyxml2::XMLElement* material_element);
	bool ParseLights(tinyxml2::XMLElement* material_element);
	bool ParseTransforms(glm::mat4& tMatrix, tinyxml2::XMLElement* transform_elem);

	std::map<std::string, MaterialPtr> materials;
	ScenePtr generated_scene;
};

