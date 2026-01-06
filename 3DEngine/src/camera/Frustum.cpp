#include "Frustum.h"

#include <glm/glm.hpp>

std::array<glm::vec3, 8> PerspectiveFrustum::CornerPoints() const
{
	std::array<glm::vec3, 8> points;

	float fovX = fovY * aspectRatio;
	float tan_fovY_half = std::tan(glm::radians(fovY /2.f));
	float tan_fovX_half = std::tan(glm::radians(fovX /2.f));

	float near_up = tan_fovY_half * nearPlane;
	float far_up = tan_fovY_half * farPlane;
	float near_side = tan_fovX_half * nearPlane;
	float far_side = tan_fovX_half * farPlane;

	points[0] = glm::vec3(nearPlane, near_up	, near_side);
	points[1] = glm::vec3(nearPlane, near_up	,-near_side);
	points[2] = glm::vec3(nearPlane,-near_up	,near_side);
	points[3] = glm::vec3(nearPlane,-near_up	,-near_side);
	points[4] = glm::vec3(farPlane,	 far_up,	  far_side);
	points[5] = glm::vec3(farPlane,	 far_up,	 -far_side);
	points[6] = glm::vec3(farPlane,	-far_up,	  far_side);
	points[7] = glm::vec3(farPlane,	-far_up,	 -far_side);

	std::transform(points.begin(), points.end(), points.begin(), 
		[this](glm::vec3& v) { return (frame * v); });

	return points;
}

std::array<glm::vec3, 8> OrthogonalFrustum::CornerPoints() const
{
	std::array<glm::vec3, 8> points;

	points[0] = glm::vec3(nearPlane, top  , right);
	points[1] = glm::vec3(nearPlane, top  , left);
	points[2] = glm::vec3(nearPlane, bottom, right);
	points[3] = glm::vec3(nearPlane, bottom, left);
	points[4] = glm::vec3(farPlane,  top  , right);
	points[5] = glm::vec3(farPlane,  top  , left );
	points[6] = glm::vec3(farPlane,  bottom, right);
	points[7] = glm::vec3(farPlane,  bottom, left );

	std::transform(points.begin(), points.end(), points.begin(),
		[this](glm::vec3& v) { return (frame * v); });

	return points;
}

Frustum::Frustum()
	: nearPlane(1.f)
	, farPlane(100.f)
	, position(0,0,0)
{
}
