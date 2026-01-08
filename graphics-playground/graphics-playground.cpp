#include "graphics-playground.h"

#ifdef _MSC_VER
#pragma warning(disable: 4251) // See https://github.com/cginternals/glbinding/issues/141
#endif
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <glbinding/gl/functions.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/types_to_string.h>
#include <glbinding-aux/debug.h>
#ifdef _MSC_VER
#pragma warning(default: 4251)
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>

#include <core/GLErrorHandler.h>
#include <input/WindowEventHandler.h>
#include <shader/ShaderLibrary.h>
#include <scene/SceneParser.h>
#include <scene/Scene.h>
#include <util/TimeManager.h>
#include <util/Util.h>
#include <config.h>
#include <error.h>

#include "GlfwInputHandler.h"

using std::string;
using namespace gl;
using namespace glbinding;
namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
	atexit(glfwTerminate);

	//Scoped shared pointers -> force cleanup of OpenGL resources
	{
		GraphicsPlayground pg;

		std::string sceneName;

		if (argc == 2)
			sceneName = string(argv[1]);
		else {
			auto sceneList = pg.FindScences();

			unsigned int idCounter = 0;
			for (const auto& entry : sceneList)
				std::cout << idCounter++ << " - " << entry << std::endl;


			while (sceneName.size() == 0) {
				std::cout << "Scene to load (id): ";
				unsigned int sceneIdx;
				std::cin >> sceneIdx;

				if (sceneIdx < sceneList.size())
					sceneName = sceneList[sceneIdx];
			}
		}

		if (pg.InitializeWindow())
		{
			if (pg.LoadScene(sceneName))
			{
				pg.Render();
			}
			else
			{
				Error("Scene could not be loaded: " + sceneName);
				return -1;
			}
		}
		else
		{
			Error("Window initialization failed, exiting");
			return -1;
		}
	}

	return 0;
}

GraphicsPlayground::GraphicsPlayground()
	: window(nullptr)
	, numMultisamples(4)
{
	viewport = Viewport::Create(1600, 900);
	renderer = Renderer::Create(viewport);
}

GraphicsPlayground::~GraphicsPlayground()
{
	// Clear all GLFW callbacks before destruction to prevent accessing destroyed objects
	if (window)
	{
		// Force windowed mode if in fullscreen to avoid fullscreen-related cleanup issues
		GLFWmonitor* currentMonitor = glfwGetWindowMonitor(window);
		if (currentMonitor != nullptr)
		{
			// We're in fullscreen, switch to windowed before cleanup
			glfwSetWindowMonitor(window, nullptr, 100, 100, 800, 600, GLFW_DONT_CARE);
		}

		// Clear callbacks before destruction
		glfwSetKeyCallback(window, nullptr);
		glfwSetMouseButtonCallback(window, nullptr);
		glfwSetCursorPosCallback(window, nullptr);
		glfwSetScrollCallback(window, nullptr);
		glfwSetFramebufferSizeCallback(window, nullptr);
	}

	ShaderLibrary::Instance()->Reset();
}

std::vector<std::string> GraphicsPlayground::FindScences() const
{
	std::vector<std::string> scenes;

	for (const auto& entry : std::filesystem::directory_iterator(Config::SCENE_BASE_PATH))
	{
		if (entry.is_regular_file() && entry.path().extension().string().compare(".xml") == 0)
			scenes.push_back(entry.path().filename().string());
	}

	return scenes;
}


void GraphicsPlayground::InitGL()
{
	glbinding::initialize(glfwGetProcAddress);
	GLErrorHandler::Initialize();

	// Use depth buffering for hidden surface elimination
	glEnable(GL_DEPTH_TEST);

	GLint iMultiSample = 0;
	GLint iNumSamples = 0;
	glEnable(GL_MULTISAMPLE);
	glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
	glGetIntegerv(GL_SAMPLES, &iNumSamples);

	std::cout << "MSAA: buffers= " << iMultiSample << ", samples = " << iNumSamples << std::endl;

		std::cout << std::endl
		<< "OpenGL Version:  " << aux::ContextInfo::version() << std::endl
		<< "OpenGL Vendor:   " << aux::ContextInfo::vendor() << std::endl
		<< "OpenGL Renderer: " << aux::ContextInfo::renderer() << std::endl;
}

bool GraphicsPlayground::InitializeWindow()
{	
	glfwSetErrorCallback([](int errnum, const char* errmsg) {
		std::cerr << errnum << ": " << errmsg << std::endl; });
	
	/* Initialize the library */
	if (!glfwInit())
		return false;

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_SAMPLES, numMultisamples);

#ifdef __APPLE__
	// macOS requires explicit OpenGL version and profile specification
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE ? 1 : 0);
#endif

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(viewport->width, viewport->height, WINDOW_TITLE_PREFIX.c_str(), NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return false;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	InitGL();

	// Get actual framebuffer size (important for Retina displays where it differs from window size)
	int fbWidth, fbHeight;
	glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
	viewport = Viewport::Create(fbWidth, fbHeight);
	viewport->Apply();

	return window;
}

void GraphicsPlayground::Render() {

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer->Render();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		TimeManager::Tick(glfwGetTime());
	}
}

bool GraphicsPlayground::LoadScene(std::string& sceneName) {

	auto sceneData = Util::LoadTextFile(Config::SCENE_BASE_PATH / fs::path(sceneName));

	if (sceneData.length() > 0) {
		SceneParser sceneParser;
		if (sceneParser.Parse(sceneData))
		{
			auto scene = sceneParser.Scene();
			TimeManager::Instance().AddTimeObserver(scene);

			SetWindowTitle(scene->name);
			InitializeEvents(scene);
			renderer->SetScene(scene);
			WindowEventHandler::ViewportChanged(viewport);
			return true;
		}
	}

	return false;
}

void GraphicsPlayground::InitializeEvents(ScenePtr& scene) {

	WindowEventHandler& winEventHandler = WindowEventHandler::Instance();
	winEventHandler.AddViewportObserver(renderer);

	GlfwInputHandler& inputHandler = GlfwInputHandler::Instance();
	inputHandler.SetWindow(window);
	inputHandler.AddKeyboardObserver(renderer);

	scene->ConnectInputHandler(inputHandler);

	glfwSetKeyCallback(window, GlfwInputHandler::key);
	glfwSetMouseButtonCallback(window, GlfwInputHandler::click);
	glfwSetCursorPosCallback(window, GlfwInputHandler::mouseMove);
	glfwSetScrollCallback(window, GlfwInputHandler::wheel);

	glfwSetFramebufferSizeCallback(window, []([[maybe_unused]] GLFWwindow* window, int width, int height)
	{
		WindowEventHandler::Resize(width, height);
	});

}

const std::string GraphicsPlayground::WINDOW_TITLE_PREFIX = "OpenGL Playground";

void GraphicsPlayground::SetWindowTitle(const std::string title)
{
	string windowTitle = string(WINDOW_TITLE_PREFIX) + " - " + title;
	glfwSetWindowTitle(window, windowTitle.c_str());
}

