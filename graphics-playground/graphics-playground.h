#pragma once

#include <rendering/Renderer.h>
#include <rendering/Viewport.h>

struct GLFWwindow;

#include <string>
#include <vector>

class GraphicsPlayground {
public:

	GraphicsPlayground();

	~GraphicsPlayground();

	void InitGL();

	bool InitializeWindow();

	void Render();

	bool LoadScene(std::string& sceneName);

	void InitializeEvents(Scene_ptr& scene);

	void SetWindowTitle(const std::string title);

	std::vector<std::string> FindScences() const;

private:
	GLFWwindow* window;
	Viewport_ptr viewport;
	Renderer_ptr renderer;
	unsigned int numMultisamples;
	static const std::string WINDOW_TITLE_PREFIX;

};