#pragma once

#include <vector>
#include <array>

#include <glm/fwd.hpp>

#include "../util/SharedPointer.h"
#include "../math/BoundingBox.h"
#include "../light/LightModel.h"
#include "../animation/TimeObserver.h"
#include "../camera/Camera.h"

//fwd decls
class ShaderBase;
class InputHandler;
	
SHARED_PTR_CLASS_DECL(Scene);
SHARED_PTR_CLASS_DECL(Shape);
SHARED_PTR_CLASS_DECL(Skybox);
SHARED_PTR_CLASS_DECL(WireCube);
SHARED_PTR_CLASS_DECL(InspectionCameraAdapter);
SHARED_PTR_CLASS_DECL(FirstPersonCameraAdapter);
SHARED_PTR_CLASS_DECL(Framebuffer);
SHARED_PTR_CLASS_DECL(Viewport);
SHARED_PTR_CLASS_DECL(AmbientLight)

class Scene : public TimeObserver, public CameraObserver, public std::enable_shared_from_this<Scene>
{
public:		

	static Scene_ptr Create(const Camera_ptr& cam, bool has_frambufer = true);

	Scene(const Camera_ptr& cam, bool has_framebufer);

	virtual ~Scene();

	void Render(const Viewport_ptr& viewport);

	void AddShape(const Shape_ptr& shape);
	void AddShapes(const std::vector<Shape_ptr> shapes);
	void AddLight(const PointLight_ptr& shape);
	void AddLight(const SpotLight_ptr& light);
	void SetLight(const DirectionalLight_ptr& light);
	void SetLight(const AmbientLight_ptr& light);
	void SetSkybox(const Skybox_ptr& skybox);
	void SetCamera(const Camera_ptr& cam);

	AABBox BoundingBox() const;

	void SetRenderBoundingBoxes(bool enable);
	bool RenderBoundingBoxes() const { return renderBoundingBoxes; };

	void SetRenderLightRepresentation(bool enable);
	bool RenderLightRepresentation() const { return renderLightRepresentation; };

	void TimeUpdate(double time) override;

	Camera_ptr activeCamera;
	LightModel_ptr lightModel;
	std::vector<Shape_ptr> objects;
	Skybox_ptr skybox;
	std::string name;
	
	
	class LightAnimationParam
	{
		public:
			glm::vec3 rotationAxis;
			float radiansPerInterval;
	};

	void ConnectInputHandler(InputHandler& ih);

	std::array<LightAnimationParam,3> lightAnimParams;

protected:

	void UpdateLightBboxes();

	virtual void CameraChanged() override;

	InspectionCameraAdapter_ptr inspectionCamAdapter;
	FirstPersonCameraAdapter_ptr fpCamAdapter;

	Framebuffer_ptr framebuffer;
	Shape_ptr wireCube;

	bool renderLightRepresentation;
	bool renderBoundingBoxes;
};


