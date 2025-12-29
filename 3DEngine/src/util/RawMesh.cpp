#include "stdafx.h"

#include "RawMesh.h"

#include "../math/BoundingBox.h"

#include <unordered_map>

WavefrontObjMaterial_ptr WavefrontObjMaterial::Create(std::string name)
{
	return  std::make_shared<WavefrontObjMaterial>(name);
}

WavefrontObjMaterial::WavefrontObjMaterial(std::string theName)
	: opacity(1)
	, specularEnabled(true)
	, name(theName)
{

}

bool WavefrontObjMaterial::HasTextures() const
{
	return !ambientColorTexture.empty()
		|| !diffuseColorTexture.empty()
		|| !specularColorTexture.empty()
		|| !specularHightlightTexture.empty()
		|| !alphaMapTexture.empty()
		|| !bumpMapTexture.empty()
		|| !displacementMapTexture.empty();
}

bool WavefrontObjMaterial::HasBumpMap() const
{
	return bumpMapTexture.empty();
}

bool WavefrontObjMaterial::HasDisplacementMap() const
{
	return displacementMapTexture.empty();
}

IndexedRawMesh::IndexedRawMesh()
{
}


IndexedRawMesh::~IndexedRawMesh()
{
	
}

bool AbstractRawMesh::HasNormals() const
{
	return normals.size() == vertices.size();
}

bool AbstractRawMesh::HasTangents() const
{
	return tangents.size() == vertices.size();
}

bool AbstractRawMesh::HasTexCoords() const
{
	return texCoords.size() > 0;
}

void AbstractRawMesh::AddGroup(const std::string& name, const std::string& material, const Range& idxRange)
{
	groupRanges.push_back(idxRange);
	groupNames.push_back(name);
	groupMaterial.push_back(material);
}

bool IndexedRawMesh::ComputeNormals()
{
	//see http://mrl.nyu.edu/~perlin/courses/fall2002/meshnormals.html

	//ReInit normals to zero
	normals.clear();
	normals.resize(vertices.size());	

	for (int i = 0; i < faces.size(); i++)
	{
		int aIdx = faces[i].v[0][Tri::pos];
		int bIdx = faces[i].v[1][Tri::pos];
		int cIdx = faces[i].v[2][Tri::pos];
							   
		glm::vec3& a = vertices[aIdx];
		glm::vec3& b = vertices[bIdx];
		glm::vec3& c = vertices[cIdx];

		glm::vec3 e1 = b - a;
		glm::vec3 e2 = c - b;
		glm::vec3 e3 = a - c;

		glm::vec3 perpVector = glm::cross(e3, e1);
		normals[aIdx] += perpVector;

		perpVector = glm::cross(e1, e2);
		normals[bIdx] += perpVector;

		perpVector = glm::cross(e2, e3);
		normals[cIdx] += perpVector;

		//Update vertex indices
		faces[i].v[0][Tri::normal] = aIdx;
		faces[i].v[1][Tri::normal] = bIdx;
		faces[i].v[2][Tri::normal] = cIdx;
	}

	//Normalize normals (unit-length)
	for (int i = 0; i < normals.size(); i++) {
		if (glm::length(normals[i]) > 0)
			normals[i] = glm::normalize(normals[i]);
	}

	return true;
}

