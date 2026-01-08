
#include "Scene.h"

#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "../util/Util.h"
#include "../util/TimeManager.h"

#include "../input/WindowEventHandler.h"

#include "../input/InputHandler.h"
#include "../camera/InspectionCameraAdapter.h"
#include "../camera/FirstPersonCameraAdapter.h"

#include "../camera/Camera.h"
#include "../shader/ShaderBase.h"
#include "../shape/Shape.h"
#include "../shape/Skybox.h"
#include "../shape/WireCube.h"
#include "../light/LightModel.h"
#include "../light/PointLight.h"
#include "../light/DirectionalLight.h"
#include "../light/SpotLight.h"
#include "../texture/Framebuffer.h"
#include "../texture/DepthTexture.h"

#include "../rendering/Viewport.h"

#include "../materials/Material.h"

#include "../error.h"

using std::vector;

ScenePtr Scene::Create(const CameraPtr& cam, bool has_frambufer)
{
	auto ptr = std::make_shared<Scene>(cam, has_frambufer);
	cam->AddObserver(ptr);
	return ptr;
}

Scene::Scene(const CameraPtr& cam,bool has_frambufer)
	: useInspectionMode(false)
	, lastMousePosition(0.0f, 0.0f)
	, renderLightRepresentation(false)
	, renderBoundingBoxes(false)
{

	if (has_frambufer)
		framebuffer = Framebuffer::Create();

	activeCamera = cam;

	lightModel = std::make_shared<LightModel>();

	if(!lightModel->IsValid())
	{
		throw std::runtime_error("Could not create light model");
	}

	// Create both camera adapters
	inspectionCamAdapter = std::make_shared<InspectionCameraAdapter>(cam);
	fpCamAdapter = std::make_shared<FirstPersonCameraAdapter>(cam);

	// Register first person camera adapter for time updates (smooth movement)
	TimeManager::Instance().AddTimeObserver(fpCamAdapter);

	WindowEventHandler& winEventHandler = WindowEventHandler::Instance();
	winEventHandler.AddViewportObserver(cam);

	lightAnimParams[0].radiansPerInterval = glm::radians(0.4f);
	lightAnimParams[1].radiansPerInterval = glm::radians(0.7f);
	lightAnimParams[2].radiansPerInterval = glm::radians(0.6f);

	lightAnimParams[0].rotationAxis = glm::vec3(0.0f, 1.0f, 0.0f);
	lightAnimParams[1].rotationAxis = glm::normalize(glm::vec3(0.5f, 1.0f, 0.0f));
	lightAnimParams[2].rotationAxis = glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f));


	wireCube = Util::CreateWireBox();
	auto mat = ConstantColorMaterial::Create();
	mat->color = glm::vec3(1, 0, 1);
	wireCube->SetMaterial(mat);
}

Scene::~Scene()
{
	// Unregister time observer to prevent accessing destroyed objects
	if (fpCamAdapter)
		TimeManager::Instance().RemoveTimeObserver(fpCamAdapter);
}

void Scene::UpdateLightBboxes()
{
}

void Scene::AddShape(const ShapePtr& shape)
{
	objects.push_back(shape);
	UpdateLightBboxes();
}

void Scene::AddShapes(const std::vector<ShapePtr> shapes)
{
	std::copy(shapes.cbegin(), shapes.cend(), std::back_inserter(objects));
	UpdateLightBboxes();
}

void Scene::SetSkybox(const SkyboxPtr& skybox)
{
	this->skybox = skybox;
}


void Scene::Render(const ViewportPtr& viewport)
{
	viewport->Apply();

	//Render skybox
	if (skybox)
		skybox->Render(shared_from_this());

	//Render objects
	for(ShapePtr& s : objects)
	{
		s->Render(shared_from_this());
	}

	if(renderLightRepresentation)
	{
		for (auto& pl : lightModel->pointLights)
		{
			if (const auto& plr = pl->ModelRepresentation())
			{
					plr->Render(shared_from_this());
			}
		}

		for(auto& sl : lightModel->spotLights)
		{
			if (const auto& plr = sl->ModelRepresentation())
			{
				plr->Render(shared_from_this());
			}
		}

		if (lightModel->directionalLight)
		{
			if (const auto& dlr = lightModel->directionalLight->ModelRepresentation())
			{
				dlr->Render(shared_from_this());
			}
		}
	}

	if (renderBoundingBoxes)
	{
		//glDisable(GL_DEPTH_TEST);
		//glDepthMask(GL_FALSE);

		for (auto& sh : objects)
		{
			auto bbox = sh->BoundingBox();
			auto tmat = glm::translate(glm::mat4(1.f), bbox.p);
			auto smat = glm::scale(tmat, bbox.d);
			wireCube->SetWorldTransform(smat);
			wireCube->Render(shared_from_this());
		}

		//glDepthMask(GL_TRUE);
		//glEnable(GL_DEPTH_TEST);
	}
}

