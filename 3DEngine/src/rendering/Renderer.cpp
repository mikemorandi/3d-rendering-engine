#include "Renderer.h"
#include "Viewport.h"
#include "GeometryBuffer.h"

#include  "../scene/Scene.h"
#include  "../scene/SceneOverlay2D.h"

#include  "../util/TimeManager.h"

#include "../error.h"
#include "../shader/ShadowDepthShader.h"
#include "../light/Light.h"
#include "../light/DirectionalLight.h"
#include "../light/SpotLight.h"
#include "../shape/Shape.h"
#include "../math/BoundingBox.h"
#include "../texture/Framebuffer.h"

#include <iostream>
#include <stdexcept>

Renderer_ptr Renderer::Create(const Viewport_ptr& viewport)
{
	return std::make_shared<Renderer>(viewport);
}

Renderer::Renderer(const Viewport_ptr& viewport)
	: m_ShowDebugElements(false)
	, m_ShadowMapsInitialized(false)
	, m_Viewport(viewport)
{
	// ShadowDepthShader created lazily in InitializeShadowMaps()
	// after OpenGL context is available
}

Renderer::~Renderer()
{
}

void Renderer::SetScene(Scene_ptr scene)
{
	m_Scene = scene;
	m_ShadowMapsInitialized = false;  // Reset flag when scene changes
	Create2DOverlayScene();
}
Scene_ptr Renderer::Scene()
{
	return m_Scene;
}

void Renderer::Render()
{
	// Initialize shadow maps on first render
	if (m_Scene && !m_ShadowMapsInitialized)
	{
		try {
			std::cout << "[Shadow] First render - initializing shadow maps..." << std::endl;
			InitializeShadowMaps();
			m_ShadowMapsInitialized = true;
			std::cout << "[Shadow] Shadow maps initialized successfully" << std::endl;
		}
		catch (const std::exception& e) {
			std::cerr << "[Shadow] FATAL: Exception during shadow map initialization: " << e.what() << std::endl;
			throw;
		}
	}

	// Generate shadow maps first
	if (m_Scene)
	{
		try {
			GenerateShadowMaps();
		}
		catch (const std::exception& e) {
			std::cerr << "[Shadow] ERROR during shadow map generation: " << e.what() << std::endl;
			// Don't throw - continue rendering without shadows
		}
	}

	// Main rendering pass
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(m_Scene)
	{
		m_Scene->Render(m_Viewport);
	}

	if (m_ShowDebugElements && overlay2Dscene)
	{
		glDisable(GL_DEPTH_TEST);
		overlay2Dscene->Render(m_Viewport);
		glEnable(GL_DEPTH_TEST);
	}
}

void Renderer::ViewportChanged(const Viewport_ptr& viewport)
{
	m_Viewport = viewport;
}

void Renderer::OnKey(const Input::Key key, [[maybe_unused]] const Input::Modifier mod)
{
	if (key == Input::Key::D)
	{
		auto& tm = TimeManager::Instance();

		if (!m_ShowDebugElements)			
			tm.AddTimeObserver(overlay2Dscene);
		else
			tm.RemoveTimeObserver(overlay2Dscene);

		m_ShowDebugElements = !m_ShowDebugElements;

		std::string msg = (m_ShowDebugElements ? "Enabling" : "Disabling");
		msg += " debug mode";
		Debug(msg);
	}
	else if (key == Input::Key::B)
	{
		if (m_Scene)
			m_Scene->SetRenderBoundingBoxes(!m_Scene->RenderBoundingBoxes());
	}
	else if (key == Input::Key::L)
	{
		if (m_Scene)
			m_Scene->SetRenderLightRepresentation(!m_Scene->RenderLightRepresentation());
	}
}

void Renderer::Create2DOverlayScene()
{
	if (m_Scene)
	{
		overlay2Dscene = SceneOverlay2D::Create(m_Scene);
	}
}

void Renderer::InitializeShadowMaps()
{
	try {
		std::cout << "[Shadow] InitializeShadowMaps: Starting..." << std::endl;

		// Create shadow depth shader on first use (requires OpenGL context)
		if (!m_ShadowDepthShader) {
			std::cout << "[Shadow] InitializeShadowMaps: Creating ShadowDepthShader..." << std::endl;
			m_ShadowDepthShader = ShadowDepthShader::Create();
			std::cout << "[Shadow] InitializeShadowMaps: ShadowDepthShader created successfully" << std::endl;
		}

		if (!m_Scene || !m_Scene->lightModel) {
			std::cout << "[Shadow] InitializeShadowMaps: No scene or light model" << std::endl;
			return;
		}

		std::cout << "[Shadow] InitializeShadowMaps: Getting scene bounds..." << std::endl;
		AABBox sceneBounds = m_Scene->BoundingBox();

		std::cout << "[Shadow] InitializeShadowMaps: Calling lightModel->InitializeShadowMaps..." << std::endl;
		m_Scene->lightModel->InitializeShadowMaps(sceneBounds);

		std::cout << "[Shadow] InitializeShadowMaps: Completed successfully" << std::endl;
	}
	catch (const std::out_of_range& e) {
		std::cerr << "[Shadow] ERROR in InitializeShadowMaps: std::out_of_range: " << e.what() << std::endl;
		throw;
	}
	catch (const std::exception& e) {
		std::cerr << "[Shadow] ERROR in InitializeShadowMaps: " << e.what() << std::endl;
		throw;
	}
}

void Renderer::GenerateShadowMaps()
{
	if (!m_Scene || !m_Scene->lightModel) return;

	auto& lightModel = m_Scene->lightModel;

	// Update light matrices based on current scene bounds
	AABBox sceneBounds = m_Scene->BoundingBox();
	lightModel->UpdateShadowMatrices(sceneBounds);

	// Render directional light shadow map
	if (lightModel->directionalLight &&
	    lightModel->directionalLight->CastsShadows())
	{
		RenderShadowMap(lightModel->directionalLight);
	}

	// Render spotlight shadow maps
	for (auto& spotLight : lightModel->spotLights)
	{
		if (spotLight->CastsShadows())
		{
			RenderShadowMap(spotLight);
		}
	}
}

void Renderer::RenderShadowMap(const Light_ptr& light)
{
	auto shadowMap = light->GetShadowMap();
	if (!shadowMap) return;

	// Get shadow framebuffer
	auto shadowFBO = light->GetShadowFramebuffer();
	if (!shadowFBO || !shadowFBO->Bind()) return;

	// Set viewport to shadow map resolution
	int shadowRes = light->GetShadowMapResolution();
	glViewport(0, 0, shadowRes, shadowRes);

	// Clear depth buffer
	glClear(GL_DEPTH_BUFFER_BIT);

	// Enable front-face culling to reduce peter panning
	glCullFace(GL_FRONT);

	// Render all objects with shadow depth shader
	glm::mat4 lightSpaceMatrix = light->GetLightSpaceMatrix();

	for (auto& object : m_Scene->objects)
	{
		if (!object->CastsShadows()) continue;

		m_ShadowDepthShader->SetLightSpaceMatrix(lightSpaceMatrix);
		m_ShadowDepthShader->Use(m_Scene, object->WorldTransform());
		object->RenderGeometry(m_ShadowDepthShader);  // Render with shadow shader only
		m_ShadowDepthShader->UnUse();
	}

	// Restore state
	glCullFace(GL_BACK);
	shadowFBO->Unbind();

	// Restore viewport
	m_Viewport->Apply();
}
