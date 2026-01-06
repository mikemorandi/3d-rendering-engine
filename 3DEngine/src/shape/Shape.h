#pragma once

#include <glm/mat4x4.hpp>

#include "../util/SharedPointer.h"
#include "../rendering/Renderable.h"

#include "../math/BoundingBox.h"

class Scene;

SHARED_PTR_CLASS_DECL(Shape);
SHARED_PTR_CLASS_DECL(Material);
SHARED_PTR_CLASS_DECL(ShaderBase);

class Shape : public Renderable
{
public:
	virtual ~Shape();

	virtual void Init() = 0;

	void SetMaterial(const Material_ptr& material) { this->material = material; };

	Material_ptr Material() const { return material; };

	virtual void SetWorldTransform(const glm::mat4& t);
	const glm::mat4& WorldTransform() const;

	/// Returns the bounding box in world coordinates
	virtual AABBox BoundingBox() const;

	// Shadow mapping support
	void SetCastsShadows(bool enabled) { castsShadows = enabled; }
	bool CastsShadows() const { return castsShadows; }

	/// Render geometry using a specific shader (for shadow mapping, etc.)
	/// The shader should already have Use() called before this method
	virtual void RenderGeometry(const ShaderBase_ptr& shader) const { (void)shader; }

protected:

	Shape();

	Shape(const Shape& rhs);

	Material_ptr material;
	AABBox bboxModelSpace;
	glm::mat4 worldTransform;

private:
	mutable bool boundingBoxDirty;
	mutable AABBox cachedWorldBBox;
	bool castsShadows;
};

