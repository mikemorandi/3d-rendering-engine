#pragma once

#define GLM_FORCE_SWIZZLE
#include <glm/fwd.hpp>
#include <glm/mat3x3.hpp>

class Ray;

class AABBox
{
public:
	AABBox();

	AABBox(const glm::vec3& lower, const glm::vec3& upper);

	virtual glm::vec3 Minimum() const;

	virtual glm::vec3 Maximum() const;

	/// Transforms the bbox and returns it; the current instance remains untouched
	virtual AABBox Transform(const glm::mat4& t) const;

	/// Creates the hull bounding box of this box an the passed box
	virtual AABBox HullBox(const AABBox& other) const;

	bool Intersect(const Ray &r, float& tmin, float& tmax) const;

	inline void operator*=(const glm::mat4& rhs);

	inline void operator+=(const AABBox& rhs) { *this = HullBox(rhs); };

	inline AABBox operator+(const AABBox& rhs) { return HullBox(rhs); }

	inline bool operator==(const AABBox& other) {
		return d == other.d && p == other.p;
	};	

	inline bool operator!=(const AABBox& other) {
		return !(*this == other);
	}

	enum Axis { X = 0, Y = 1, Z = 2 };

	glm::vec3 d;
	glm::vec3 p;
};

inline AABBox operator*(const glm::mat4& lhs, const AABBox& rhs) {
	return rhs.Transform(lhs);
}

/// Oriented bounding box
class OBBox : public AABBox
{
public:

	OBBox();

	OBBox(const glm::vec3& lower, const glm::vec3& upper);

	virtual glm::vec3 Minimum() const override;

	virtual glm::vec3 Maximum() const override;

	glm::mat3 R;

};