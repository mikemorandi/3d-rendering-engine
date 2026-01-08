#pragma once

#include <string>
#include <memory>
#include <vector>
#include <filesystem>


class ImageData;
using ImageDataPtr = std::shared_ptr<ImageData>;
using ImageDataConstPtr = std::shared_ptr<const ImageData>;
using ImageDataWeakPtr = std::weak_ptr<ImageData>;

class ImageData {
public:

	static ImageDataPtr Create(unsigned int width, unsigned int height, unsigned int components = 4);

	typedef unsigned char pixelFormat_t;

	ImageData(unsigned int width, unsigned int height, unsigned int components = 4);

	size_t Bytes();

	unsigned int width;
	unsigned int height;
	unsigned int components;
	std::unique_ptr<pixelFormat_t[]> pixelData;
};

class ImageUtil
{
public:
		
	static ImageDataPtr LoadImage(const std::filesystem::path path);

	static std::vector<ImageDataPtr> LoadImages(const std::vector<std::filesystem::path> paths);

	static std::vector<ImageDataPtr> LoadCubeMapImages(const std::filesystem::path path);

private:

	static void Initialize();

	static bool initialized;

};
