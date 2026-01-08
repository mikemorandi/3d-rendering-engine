
#include "BoundingBoxUtil.h"
#include "BoundingBox.h"

#include <algorithm>
#include <iterator>
#include <limits>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/orthonormalize.hpp>

CoordinateFrame BoundingBoxUtil::BasisFromDirection(const glm::vec3 & direction)
{
	auto directionNormalized = glm::normalize(direction);

	// Find directional component with least extent
	int minDirIndex = -1;
	float ldMin = std::numeric_limits<float>::max();
	for (int i = 0; i < 3; i++)
	{
		if (float current_comp = std::abs(directionNormalized[i]); current_comp < ldMin)
		{
			minDirIndex = i;
			ldMin = current_comp;
		}
	}
	assert(minDirIndex >= 0 && minDirIndex < 3);

	glm::vec3 sideVec0(0,0,0);
	sideVec0[minDirIndex] = 1;
	sideVec0 = glm::orthonormalize(sideVec0, directionNormalized);

	glm::vec3 sideVec1 = glm::cross(directionNormalized, sideVec0);
	return CoordinateFrame(directionNormalized, sideVec1, sideVec0);
}

bool BoundingBoxUtil::DirectionalLightFrustum(const AABBox & bbox, const glm::vec3& lightDir, OrthogonalFrustum& frust)
{
	auto get_corner = [&bbox](bool a, bool b, bool c)
	{
		glm::vec3 corner = bbox.p;
		corner[0] += (bbox.d[0] * (a ? 1.f : -1.f) );
		corner[1] += (bbox.d[1] * (b ? 1.f : -1.f) );
		corner[2] += (bbox.d[2] * (c ? 1.f : -1.f) );
		return corner;
	};

	glm::vec3 lightDirNormalized = glm::normalize(lightDir);

	bool has_comp[3];
	has_comp[0] = std::abs(lightDirNormalized[0]) > 1e-5;
	has_comp[1] = std::abs(lightDirNormalized[1]) > 1e-5;
	has_comp[2] = std::abs(lightDirNormalized[2]) > 1e-5;

	size_t num_corr = std::count(std::cbegin(has_comp), std::cend(has_comp), true);

	if (num_corr == 1)
	{
		auto it = std::find(std::cbegin(has_comp), std::cend(has_comp), true);
		int idx = static_cast<int>(std::distance(std::cbegin(has_comp),it));
		float sign = std::signbit(lightDirNormalized[idx]) ? 1.f : -1.f;

		frust.position = bbox.p;
		frust.position[idx] += (bbox.d[idx] * sign);
		frust.nearPlane = 0.0f;
		frust.farPlane = 2 * bbox.d[idx];

		int axis1 = (idx + 1) % 3;
		int axis2 = (idx + 2) % 3;

		frust.left = -bbox.d[axis1];
		frust.right = bbox.d[axis1];
		frust.top = bbox.d[axis2];
		frust.bottom = -bbox.d[axis2];

		frust.frame[0] = lightDir;
		glm::vec3 basisv1(0);
		basisv1[axis1] = 1;
		glm::vec3 basisv2(0);
		basisv2[axis2] = 1;

		frust.frame[1] = basisv1;
		frust.frame[2] = basisv2;

		return true;
	}
	else if (num_corr == 2 || num_corr == 3)
	{
		auto it = std::find(std::cbegin(has_comp), std::cend(has_comp), false);
		int axis0 = static_cast<int>(std::distance(std::cbegin(has_comp), it));
		(void)axis0;  // Used for indexing calculation below

		frust.position = get_corner(has_comp[0], has_comp[1], has_comp[2]); // position
		frust.frame = BasisFromDirection(lightDirNormalized);

		auto project2plane = [&frust] (const glm::vec3& p, float& a, float& b)
		{
			glm::vec3 dv = p - frust.position;
			a = glm::dot(dv, frust.frame[1]);
			b = glm::dot(dv, frust.frame[2]);
		};

		float a_min = std::numeric_limits<float>::max();
		float b_min = std::numeric_limits<float>::max();
		float a_max = std::numeric_limits<float>::min();
		float b_max = std::numeric_limits<float>::min();

		float a, b;

		project2plane(get_corner(false, false, false), a, b);
		a_min = std::min(a_min, a); b_min = std::min(b_min, b);
		a_max = std::max(a_max, a); b_max = std::max(b_max, b);
		project2plane(get_corner(false, false, true),  a, b);
		a_min = std::min(a_min, a); b_min = std::min(b_min, b);
		a_max = std::max(a_max, a); b_max = std::max(b_max, b);
		project2plane(get_corner(false, true,  false), a, b);
		a_min = std::min(a_min, a); b_min = std::min(b_min, b);
		a_max = std::max(a_max, a); b_max = std::max(b_max, b);
		project2plane(get_corner(false, true,  true),  a, b);
		a_min = std::min(a_min, a); b_min = std::min(b_min, b);
		a_max = std::max(a_max, a); b_max = std::max(b_max, b);
		project2plane(get_corner(true,  false, false), a, b);
		a_min = std::min(a_min, a); b_min = std::min(b_min, b);
		a_max = std::max(a_max, a); b_max = std::max(b_max, b);
		project2plane(get_corner(true,  false, true),  a, b);
		a_min = std::min(a_min, a); b_min = std::min(b_min, b);
		a_max = std::max(a_max, a); b_max = std::max(b_max, b);
		project2plane(get_corner(true,  true,  false), a, b);
		a_min = std::min(a_min, a); b_min = std::min(b_min, b);
		a_max = std::max(a_max, a); b_max = std::max(b_max, b);
		project2plane(get_corner(true,  true,  true),  a, b);
		a_min = std::min(a_min, a); b_min = std::min(b_min, b);
		a_max = std::max(a_max, a); b_max = std::max(b_max, b);

		frust.left = a_min;
		frust.right = a_max;
		frust.top = b_max;
		frust.bottom = b_min;
		frust.nearPlane = 0;
		frust.farPlane = glm::length(bbox.d) * 2.f;

		return true;

	}

	return false;
}

