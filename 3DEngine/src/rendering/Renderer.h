#pragma once

#include "../util/SharedPointer.h"
#include "../input/ViewportObserver.h"
#include "../input/KeyboardObserver.h"

SHARED_PTR_CLASS_DECL(Renderer);
SHARED_PTR_CLASS_DECL(Viewport);
SHARED_PTR_CLASS_DECL(Scene);
SHARED_PTR_CLASS_DECL(GeometryBuffer);
SHARED_PTR_CLASS_DECL(ShadowDepthShader);
SHARED_PTR_CLASS_DECL(Light);


class Renderer : public ViewportObserver, public KeyboardObserver
{
public:

	static Renderer_ptr Create(const Viewport_ptr& viewport);

	Renderer(const Viewport_ptr& viewport);

	void Render();

	virtual ~Renderer();

	virtual void ViewportChanged(const Viewport_ptr& viewport) override;

	virtual void OnKey(const Input::Key key, const Input::Modifier mod) override;

	void SetScene(Scene_ptr scene);

	Scene_ptr Scene();

protected:


	void Create2DOverlayScene();

	// Shadow mapping
	void GenerateShadowMaps();
	void RenderShadowMap(const Light_ptr& light);
	void InitializeShadowMaps();

	bool				m_ShowDebugElements;
	bool				m_ShadowMapsInitialized;

	Scene_ptr			m_Scene;
	Scene_ptr			overlay2Dscene;
	GeometryBuffer_ptr	m_gBuffer;
	Viewport_ptr		m_Viewport;

	ShadowDepthShader_ptr m_ShadowDepthShader;

};