bool IndexedRawMesh::ComputeTangents()
{
	//see http://www.terathon.com/code/tangent.html for details

	if (!(HasNormals() && HasTexCoords()))
		return false;

	if (texCoords.size() != vertices.size())
	{
		Error("ObjLoader: #tex coords does not equal #vertices");
		return false;
	}

	//ReInit normals to zero
	tangents.clear();
	tangents.resize(vertices.size());

	std::vector<glm::vec3> tangents_tmp(vertices.size());
	tangents_tmp.resize(vertices.size());

	std::vector<glm::vec3> bitangents(vertices.size());
	bitangents.resize(vertices.size());

	//Accumulate tangent directions
	for (int i = 0; i < faces.size(); i++)
	{
		auto& v1Idx = faces[i].v[0];
		auto& v2Idx = faces[i].v[1];
		auto& v3Idx = faces[i].v[2];

		if (v1Idx[Tri::tex_uv] >= 0 && v2Idx[Tri::tex_uv] >= 0 && v3Idx[Tri::tex_uv] >= 0)
		{
			glm::vec2& tc1 = texCoords[v1Idx[Tri::tex_uv]];
			glm::vec2& tc2 = texCoords[v2Idx[Tri::tex_uv]];
			glm::vec2& tc3 = texCoords[v3Idx[Tri::tex_uv]];

			glm::vec3& v1 = vertices[v1Idx[Tri::pos]];
			glm::vec3& v2 = vertices[v2Idx[Tri::pos]];
			glm::vec3& v3 = vertices[v3Idx[Tri::pos]];

			//difference vectors
			glm::vec3 q1 = v2 - v1;
			glm::vec3 q2 = v3 - v1;

			//difference tex coords
			glm::vec2 dTc1 = tc2 - tc1;
			glm::vec2 dTc2 = tc3 - tc1;

			//Solve for tangents and bitangents
			glm::mat3x2 Q = glm::transpose(glm::mat2x3(q1, q2));
			glm::mat2 TX_1 = glm::inverse(glm::mat2(dTc1, dTc2));
			glm::mat2x3 R = glm::transpose(TX_1 * Q);

			glm::vec3 sdir = R[0];
			glm::vec3 tdir = R[1];

			if (!std::isnan(sdir.x) && !std::isnan(sdir.y) && !std::isnan(sdir.z))
			{
				tangents_tmp[v1Idx[Tri::pos]] += sdir;
				tangents_tmp[v2Idx[Tri::pos]] += sdir;
				tangents_tmp[v3Idx[Tri::pos]] += sdir;
			}

			if (!std::isnan(tdir.x) && !std::isnan(tdir.y) && !std::isnan(tdir.z))
			{
				bitangents[v1Idx[Tri::pos]] += tdir;
				bitangents[v2Idx[Tri::pos]] += tdir;
				bitangents[v3Idx[Tri::pos]] += tdir;
			}
		}
		else
		{
			tangents.clear();
			bitangents.clear();
			return false;
		}
	}

	//Orthogonalize and normalize
	for (int i = 0; i < vertices.size(); i++)
	{
		const glm::vec3& n = normals[i];
		const glm::vec3& t = tangents_tmp[i];
		const glm::vec3& bt = bitangents[i];

		glm::vec3 gm_tmp = t - n * glm::dot(n, t);

		if (glm::length(gm_tmp) > 0)
		{
			// Gram-Schmidt orthogonalization
			tangents[i] = glm::vec4(glm::normalize(gm_tmp), 1.0);

			// Calculate handedness
			tangents[i].w = (glm::dot(glm::cross(n, t), bt) < 0.0f) ? -1.0f : 1.0f;
		}
		else
		{
			tangents[i] = glm::normalize(glm::vec4(1, 1, 1, 1));
			tangents[i].w = 1;
			Warn("Tangent could not be computed");
		}
	}

	return true;
}

OpenGLRawMesh_ptr IndexedRawMesh::ConvertToOpenGLMesh() const
{
	OpenGLRawMesh_ptr gl_mesh = OpenGLRawMesh::Create();

	gl_mesh->name = name;
	gl_mesh->meshPath = meshPath;
	gl_mesh->materials = materials;
	gl_mesh->groupMaterial = groupMaterial;
	gl_mesh->groupNames = groupNames;
	gl_mesh->groupRanges = groupRanges;

	gl_mesh->vertices.reserve(vertices.size());
	gl_mesh->texCoords.reserve(vertices.size());
	gl_mesh->normals.reserve(vertices.size());
	gl_mesh->triangleIndices.reserve(faces.size());

	std::unordered_map<int, std::unordered_map<int, int> > hash_idx_table;

	int duplicates = 0;

	for (auto& t : faces)
	{
		for (int i = 0; i < 3; i++)
		{
			auto& pos_idx = t.v[i][Tri::pos];
			auto& uv_idx = t.v[i][Tri::tex_uv];
			auto& normal_idx = t.v[i][Tri::normal];

			int current_index;
			bool found = false;

			auto hashmap_it = hash_idx_table.find(pos_idx);
			if (hashmap_it != hash_idx_table.end())
			{
				auto hit2 = hashmap_it->second.find(uv_idx);
				if (hit2 != hashmap_it->second.end())
				{
					found = true;
					current_index = hit2->second;
					duplicates++;
				}
			}

			if (!found)
			{
				gl_mesh->vertices.push_back(vertices[pos_idx]);

				if (uv_idx >= 0 && texCoords.size() > uv_idx)
					gl_mesh->texCoords.push_back(texCoords[uv_idx]);

				if (normal_idx >= 0 && normals.size() > normal_idx)
					gl_mesh->normals.push_back(normals[normal_idx]);

				current_index = static_cast<int>(gl_mesh->vertices.size() - 1);

				auto hIt = hash_idx_table.find(pos_idx);

				if (hIt == hash_idx_table.end())			
					hash_idx_table[pos_idx] = std::unordered_map<int, int>();
				
				hash_idx_table[pos_idx][uv_idx] = current_index;
			}

			gl_mesh->triangleIndices.push_back(current_index);	
		}
	}	

	gl_mesh->vertices.shrink_to_fit();
	gl_mesh->texCoords.shrink_to_fit();
	gl_mesh->normals.shrink_to_fit();
	gl_mesh->triangleIndices.shrink_to_fit();

	return gl_mesh;
}

