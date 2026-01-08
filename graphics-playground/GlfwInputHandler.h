#pragma once

#include "input/InputHandler.h"
#include <vector>

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

struct GLFWwindow;

class GlfwInputHandler : public InputHandler
{

public:

	static GlfwInputHandler& Instance();

	static void click(GLFWwindow* window, int button, int action, int mods);
	static void wheel(GLFWwindow* window, double x, double y);
	static void mouseMove(GLFWwindow* window, double x, double y);

	static void key(GLFWwindow* window, int key, int scancode, int action, int mods);

	void SetWindow(GLFWwindow* window);

	virtual void AddMouseObserver(const MouseObserverWeakPtr& observer);
	virtual void AddKeyboardObserver(const KeyboardObserverWeakPtr& observer);

protected:

	GlfwInputHandler();

	void handleDrag(const glm::vec2& position);
	void handleMove(const glm::vec2& position);
	void handleClick(const glm::vec2& position, int button, int action, int mods);
	void handleWheel(const glm::vec2& position, const glm::vec2& offsets);
	void handleKey(GLFWwindow* window, int key, int modifier);
	void handleKeyStateChange(int key, int action);
	void toggleFullscreen();

	std::vector<MouseObserverWeakPtr> mouseObservers;
	std::vector<KeyboardObserverWeakPtr> keyboardObservers;

	bool isDragging;
	glm::vec2 lastCursorPos;

	GLFWwindow* window;
	bool isFullscreen;
	int windowedPosX, windowedPosY;
	int windowedWidth, windowedHeight;

	static GlfwInputHandler instance;


};

