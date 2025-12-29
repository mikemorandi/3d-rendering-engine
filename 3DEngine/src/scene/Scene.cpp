#include "stdafx.h"

#include "Scene.h"

#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "../util/Util.h"

#include "../input/WindowEventHandler.h"

#include "../input/InputHandler.h"
#include "../camera/InspectionCameraAdapter.h"
#include "../camera/FirstPersonCameraAdapter.h"

#include "../camera/Camera.h"
#include "../shader/ShaderBase.h"
#include "../shader/ShadowMapShader.h"
#include "../shape/Shape.h"
#include "../shape/Skybox.h"
#include "../shape/WireCube.h"
#include "../light/lightModel.h"
#include "../light/PointLight.h"
#include "../light/DirectionalLight.h"
#include "../light/SpotLight.h"
#include "../light/Shadow.h"
#include "../texture/Framebuffer.h"
#include "../texture/DepthTexture.h"

#include "../rendering/Viewport.h"

#include "../materials/Material.h"

#include "../error.h"

using std::vector;

Scene_ptr Scene::Create(const Camera_ptr& cam, bool has_frambufer)
{
	auto ptr = std::make_shared<Scene>(cam, has_frambufer);
	cam->AddObserver(ptr);
	return ptr;
}

Scene::Scene(const Camera_ptr& cam,bool has_frambufer)
	: shadowShader(ShadowMapShader::Create())
	, renderLightRepresentation(false)
	, renderBoundingBoxes(false)
{

	if (has_frambufer)
		framebuffer = Framebuffer::Create();

	activeCamera = cam;

	lightModel.reset(new LightModel());

	if(!lightModel->IsValid())
	{
		throw std::runtime_error("Could not create light model");
	}
	
	if(true)
	{
		inspectionCamAdapter.reset(new InspectionCameraAdapter (cam));
	} 
	else
	{
		fpCamAdapter.reset(new FirstPersonCameraAdapter(cam));
	}

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

}

void Scene::UpdateLightBboxes()
{
	auto bb = BoundingBox();

	for (auto& sl : lightModel->spotLights)
		sl->SetSceneBoundingBox(bb);

	if (auto& dl = lightModel->directionalLight)
		dl->SetSceneBoundingBox(bb);
}

void Scene::AddShape(const Shape_ptr& shape)
{
	objects.push_back(shape);
	UpdateLightBboxes();
}

void Scene::AddShapes(const std::vector<Shape_ptr> shapes)
{
	std::copy(shapes.cbegin(), shapes.cend(), std::back_inserter(objects));
	UpdateLightBboxes();
}

void Scene::SetSkybox(const Skybox_ptr& skybox)
{
	this->skybox = skybox;
}

void Scene::RenderShadowMaps()
{
	if (framebuffer)
	{
		framebuffer->Bind();
		{
			glClearDepth(1);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);

			auto renderShadowMap = [this](Shadow_ptr smap)
			{
				auto smapTex = smap->ShadowMap();
				framebuffer->Attach(smapTex, Framebuffer::Attachment::Depth);
				framebuffer->SetDrawToColorBufferEnabled(false);

				glClear(GL_DEPTH_BUFFER_BIT);

				const glm::ivec2& smapDim = smapTex->Dimensions();
				glViewport(0, 0, smapDim.x, smapDim.y);

				shadowShader->SetLightMatrix(smap->LightViewProjectionMatrix());

				for (Shape_ptr& s : objects)
				{
					if (shadowShader->Use(shared_from_this(), s->WorldTransform()))
					{
						s->RenderShadowMap(shadowShader);
					}
				}
			};

			//Generate shadow maps
			for (auto& sl : lightModel->spotLights)
			{
				if (Shadow_ptr smap = sl->Shadow())
					renderShadowMap(smap);
			}

			if (lightModel->directionalLight)
			{
				if (Shadow_ptr smap = lightModel->directionalLight->Shadow())
					renderShadowMap(smap);
			}

		}
		shadowShader->UnUse();
		framebuffer->Unbind();

		glCullFace(GL_BACK);
		glDisable(GL_CULL_FACE);
	};
}

void Scene::Render(const Viewport_ptr& viewport)
{		
	RenderShadowMaps();
	
	viewport->Apply();

	//Render skybox
	if (skybox)
		skybox->Render(shared_from_this());

	//Render objects
	for(Shape_ptr& s : objects)
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

		if(lightModel->directionalLight)
		{
			auto bbox = lightModel->directionalLight->SceneBoundingBox();
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
	//Animate lights
	int numPLs = (int)lightModel->spotLights.size();
	SpotLight_ptr pl;
	
	auto rotate_light = [](SpotLight_ptr& sl, float radians, const glm::vec3& axis)
	{
		glm::mat4 lightTransform1 = glm::rotate(glm::mat4(1.0f), radians, axis);
		sl->SetPosition(lightTransform1 * sl->Position());

		glm::vec3 newDir = glm::transpose(glm::inverse(glm::mat3(lightTransform1))) * sl->Frame().ViewDir();
		sl->SetDirection(newDir);
	};

	for (int i = 0; i < std::min(lightAnimParams.size(), lightModel->spotLights.size()); i++)
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

void Scene::SetCamera(const Camera_ptr& cam)
{
	activeCamera = cam;
	cam->AddObserver(shared_from_this());
}

void Scene::AddLight(const PointLight_ptr& light)
{
	lightModel->pointLights.push_back(light);
}

void Scene::AddLight(const SpotLight_ptr& light)
{
	lightModel->spotLights.push_back(light);
	light->SetSceneBoundingBox(BoundingBox());
}

void Scene::SetLight(const DirectionalLight_ptr& light)
{
	lightModel->directionalLight = light;
	lightModel->directionalLight->SetSceneBoundingBox(BoundingBox());
	lightModel->directionalLight->SetCamera(activeCamera);
}

void Scene::SetLight(const AmbientLight_ptr& light)
{
	lightModel->ambientLight = light;
}

void Scene::ConnectInputHandler(InputHandler& ih)
{
	if (inspectionCamAdapter)
		ih.AddMouseObserver(inspectionCamAdapter);
	else if (fpCamAdapter)
	{
		ih.AddMouseObserver(fpCamAdapter);
		ih.AddKeyboardObserver(fpCamAdapter);
	}
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

void Scene::CameraChanged()
{
	if(lightModel->directionalLight)
		lightModel->directionalLight->UpdateShadow();
}