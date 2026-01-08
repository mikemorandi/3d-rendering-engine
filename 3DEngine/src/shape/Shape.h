#pragma once

#include <glm/mat4x4.hpp>

#include "../rendering/Renderable.h"

#include "../math/BoundingBox.h"

class Scene;

// Forward declarations
class Shape;
using ShapePtr = std::shared_ptr<Shape>;
using ShapeConstPtr = std::shared_ptr<const Shape>;
using ShapeWeakPtr = std::weak_ptr<Shape>;

class Material;
using MaterialPtr = std::shared_ptr<Material>;
using MaterialConstPtr = std::shared_ptr<const Material>;
using MaterialWeakPtr = std::weak_ptr<Material>;

class ShaderBase;
using ShaderBasePtr = std::shared_ptr<ShaderBase>;
using ShaderBaseConstPtr = std::shared_ptr<const ShaderBase>;
using ShaderBaseWeakPtr = std::weak_ptr<ShaderBase>;

class Shape : public Renderable
{
public:
	virtual ~Shape();

	virtual void Init() = 0;

	void SetMaterial(const MaterialPtr& material) { this->material = material; };

	MaterialPtr Material() const { return material; };

	virtual void SetWorldTransform(const glm::mat4& t);
	const glm::mat4& WorldTransform() const;

	/// Returns the bounding box in world coordinates
	virtual AABBox BoundingBox() const;

	// Shadow mapping support
	void SetCastsShadows(bool enabled) { castsShadows = enabled; }
	bool CastsShadows() const { return castsShadows; }

	/// Render geometry using a specific shader (for shadow mapping, etc.)
	/// The shader should already have Use() called before this method
	virtual void RenderGeometry(const ShaderBasePtr& shader) const { (void)shader; }

protected:

	Shape();

	Shape(const Shape& rhs);

	MaterialPtr material;
	AABBox bboxModelSpace;
	glm::mat4 worldTransform;

private:
	mutable bool boundingBoxDirty;
	mutable AABBox cachedWorldBBox;
	bool castsShadows;
};