bool BoundingBoxUtil::DirectionalLightFrustum(const AABBox & box, const glm::vec3 & lightDir, const Frustum & camFrustum, OrthogonalFrustum & frustum)
{
	auto cam_points = camFrustum.CornerPoints();

	glm::vec3 lightDirNormalized = glm::normalize(lightDir);
	frustum.frame = BasisFromDirection(lightDirNormalized);

	// Transform camera frustum points to light space
	std::transform(cam_points.begin(), cam_points.end(), cam_points.begin(),
		[&frustum](const glm::vec3& v) { return frustum.frame * v; });

	// Initialize bounds with camera frustum points
	frustum.left = std::numeric_limits<glm::vec3::value_type>::max();
	frustum.right = -std::numeric_limits<glm::vec3::value_type>::max();
	frustum.top = -std::numeric_limits<glm::vec3::value_type>::max();
	frustum.bottom = std::numeric_limits<glm::vec3::value_type>::max();
	float nearZ = std::numeric_limits<glm::vec3::value_type>::max();
	float farZ = -std::numeric_limits<glm::vec3::value_type>::max();

	for (const auto& p : cam_points)
	{
		frustum.left = std::min(frustum.left, p[CoordinateFrame::SideAxis]);
		frustum.right = std::max(frustum.right, p[CoordinateFrame::SideAxis]);
		frustum.bottom = std::min(frustum.bottom, p[CoordinateFrame::UpAxis]);
		frustum.top = std::max(frustum.top, p[CoordinateFrame::UpAxis]);
		nearZ = std::min(nearZ, p[CoordinateFrame::ViewAxis]);
		farZ = std::max(farZ, p[CoordinateFrame::ViewAxis]);
	}

	// Include bounding box corners in the calculation to constrain the frustum
	auto get_corner = [&box](bool a, bool b, bool c)
	{
		glm::vec3 corner = box.p;
		corner[0] += (box.d[0] * (a ? 1.f : -1.f));
		corner[1] += (box.d[1] * (b ? 1.f : -1.f));
		corner[2] += (box.d[2] * (c ? 1.f : -1.f));
		return corner;
	};

	// Transform all 8 corners of the bounding box to light space and update bounds
	std::array<glm::vec3, 8> box_corners = {
		frustum.frame * get_corner(false, false, false),
		frustum.frame * get_corner(false, false, true),
		frustum.frame * get_corner(false, true, false),
		frustum.frame * get_corner(false, true, true),
		frustum.frame * get_corner(true, false, false),
		frustum.frame * get_corner(true, false, true),
		frustum.frame * get_corner(true, true, false),
		frustum.frame * get_corner(true, true, true)
	};

	for (const auto& p : box_corners)
	{
		frustum.left = std::min(frustum.left, p[CoordinateFrame::SideAxis]);
		frustum.right = std::max(frustum.right, p[CoordinateFrame::SideAxis]);
		frustum.bottom = std::min(frustum.bottom, p[CoordinateFrame::UpAxis]);
		frustum.top = std::max(frustum.top, p[CoordinateFrame::UpAxis]);
		nearZ = std::min(nearZ, p[CoordinateFrame::ViewAxis]);
		farZ = std::max(farZ, p[CoordinateFrame::ViewAxis]);
	}

	frustum.nearPlane = nearZ;
	frustum.farPlane = farZ;

	return true;
}
