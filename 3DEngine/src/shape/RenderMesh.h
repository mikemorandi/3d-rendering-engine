#pragma once

#include "Shape.h"
#include "../enums.h"
#include <vector>
#include <map>
#include <array>
#include <string>
#include <memory>

#include "../core/gl.h"

class OpenGLRawMesh;
using OpenGLRawMeshPtr = std::shared_ptr<OpenGLRawMesh>;
using OpenGLRawMeshConstPtr = std::shared_ptr<const OpenGLRawMesh>;
using OpenGLRawMeshWeakPtr = std::weak_ptr<OpenGLRawMesh>;

class RenderMesh;
using RenderMeshPtr = std::shared_ptr<RenderMesh>;
using RenderMeshConstPtr = std::shared_ptr<const RenderMesh>;
using RenderMeshWeakPtr = std::weak_ptr<RenderMesh>;

class WavefrontObjMaterial;
using WavefrontObjMaterialPtr = std::shared_ptr<WavefrontObjMaterial>;
using WavefrontObjMaterialConstPtr = std::shared_ptr<const WavefrontObjMaterial>;
using WavefrontObjMaterialWeakPtr = std::weak_ptr<WavefrontObjMaterial>;

class Scene;
using ScenePtr = std::shared_ptr<Scene>;
using SceneConstPtr = std::shared_ptr<const Scene>;
using SceneWeakPtr = std::weak_ptr<Scene>;

class Texture2D;
using Texture2DPtr = std::shared_ptr<Texture2D>;
using Texture2DConstPtr = std::shared_ptr<const Texture2D>;
using Texture2DWeakPtr = std::weak_ptr<Texture2D>;

class ShaderBase;
using ShaderBasePtr = std::shared_ptr<ShaderBase>;
using ShaderBaseConstPtr = std::shared_ptr<const ShaderBase>;
using ShaderBaseWeakPtr = std::weak_ptr<ShaderBase>;

class MaterialShader;
using MaterialShaderPtr = std::shared_ptr<MaterialShader>;
using MaterialShaderConstPtr = std::shared_ptr<const MaterialShader>;
using MaterialShaderWeakPtr = std::weak_ptr<MaterialShader>;

class RenderMesh : public Shape
{
public:

	static RenderMeshPtr Create(const OpenGLRawMeshPtr&);

	RenderMesh(const OpenGLRawMeshPtr& rawMesh);

	virtual void Render(const ScenePtr& scene) const override;
	virtual void RenderGeometry(const ShaderBasePtr& shader) const override;
	virtual void Init() override; 

	typedef std::pair<int, int> IntPair;
	typedef std::vector<IntPair> IntPairVector;

	[[nodiscard]] bool SetPositions(const std::vector<glm::vec3>& positions, const std::vector<int>& indices, IntPairVector* indexGroups = nullptr);
	[[nodiscard]] bool SetNormals(const std::vector<glm::vec3>& normals);
	[[nodiscard]] bool SetTangents(const std::vector<glm::vec4>& tangents);
	[[nodiscard]] bool SetTextureCoordinates(const std::vector<glm::vec2>& texCoords);
	[[nodiscard]] bool SetColors(const std::vector<float>& colors);

	[[nodiscard]] bool IsInitialized() const { return initialized; };
	bool MapVertexAttribute(GLSLShader::VertexAttribute attrib, GLuint channel) const;

	enum class DrawMode : unsigned int
	{ 
		Triangle = static_cast<unsigned int>(GL_TRIANGLES),
		Lines = static_cast<unsigned int>(GL_LINES),
		Point = static_cast<unsigned int>(GL_POINTS)
	};

	void SetDrawingMode(DrawMode mode);

protected:

	RenderMesh(DrawMode = DrawMode::Triangle);

	void InitFromRawMesh(const OpenGLRawMeshPtr& rawMesh);

	/// Map vertex attributes to correct channels
	void MapVertexAttributes(ShaderBasePtr shader) const;

	/// Naked draw commands without shaders
	void Draw(const size_t& group) const;

	inline void SetAttribPointer(const GLSLShader::VertexAttribute& attrib) const;

	struct VertexAttribData
	{
		GLuint channel;
		GLint  size;
	};

	GLuint									vaoHandle;
	DrawMode								drawMode;
	int										primitiveSize;
	std::string								name;

	std::unique_ptr<GLuint[]>				bufferObjects;
	std::unique_ptr<GLuint[]>				indexBufferObjects;
	std::unique_ptr<VertexAttribData[]>		vAttribData;

	std::vector< std::pair<int,int> >		ranges;
	std::vector<MaterialPtr>				materialsNew;

	std::array<bool,6>						vtxAttribSet;
	bool									initialized;
	size_t									vertexCount;
};

