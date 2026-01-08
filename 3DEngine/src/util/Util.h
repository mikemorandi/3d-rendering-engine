#pragma once

#include <iosfwd>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>

#include "SharedPointer.h"

class RenderMesh;
using RenderMeshPtr = std::shared_ptr<RenderMesh>;
using RenderMeshConstPtr = std::shared_ptr<const RenderMesh>;
using RenderMeshWeakPtr = std::weak_ptr<RenderMesh>;
class WireCube;
using WireCubePtr = std::shared_ptr<WireCube>;
using WireCubeConstPtr = std::shared_ptr<const WireCube>;
using WireCubeWeakPtr = std::weak_ptr<WireCube>;
class ShaderBase;
using ShaderBasePtr = std::shared_ptr<ShaderBase>;
using ShaderBaseConstPtr = std::shared_ptr<const ShaderBase>;
using ShaderBaseWeakPtr = std::weak_ptr<ShaderBase>;

class Util
{
public:

	static unsigned long GetFileLength(std::ifstream& file);
	[[nodiscard]] static std::string LoadTextFile(char* filename);
	[[nodiscard]] static std::string LoadTextFile(const std::string& s);
	[[nodiscard]] static std::string LoadTextFile(const std::filesystem::path &path);
	static void PrintStrings(const std::vector<std::string>& strings);
	static void PrintUniforms(const ShaderBasePtr& shader);
	[[nodiscard]] static RenderMeshPtr LoadModel(const std::filesystem::path& path, bool computeTangents = false);

	[[nodiscard]] static std::filesystem::path ExtractBaseFolder(std::string path);
	[[nodiscard]] static std::filesystem::path ExtractFileName(std::string path);
	[[nodiscard]] static bool FileExists(const std::filesystem::path& path);

	static RenderMeshPtr GetDragon();
	static RenderMeshPtr GetHorse();
	static RenderMeshPtr GetElephant();
	static RenderMeshPtr CreateBox();
	static RenderMeshPtr CreateWireBox();
};