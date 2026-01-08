#pragma once

#include <string>
#include <filesystem>

struct Config {

public:

	inline static const std::filesystem::path DATA_BASE_PATH = "./data/";
	inline static const std::filesystem::path TEXTURE_BASE_PATH = DATA_BASE_PATH / "texture";
	inline static const std::filesystem::path MODELS_BASE_PATH = DATA_BASE_PATH / "models";
	inline static const std::filesystem::path SHADER_BASE_PATH = DATA_BASE_PATH / "shader";
	inline static const std::filesystem::path SCENE_BASE_PATH = DATA_BASE_PATH / "scenes";
};