
#include "CubeMapTexture.h"
#include "../texture/ImageUtil.h"
#include "../error.h"

namespace fs = std::filesystem;

CubeMapTexture::CubeMapTexture(const std::filesystem::path& cubeMapTexturePath)
{	
	std::vector<ImageDataPtr> images = ImageUtil::LoadCubeMapImages(cubeMapTexturePath);
	LoadImages(images);
}

CubeMapTexture::CubeMapTexture(const std::filesystem::path& textureBasePath, const std::string& imageExtension)
{
	
	auto getTextureFileName = [textureBasePath, imageExtension](int i) -> fs::path
	{
		const char* suffixes[] = { "posy", "negx", "posz", "posx","negz", "negy" };
		return textureBasePath / suffixes[i] / fs::path("." + imageExtension);
	};

	std::vector<ImageDataPtr> images = ImageUtil::LoadImages({
		getTextureFileName(0),
		getTextureFileName(1),
		getTextureFileName(2),
		getTextureFileName(3),
		getTextureFileName(4),
		getTextureFileName(5),
		});

	LoadImages(images);
}

void CubeMapTexture::LoadImages(std::vector<ImageDataPtr>& images)
{
	/*
	cubemap texture layout
			 _______
			|		|
			|  +Y	|
	 _______|_______|_______ _______
	|       |		|		|		|
	| - X   |  +Z	|  +X	|  -Z	|
	|_______|_______|_______|_______|
			|		|
			|  -Y	|
			|_______|
	*/


	if (!images.empty())
	{
		GLenum targets[] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
		};

		//Generate texture object and bind
		glGenTextures(1, &texObject);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texObject);

		GLenum format = (images[0]->components == 3 ? GL_RGB : GL_RGBA);

		for (int i = 0; i < 6; i++)
		{
			glTexImage2D(targets[i], 0, format, images[i]->width, images[i]->height, 0, format, GL_UNSIGNED_BYTE, images[i]->pixelData.get());
		}
		InitTextureParams();
	}
}

void CubeMapTexture::InitTextureParams()
{
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

CubeMapTexture::~CubeMapTexture()
{
	glDeleteTextures(1, &texObject);
}

void CubeMapTexture::BindTexture(int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texObject);
}