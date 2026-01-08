#pragma once

#include <glm/fwd.hpp>
#include <memory>


class MouseObserver;
using MouseObserverPtr = std::shared_ptr<MouseObserver>;
using MouseObserverConstPtr = std::shared_ptr<const MouseObserver>;
using MouseObserverWeakPtr = std::weak_ptr<MouseObserver>;

namespace Input 
{
	enum class MouseButton {LEFT, MIDDLE, RIGHT};
	enum class Direction {UP, DOWN};
};

class MouseObserver
{
public:

	virtual void OnMouseMove(const glm::vec2& position) = 0;
	virtual void OnMouseDrag(const glm::vec2& position) = 0;
	virtual void OnMouseClick(Input::MouseButton button, Input::Direction state, const glm::vec2& position) = 0;
	virtual void OnMouseWheel(Input::Direction direction, const glm::vec2& position) = 0;
};