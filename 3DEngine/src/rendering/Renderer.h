#pragma once

#include <memory>
#include "../input/ViewportObserver.h"
#include "../input/KeyboardObserver.h"

class Renderer;
using RendererPtr = std::shared_ptr<Renderer>;
using RendererConstPtr = std::shared_ptr<const Renderer>;
using RendererWeakPtr = std::weak_ptr<Renderer>;

class Viewport;
using ViewportPtr = std::shared_ptr<Viewport>;
using ViewportConstPtr = std::shared_ptr<const Viewport>;
using ViewportWeakPtr = std::weak_ptr<Viewport>;

class Scene;
using ScenePtr = std::shared_ptr<Scene>;
using SceneConstPtr = std::shared_ptr<const Scene>;
using SceneWeakPtr = std::weak_ptr<Scene>;

class GeometryBuffer;
using GeometryBufferPtr = std::shared_ptr<GeometryBuffer>;
using GeometryBufferConstPtr = std::shared_ptr<const GeometryBuffer>;
using GeometryBufferWeakPtr = std::weak_ptr<GeometryBuffer>;

class ShadowDepthShader;
using ShadowDepthShaderPtr = std::shared_ptr<ShadowDepthShader>;
using ShadowDepthShaderConstPtr = std::shared_ptr<const ShadowDepthShader>;
using ShadowDepthShaderWeakPtr = std::weak_ptr<ShadowDepthShader>;

class Light;
using LightPtr = std::shared_ptr<Light>;
using LightConstPtr = std::shared_ptr<const Light>;
using LightWeakPtr = std::weak_ptr<Light>;


class Renderer : public ViewportObserver, public KeyboardObserver
{
public:

	static RendererPtr Create(const ViewportPtr& viewport);

	Renderer(const ViewportPtr& viewport);

	void Render();

	virtual ~Renderer();

	virtual void ViewportChanged(const ViewportPtr& viewport) override;

	virtual void OnKey(const Input::Key key, const Input::Modifier mod) override;

	void SetScene(ScenePtr scene);

	ScenePtr Scene();

protected:


	void Create2DOverlayScene();

	// Shadow mapping
	void GenerateShadowMaps();
	void RenderShadowMap(const LightPtr& light);
	void InitializeShadowMaps();

	bool				m_ShowDebugElements;
	bool				m_ShadowMapsInitialized;

	ScenePtr			m_Scene;
	ScenePtr			overlay2Dscene;
	GeometryBufferPtr	m_gBuffer;
	ViewportPtr			m_Viewport;

	ShadowDepthShaderPtr m_ShadowDepthShader;

};

