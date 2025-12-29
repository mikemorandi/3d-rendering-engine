#pragma once

#include <iosfwd>
#include <vector>
#include <string>
#include <filesystem>

#include "SharedPointer.h"

SHARED_PTR_CLASS_DECL(RenderMesh);
SHARED_PTR_CLASS_DECL(WireCube);
SHARED_PTR_CLASS_DECL(ShaderBase);

class Util
{
public:

	static unsigned long GetFileLength(std::ifstream& file);
	static std::string LoadTextFile(char* filename);
	static std::string LoadTextFile(const std::string& s);
	static std::string LoadTextFile(const std::filesystem::path &path);
	static void PrintStrings(const std::vector<std::string> strings);
	static void PrintUniforms(const ShaderBase_ptr& shader);
	static RenderMesh_ptr LoadModel(const std::filesystem::path& path, bool computeTangents = false);

	static std::filesystem::path ExtractBaseFolder(std::string path);
	static std::filesystem::path ExtractFileName(std::string path);
	static bool FileExists(const std::string& name);

	static RenderMesh_ptr GetDragon();
	static RenderMesh_ptr GetHorse();
	static RenderMesh_ptr GetElephant();
	static RenderMesh_ptr CreateBox();
	static RenderMesh_ptr CreateWireBox();
};