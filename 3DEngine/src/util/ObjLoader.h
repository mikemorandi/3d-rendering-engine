#pragma once

#include <memory>

#include <string>
#include <iosfwd>
#include <vector>
#include <filesystem>
#include <glm/fwd.hpp>

class WavefrontObjMaterial;
using WavefrontObjMaterialPtr = std::shared_ptr<WavefrontObjMaterial>;
using WavefrontObjMaterialConstPtr = std::shared_ptr<const WavefrontObjMaterial>;
using WavefrontObjMaterialWeakPtr = std::weak_ptr<WavefrontObjMaterial>;
class ObjLoader;
using ObjLoaderPtr = std::shared_ptr<ObjLoader>;
using ObjLoaderConstPtr = std::shared_ptr<const ObjLoader>;
using ObjLoaderWeakPtr = std::weak_ptr<ObjLoader>;
class IndexedRawMesh;
using IndexedRawMeshPtr = std::shared_ptr<IndexedRawMesh>;
using IndexedRawMeshConstPtr = std::shared_ptr<const IndexedRawMesh>;
using IndexedRawMeshWeakPtr = std::weak_ptr<IndexedRawMesh>;

class ObjLoader
{
public:

	IndexedRawMeshPtr LoadObjFile(const std::filesystem::path& path);

protected:
	IndexedRawMeshPtr LoadObj(std::istream& istr, const std::filesystem::path& path);
	bool LoadMtllib(std::istream& istr, IndexedRawMeshPtr newMesh);

private:

	std::string currentFile;
};

	