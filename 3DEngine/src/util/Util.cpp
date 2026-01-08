
#include "Util.h"
#include "RawMesh.h"

#include "../util/ObjLoader.h"

#include "../shape/Box.h"
#include "../shape/WireCube.h"
#include "../shape/RenderMesh.h"

#include "../shader/ShaderBase.h"

#include "../error.h"

#include <fstream>
#include <filesystem>
#include <ctime>
#include <regex>
#include <cctype>
#include <functional>

#include "../core/gl.h"

#include <glm/gtc/matrix_transform.hpp>

using std::string;
using std::vector;

namespace fs = std::filesystem;

unsigned long Util::GetFileLength(std::ifstream& file)
{
	if(!file.good()) return 0;

	file.seekg(0,std::ios::end);
	unsigned long len = (unsigned long)file.tellg();
	file.seekg(std::ios::beg);

	return len;
}

std::string Util::LoadTextFile(const std::string& s)
{
	return LoadTextFile((char*)s.c_str());
}

std::string Util::LoadTextFile(const std::filesystem::path& path)
{
	return LoadTextFile(path.string());
}

std::string Util::LoadTextFile(char* filename)
{
	std::cout << "[File] Loading: " << filename << std::endl;
	std::ifstream file;
	file.open(filename, std::ios::in);

	if(file.is_open())
		{
		std::cout << "[File] File opened successfully" << std::endl;
		unsigned long len = GetFileLength(file);
		std::cout << "[File] File length: " << len << std::endl;

		std::string shaderSource(len, ' ');
		std::cout << "[File] String created with size " << len << std::endl;

		// shaderSource[len] = 0;  // REMOVED: Out of bounds access - std::string is already null-terminated
		unsigned int i=0;
		while (file.good() && i < len)
		{
			shaderSource[i] = file.get(); // get character from file.
			if (!file.eof())
				i++;
		}

		std::cout << "[File] Read " << i << " characters" << std::endl;
		file.close();

		std::cout << "[File] Creating return string..." << std::endl;
		return std::string(shaderSource.begin(), shaderSource.begin()+i);
	}

	std::cout << "[File] Failed to open file" << std::endl;
	return std::string();

}

void Util::PrintStrings(const std::vector<string>& strings)
{
	for (const auto& str : strings)
	{
		std::cout << str << std::endl;
	}
}

void Util::PrintUniforms(const ShaderBasePtr& shader)
{
	std::cout << shader->GetName() << std::endl << "------------------" << std::endl;
	PrintStrings(shader->GetUniformAttributes());
	std::cout << std::endl;
}

RenderMeshPtr Util::LoadModel(const std::filesystem::path& path, bool computeTangentsHint /*= false*/)
{
	ObjLoader oj;
	clock_t begin = clock();
	
	if(IndexedRawMeshPtr rawMesh = oj.LoadObjFile(path))
	{	
		if(OpenGLRawMeshPtr gl_raw_mesh = rawMesh->ConvertToOpenGLMesh())
		{
			if (!gl_raw_mesh->HasNormals())
			{
				Info(gl_raw_mesh->name + ": Computing normals..");
				if(!gl_raw_mesh->ComputeNormals())
				{
					Error("Could not compute normals");
				}
			}

			bool tangents_needed = std::any_of(gl_raw_mesh->materials.begin(), gl_raw_mesh->materials.begin(), 
				[](const WavefrontObjMaterialPtr& mat)
			{
				return mat->HasBumpMap() || mat->HasDisplacementMap();
			});

			if (computeTangentsHint || tangents_needed)
			{
				if (gl_raw_mesh->HasNormals() && gl_raw_mesh->HasTexCoords())
				{
					Info(gl_raw_mesh->name + ": Computing tangents..");
					if (!gl_raw_mesh->ComputeTangents())
						Error("Computing tangents failed");
				}
				else
					Error("Could not compute tangents, normals or tex coords missing");
			}

			RenderMeshPtr mesh = RenderMesh::Create(gl_raw_mesh);

			double elapsed_secs = double(clock() - begin) / CLOCKS_PER_SEC * 1000;
			std::cout << "time [msec]: " << elapsed_secs << std::endl;
			return mesh;
		}
	}

	return RenderMeshPtr();
	
}

RenderMeshPtr Util::GetDragon()
{
	RenderMeshPtr model = Util::LoadModel("../data/models/dragon.obj");
	glm::mat4 t = glm::translate(model->WorldTransform(),glm::vec3(0,-0.85f,0));
	t = glm::scale(t,glm::vec3(8,8,8));
	model->SetWorldTransform(t);
	return model;
}

RenderMeshPtr Util::GetHorse()
{
	RenderMeshPtr model = Util::LoadModel("../data/models/horse.obj");
	glm::mat4 t = glm::translate(model->WorldTransform(),glm::vec3(0,-0.3f,0));
	t = glm::rotate(t, glm::radians(270.0f), glm::vec3(0,1,0));
	model->SetWorldTransform(t);
	return model;
}

RenderMeshPtr Util::GetElephant()
{
	RenderMeshPtr model = Util::LoadModel("../data/models/elephant.obj");
	glm::mat4 t = glm::translate(model->WorldTransform(),glm::vec3(0,-0.5f,0));
	model->SetWorldTransform(t);
	return model;
}

RenderMeshPtr Util::CreateBox()
{
	BoxPtr box = Box::Create();
	box->Init();
	return box;
}

RenderMeshPtr Util::CreateWireBox()
{
	WireCubePtr box = WireCube::Create();
	box->Init();
	return box;
}

fs::path Util::ExtractBaseFolder(std::string filePath)
{
	fs::path pathToFile(filePath);
	return pathToFile.parent_path();
}

fs::path Util::ExtractFileName(std::string path)
{
	fs::path pathToFile(path);
	return pathToFile.filename();
}

bool Util::FileExists(const std::filesystem::path& path) {
	return std::filesystem::exists(path);
}