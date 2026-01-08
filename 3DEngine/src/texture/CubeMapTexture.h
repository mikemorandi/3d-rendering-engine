#pragma once

#include <string>
#include <memory>
#include <vector>
#include <filesystem>

#include "../core/gl.h"

class CubeMapTexture;
using CubeMapTexturePtr = std::shared_ptr<CubeMapTexture>;
using CubeMapTextureConstPtr = std::shared_ptr<const CubeMapTexture>;
using CubeMapTextureWeakPtr = std::weak_ptr<CubeMapTexture>;
class ImageData;
using ImageDataPtr = std::shared_ptr<ImageData>;
using ImageDataConstPtr = std::shared_ptr<const ImageData>;
using ImageDataWeakPtr = std::weak_ptr<ImageData>;

class CubeMapTexture
{
public:
	CubeMapTexture(const std::filesystem::path& cubeMapFile);
	CubeMapTexture(const std::filesystem::path& textureBasePath, const std::string& imageExtension);
	~CubeMapTexture();

	void BindTexture(int textureUnit);

protected:

	void LoadImages(std::vector<ImageDataPtr>& images);

	void InitTextureParams();

	GLuint texObject;
};