bool OpenGLRawMesh::ComputeNormals()
{
	//see http://mrl.nyu.edu/~perlin/courses/fall2002/meshnormals.html

	//ReInit normals to zero
	normals.clear();
	normals.resize(vertices.size());

	for (int i = 0; i < triangleIndices.size(); i=i+3)
	{
		int& aIdx = triangleIndices[i];
		int& bIdx = triangleIndices[i + 1];
		int& cIdx = triangleIndices[i + 2];

		glm::vec3& a = vertices[aIdx];
		glm::vec3& b = vertices[bIdx];
		glm::vec3& c = vertices[cIdx];

		glm::vec3 e1 = b - a;
		glm::vec3 e2 = c - b;
		glm::vec3 e3 = a - c;

		glm::vec3 perpVector = glm::cross(e3, e1);
		normals[aIdx] += perpVector;

		perpVector = glm::cross(e1, e2);
		normals[bIdx] += perpVector;

		perpVector = glm::cross(e2, e3);
		normals[cIdx] += perpVector;
	}

	//Normalize normals (unit-length)
	for (int i = 0; i < normals.size(); i++) {
		if (glm::length(normals[i]) > 0)
			normals[i] = glm::normalize(normals[i]);
	}

	return true;
}

bool OpenGLRawMesh::ComputeTangents()
{
	//see http://www.terathon.com/code/tangent.html for details

	if (!(HasNormals() && HasTexCoords()))
		return false;

	if (texCoords.size() != vertices.size())
	{
		Error("OpenGLRawMesh::ComputeTangents(): #tex coords does not equal #vertices");
		return false;
	}

	//ReInit normals to zero
	tangents.clear();
	tangents.resize(vertices.size());

	std::vector<glm::vec3> tangents_tmp(vertices.size());
	tangents_tmp.resize(vertices.size());

	std::vector<glm::vec3> bitangents(vertices.size());
	bitangents.resize(vertices.size());

	//Accumulate tangent directions
	for (int i = 0; i < triangleIndices.size(); i=i+3)
	{
		auto& v1Idx = triangleIndices[i];
		auto& v2Idx = triangleIndices[i+1];
		auto& v3Idx = triangleIndices[i+2];

		if (v1Idx >= 0 && v2Idx >= 0 && v3Idx >= 0)
		{
			glm::vec2& tc1 = texCoords[v1Idx];
			glm::vec2& tc2 = texCoords[v2Idx];
			glm::vec2& tc3 = texCoords[v3Idx];

			glm::vec3& v1 = vertices[v1Idx];
			glm::vec3& v2 = vertices[v2Idx];
			glm::vec3& v3 = vertices[v3Idx];

			//difference vectors
			glm::vec3 q1 = v2 - v1;
			glm::vec3 q2 = v3 - v1;

			//difference tex coords
			glm::vec2 dTc1 = tc2 - tc1;
			glm::vec2 dTc2 = tc3 - tc1;

			//Solve for tangents and bitangents
			glm::mat3x2 Q = glm::transpose(glm::mat2x3(q1, q2));
			glm::mat2 TX_1 = glm::inverse(glm::mat2(dTc1, dTc2));
			glm::mat2x3 R = glm::transpose(TX_1 * Q);

			glm::vec3 sdir = R[0];
			glm::vec3 tdir = R[1];

			if (!std::isnan(sdir.x) && !std::isnan(sdir.y) && !std::isnan(sdir.z))
			{
				tangents_tmp[v1Idx] += sdir;
				tangents_tmp[v2Idx] += sdir;
				tangents_tmp[v3Idx] += sdir;
			}

			if (!std::isnan(tdir.x) && !std::isnan(tdir.y) && !std::isnan(tdir.z))
			{
				bitangents[v1Idx] += tdir;
				bitangents[v2Idx] += tdir;
				bitangents[v3Idx] += tdir;
			}
		}
		else
		{
			tangents.clear();
			bitangents.clear();
			return false;
		}
	}

	//Orthogonalize and normalize
	for (int i = 0; i < vertices.size(); i++)
	{
		const glm::vec3& n = normals[i];
		const glm::vec3& t = tangents_tmp[i];
		const glm::vec3& bt = bitangents[i];

		glm::vec3 gm_tmp = t - n * glm::dot(n, t);

		if (glm::length(gm_tmp) > 0)
		{
			// Gram-Schmidt orthogonalization
			tangents[i] = glm::vec4(glm::normalize(gm_tmp), 1.0);

			// Calculate handedness
			tangents[i].w = (glm::dot(glm::cross(n, t), bt) < 0.0f) ? -1.0f : 1.0f;
		}
		else
		{
			tangents[i] = glm::normalize(glm::vec4(1, 1, 1, 1));
			tangents[i].w = 1;
			Warn("Tangent could not be computed");
		}
	}

	return true;
}

bool OpenGLRawMesh::ComputeBoundingBox(AABBox& box)
{
	float min_inf = std::numeric_limits<float>::min();
	float max_inf = std::numeric_limits<float>::max();

	glm::vec3 lower_left(max_inf, max_inf, max_inf), upper_right(min_inf, min_inf, min_inf);

	for (const glm::vec3& v : vertices)
	{
		for (int i = 0; i < 3; i++)
		{
			if (v[i] < lower_left[i])
				lower_left[i] = v[i];
			if (v[i] > upper_right[i])
				upper_right[i] = v[i];
		}
	}

	bool ok = true;
	for (int i = 0; i < 3; i++)
		ok &= (upper_right[i] != min_inf && lower_left[i] != max_inf);

	if (ok)
		box = OBBox(lower_left, upper_right);

	return ok;
}
