#pragma once

#include "Scene.h"
#include <memory>


class SceneOverlay2D;
using SceneOverlay2DPtr = std::shared_ptr<SceneOverlay2D>;
using SceneOverlay2DConstPtr = std::shared_ptr<const SceneOverlay2D>;
using SceneOverlay2DWeakPtr = std::weak_ptr<SceneOverlay2D>;
class Camera;
using CameraPtr = std::shared_ptr<Camera>;
using CameraConstPtr = std::shared_ptr<const Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;

class SceneOverlay2D : public Scene
{
public:

	SceneOverlay2D(ScenePtr scene);

	static SceneOverlay2DPtr Create(ScenePtr scene);

	virtual void TimeUpdate(double time) override;

	~SceneOverlay2D();
};

