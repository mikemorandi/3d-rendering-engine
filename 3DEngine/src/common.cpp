#include "config.h"

#include <filesystem>

const std::filesystem::path Config::DATA_BASE_PATH = "./data/";
const std::filesystem::path Config::TEXTURE_BASE_PATH = Config::DATA_BASE_PATH / "texture";
const std::filesystem::path Config::MODELS_BASE_PATH  = Config::DATA_BASE_PATH / "models";
const std::filesystem::path Config::SHADER_BASE_PATH  = Config::DATA_BASE_PATH / "shader";
const std::filesystem::path Config::SCENE_BASE_PATH   = Config::DATA_BASE_PATH / "scenes";