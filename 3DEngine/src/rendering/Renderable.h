#pragma once

#include <memory>

class Scene;
using ScenePtr = std::shared_ptr<Scene>;
using SceneConstPtr = std::shared_ptr<const Scene>;
using SceneWeakPtr = std::weak_ptr<Scene>;

class Renderable
{
public:

	/// Renders the object with the passed scene
	virtual void Render(const ScenePtr& scene) const = 0;

protected:
};