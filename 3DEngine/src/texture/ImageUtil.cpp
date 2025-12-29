#include "stdafx.h"
#include "ImageUtil.h"

#include "../error.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool ImageUtil::initialized = false;

ImageData_ptr ImageUtil::LoadImage(const std::filesystem::path path)
{
	Initialize();

	ImageData_ptr data;

	int width, height, channels;

	// Load image using stb_image
	// Request 4 components (RGBA) to match the previous DevIL behavior
	unsigned char* imgData = stbi_load(path.string().c_str(), &width, &height, &channels, 4);

	if (imgData)
	{
		// Create ImageData with 4 components (RGBA)
		data = ImageData::Create(width, height, 4);

		// stb_image loads with origin at top-left, OpenGL expects bottom-left
		// Flip the image vertically
		for (int y = 0; y < height; y++)
		{
			size_t src_offset = y * width * 4;
			size_t dst_offset = (height - 1 - y) * width * 4;
			std::copy(imgData + src_offset, imgData + src_offset + width * 4, data->pixelData.get() + dst_offset);
		}

		// Free the stb_image allocated memory
		stbi_image_free(imgData);
	}
	else
	{
		Error("Could not load texture: " + path.string() + " - " + std::string(stbi_failure_reason()));
	}

	return data;
}

std::vector<ImageData_ptr> ImageUtil::LoadImages(const std::vector<std::filesystem::path> paths)
{
	std::vector<ImageData_ptr> subImages(paths.size());
	for (size_t i = 0; i < paths.size(); i++)
	{
		subImages[i] = LoadImage(paths[i]);
	}

	return subImages;
}

std::vector<ImageData_ptr> ImageUtil::LoadCubeMapImages(const std::filesystem::path path)
{
	Initialize();

	int originalWidth, originalHeight, channels;

	// Load the cubemap cross image
	unsigned char* imgData = stbi_load(path.string().c_str(), &originalWidth, &originalHeight, &channels, 4);

	if (!imgData)
	{
		Error("Could not load cubemap texture: " + path.string() + " - " + std::string(stbi_failure_reason()));
		return std::vector<ImageData_ptr>();
	}

	std::vector<ImageData_ptr> subImages(6);

	// Calculate subimage sizes (assuming cross layout)
	unsigned int width = originalWidth / 4;
	unsigned int height = originalHeight / 3;
	int bpp = 4; // RGBA

	// Allocate memory for each face
	for (int i = 0; i < 6; i++)
	{
		subImages[i] = ImageData::Create(width, height, bpp);
	}

	// Lambda to copy a subregion from the loaded image
	auto copySubpixels = [&](size_t xOffset, size_t yOffset, size_t subWidth, size_t subHeight, ImageData_ptr& dest)
	{
		for (size_t y = 0; y < subHeight; y++)
		{
			size_t srcOffset = ((y + yOffset) * originalWidth + xOffset) * bpp;
			size_t dstOffset = y * subWidth * bpp;
			std::copy(imgData + srcOffset, imgData + srcOffset + subWidth * bpp, dest->pixelData.get() + dstOffset);
		}
	};

	// Extract the 6 faces from the cross layout
	// Layout (standard cubemap cross):
	//     [top]
	// [left][front][right][back]
	//     [bottom]
	copySubpixels(width, 0, width, height, subImages[0]);            // +X (right) - top
	copySubpixels(0, height, width, height, subImages[1]);           // -X (left)
	copySubpixels(1 * width, height, width, height, subImages[2]);   // +Y (top) - front
	copySubpixels(2 * width, height, width, height, subImages[3]);   // -Y (bottom) - right
	copySubpixels(3 * width, height, width, height, subImages[4]);   // +Z (front) - back
	copySubpixels(width, 2 * height, width, height, subImages[5]);   // -Z (back) - bottom

	// Free the stb_image allocated memory
	stbi_image_free(imgData);

	return subImages;
}

void ImageUtil::Initialize()
{
	if (!ImageUtil::initialized)
	{
		// stb_image doesn't require explicit initialization
		// Set vertical flip on load to false (we handle it manually for consistency)
		stbi_set_flip_vertically_on_load(0);
		initialized = true;
	}
}

inline ImageData_ptr ImageData::Create(unsigned int width, unsigned int height, unsigned int components)
{
	return std::make_shared<ImageData>(width, height, components);
}

ImageData::ImageData(unsigned int width, unsigned int height, unsigned int components)
	: width(width)
	, height(height)
	, components(components)
{
	int numBytes = width * height * sizeof(pixelFormat_t) * components;
	pixelData.reset(new pixelFormat_t[numBytes]);

 }

size_t ImageData::Bytes()
{
	return width * height * sizeof(pixelFormat_t) * components;
}