void Scene::TimeUpdate(double time)
{
	(void)time;  // Animation is frame-based, not time-based
	//Animate lights
	SpotLightPtr pl;
	
	auto rotate_light = [](SpotLightPtr& sl, float radians, const glm::vec3& axis)
	{
		glm::mat4 lightTransform1 = glm::rotate(glm::mat4(1.0f), radians, axis);
		sl->SetPosition(lightTransform1 * sl->Position());

		glm::vec3 newDir = glm::transpose(glm::inverse(glm::mat3(lightTransform1))) * sl->Frame().ViewDir();
		sl->SetDirection(newDir);
	};

	for (size_t i = 0; i < std::min(lightAnimParams.size(), lightModel->spotLights.size()); i++)
	{
		if (lightModel->spotLights[i]->Animated())
			rotate_light(lightModel->spotLights[i], lightAnimParams[i].radiansPerInterval, lightAnimParams[i].rotationAxis);
	}

	//if (objects.size() > 0)
	//{
	//	glm::mat4 rot_t = glm::rotate(glm::mat4(1.0f), glm::radians(0.2f), glm::vec3(1,1,1));
	//	auto ot = objects[0]->WorldTransform();
	//	ot *= rot_t;
	//	objects[0]->SetWorldTransform(ot);
	//}

	lightModel->UpdateUniformBuffer(activeCamera);
}

void Scene::SetCamera(const CameraPtr& cam)
{
	activeCamera = cam;
	cam->AddObserver(shared_from_this());
}

void Scene::AddLight(const PointLightPtr& light)
{
	lightModel->pointLights.push_back(light);
}

void Scene::AddLight(const SpotLightPtr& light)
{
	lightModel->spotLights.push_back(light);
}

void Scene::SetLight(const DirectionalLightPtr& light)
{
	lightModel->directionalLight = light;
}

void Scene::SetLight(const AmbientLightPtr& light)
{
	lightModel->ambientLight = light;
}

void Scene::ConnectInputHandler(InputHandler& ih)
{
	// Scene will handle all input and forward to the appropriate adapter
	ih.AddMouseObserver(std::static_pointer_cast<MouseObserver>(shared_from_this()));
	ih.AddKeyboardObserver(std::static_pointer_cast<KeyboardObserver>(shared_from_this()));
}

AABBox Scene::BoundingBox() const
{
	AABBox box;
	for (auto& obj : objects)
		box += obj->BoundingBox();
	return box;
}

void Scene::SetRenderBoundingBoxes(bool enable)
{
	renderBoundingBoxes = enable;
}

void Scene::SetRenderLightRepresentation(bool enable)
{
	renderLightRepresentation = enable;
}

void Scene::SetUseInspectionMode(bool enable)
{
	useInspectionMode = enable;
}

void Scene::CameraChanged()
{
}

void Scene::OnKey(const Input::Key key, const Input::Modifier mod)
{
	// Handle space key to toggle camera mode
	if (key == Input::Key::SPACE)
	{
		ToggleCameraMode();
		return;
	}

	// Forward keyboard input to the active adapter
	if (!useInspectionMode && fpCamAdapter)
	{
		fpCamAdapter->OnKey(key, mod);
	}
}

void Scene::OnKeyStateChange(const Input::Key key, const Input::KeyState state)
{
	// Forward key state changes to the active adapter
	if (!useInspectionMode && fpCamAdapter)
	{
		fpCamAdapter->OnKeyStateChange(key, state);
	}
}

void Scene::OnMouseMove(const glm::vec2& position)
{
	lastMousePosition = position;
	if (useInspectionMode && inspectionCamAdapter)
	{
		inspectionCamAdapter->OnMouseMove(position);
	}
	else if (!useInspectionMode && fpCamAdapter)
	{
		fpCamAdapter->OnMouseMove(position);
	}
}

void Scene::OnMouseDrag(const glm::vec2& position)
{
	lastMousePosition = position;
	if (useInspectionMode && inspectionCamAdapter)
	{
		inspectionCamAdapter->OnMouseDrag(position);
	}
	else if (!useInspectionMode && fpCamAdapter)
	{
		fpCamAdapter->OnMouseDrag(position);
	}
}

void Scene::OnMouseClick(Input::MouseButton button, Input::Direction state, const glm::vec2& position)
{
	lastMousePosition = position;
	if (useInspectionMode && inspectionCamAdapter)
	{
		inspectionCamAdapter->OnMouseClick(button, state, position);
	}
	else if (!useInspectionMode && fpCamAdapter)
	{
		fpCamAdapter->OnMouseClick(button, state, position);
	}
}

void Scene::OnMouseWheel(Input::Direction direction, const glm::vec2& position)
{
	if (useInspectionMode && inspectionCamAdapter)
	{
		inspectionCamAdapter->OnMouseWheel(direction, position);
	}
	else if (!useInspectionMode && fpCamAdapter)
	{
		fpCamAdapter->OnMouseWheel(direction, position);
	}
}

void Scene::ToggleCameraMode()
{
	useInspectionMode = !useInspectionMode;

	// Synchronize mouse position to prevent view jumping
	if (inspectionCamAdapter)
		inspectionCamAdapter->OnMouseClick(Input::MouseButton::LEFT, Input::Direction::UP, lastMousePosition);
	if (fpCamAdapter)
		fpCamAdapter->OnMouseClick(Input::MouseButton::LEFT, Input::Direction::UP, lastMousePosition);

	std::cout << "Camera mode: " << (useInspectionMode ? "Inspection" : "First Person") << std::endl;
}
