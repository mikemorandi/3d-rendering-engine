#pragma once

#include <vector>
#include <array>

#include <glm/fwd.hpp>

#include "../math/BoundingBox.h"
#include "../light/LightModel.h"
#include "../animation/TimeObserver.h"
#include "../camera/Camera.h"
#include "../input/KeyboardObserver.h"
#include "../input/MouseObserver.h"

//fwd decls
class ShaderBase;
class InputHandler;

class Scene;
using ScenePtr = std::shared_ptr<Scene>;
using SceneConstPtr = std::shared_ptr<const Scene>;
using SceneWeakPtr = std::weak_ptr<Scene>;

class Shape;
using ShapePtr = std::shared_ptr<Shape>;
using ShapeConstPtr = std::shared_ptr<const Shape>;
using ShapeWeakPtr = std::weak_ptr<Shape>;

class Skybox;
using SkyboxPtr = std::shared_ptr<Skybox>;
using SkyboxConstPtr = std::shared_ptr<const Skybox>;
using SkyboxWeakPtr = std::weak_ptr<Skybox>;

class WireCube;
using WireCubePtr = std::shared_ptr<WireCube>;
using WireCubeConstPtr = std::shared_ptr<const WireCube>;
using WireCubeWeakPtr = std::weak_ptr<WireCube>;

class InspectionCameraAdapter;
using InspectionCameraAdapterPtr = std::shared_ptr<InspectionCameraAdapter>;
using InspectionCameraAdapterConstPtr = std::shared_ptr<const InspectionCameraAdapter>;
using InspectionCameraAdapterWeakPtr = std::weak_ptr<InspectionCameraAdapter>;

class FirstPersonCameraAdapter;
using FirstPersonCameraAdapterPtr = std::shared_ptr<FirstPersonCameraAdapter>;
using FirstPersonCameraAdapterConstPtr = std::shared_ptr<const FirstPersonCameraAdapter>;
using FirstPersonCameraAdapterWeakPtr = std::weak_ptr<FirstPersonCameraAdapter>;

class Framebuffer;
using FramebufferPtr = std::shared_ptr<Framebuffer>;
using FramebufferConstPtr = std::shared_ptr<const Framebuffer>;
using FramebufferWeakPtr = std::weak_ptr<Framebuffer>;

class Viewport;
using ViewportPtr = std::shared_ptr<Viewport>;
using ViewportConstPtr = std::shared_ptr<const Viewport>;
using ViewportWeakPtr = std::weak_ptr<Viewport>;

class AmbientLight;
using AmbientLightPtr = std::shared_ptr<AmbientLight>;
using AmbientLightConstPtr = std::shared_ptr<const AmbientLight>;
using AmbientLightWeakPtr = std::weak_ptr<AmbientLight>;

class Scene : public TimeObserver, public CameraObserver, public KeyboardObserver, public MouseObserver, public std::enable_shared_from_this<Scene>
{
public:		

	static ScenePtr Create(const CameraPtr& cam, bool has_frambufer = true);

	Scene(const CameraPtr& cam, bool has_framebufer);

	virtual ~Scene();

	void Render(const ViewportPtr& viewport);

	void AddShape(const ShapePtr& shape);
	void AddShapes(const std::vector<ShapePtr> shapes);
	void AddLight(const PointLightPtr& shape);
	void AddLight(const SpotLightPtr& light);
	void SetLight(const DirectionalLightPtr& light);
	void SetLight(const AmbientLightPtr& light);
	void SetSkybox(const SkyboxPtr& skybox);
	void SetCamera(const CameraPtr& cam);

	[[nodiscard]] AABBox BoundingBox() const;

	void SetRenderBoundingBoxes(bool enable);
	[[nodiscard]] bool RenderBoundingBoxes() const { return renderBoundingBoxes; };

	void SetRenderLightRepresentation(bool enable);
	[[nodiscard]] bool RenderLightRepresentation() const { return renderLightRepresentation; };

	void SetUseInspectionMode(bool enable);
	[[nodiscard]] bool UseInspectionMode() const { return useInspectionMode; };

	void TimeUpdate(double time) override;

	// KeyboardObserver interface
	void OnKey(const Input::Key key, const Input::Modifier mod) override;
	void OnKeyStateChange(const Input::Key key, const Input::KeyState state) override;

	// MouseObserver interface
	void OnMouseMove(const glm::vec2& position) override;
	void OnMouseDrag(const glm::vec2& position) override;
	void OnMouseClick(Input::MouseButton button, Input::Direction state, const glm::vec2& position) override;
	void OnMouseWheel(Input::Direction direction, const glm::vec2& position) override;

	void ToggleCameraMode();

	CameraPtr activeCamera;
	LightModelPtr lightModel;
	std::vector<ShapePtr> objects;
	SkyboxPtr skybox;
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

	InspectionCameraAdapterPtr inspectionCamAdapter;
	FirstPersonCameraAdapterPtr fpCamAdapter;
	bool useInspectionMode;
	glm::vec2 lastMousePosition;

	FramebufferPtr framebuffer;
	ShapePtr wireCube;

	bool renderLightRepresentation;
	bool renderBoundingBoxes;
};


