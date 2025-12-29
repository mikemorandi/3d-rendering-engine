#pragma once

#include "../util/SharedPointer.h"

#include <vector>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

SHARED_PTR_CLASS_DECL(OpenGLRawMesh);
SHARED_PTR_CLASS_DECL(IndexedRawMesh);
SHARED_PTR_CLASS_DECL(WavefrontObjMaterial);

class AABBox;

struct Tri
{
	typedef int index_t;
	index_t v[3][3];

	Tri()
	{	
		std::fill(&v[0][0], &v[0][0] + sizeof(v) / sizeof(index_t), -1);
	};

	void Set(int idx, glm::ivec3& indices)
	{
		memcpy(v[idx], &indices[0], 3 * sizeof(glm::ivec3::value_type));
	}

	enum type { pos = 0, tex_uv = 1, normal = 2 };
};

class WavefrontObjMaterial
{
public:
	static WavefrontObjMaterial_ptr Create(std::string name);
	
	WavefrontObjMaterial(std::string name);
public:

	bool HasTextures() const;
	bool HasBumpMap() const;
	bool HasDisplacementMap() const;

	std::string name;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float opacity;
	float shininess;
	std::string ambientColorTexture;
	std::string diffuseColorTexture;
	std::string specularColorTexture;
	std::string specularHightlightTexture;
	std::string alphaMapTexture;
	std::string bumpMapTexture;
	std::string displacementMapTexture;
	bool specularEnabled;
};

class AbstractRawMesh
{
public:

	bool HasNormals() const;
	bool HasTangents() const;
	bool HasTexCoords() const;

	typedef std::pair<int, int> Range;

	void AddGroup(const std::string& name, const std::string& material, const Range& idxRange);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec4> tangents;
	std::vector<glm::vec2> texCoords;
	
	std::vector<WavefrontObjMaterial_ptr> materials;

	std::vector<Range> groupRanges;
	std::vector<std::string> groupMaterial;
	std::vector<std::string> groupNames;

	std::string name;
	std::string meshPath;
};

/**
\brief This class is a data container with a memory layout ready to use with indexed vertex bufer objects (VBO)
*/
class OpenGLRawMesh : public AbstractRawMesh
{
public:
	SHARED_PTR_FACTORY(OpenGLRawMesh);

	bool ComputeNormals();

	bool ComputeTangents();

	bool ComputeBoundingBox(AABBox& box);

	std::vector<int> triangleIndices;	
};

/**
 \brief This class is a data container with indexed geometry such as used by Wavefront OBJ or Collada
*/
class IndexedRawMesh : public AbstractRawMesh
{
public:
	SHARED_PTR_FACTORY(IndexedRawMesh);

	IndexedRawMesh();

	~IndexedRawMesh();
	
	OpenGLRawMesh_ptr ConvertToOpenGLMesh() const;

	std::vector<Tri> faces;

	bool ComputeNormals();

	bool ComputeTangents();
};

