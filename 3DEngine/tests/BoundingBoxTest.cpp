#include <gtest/gtest.h>

#include "../src/math/BoundingBox.h"
#include "../src/math/BoundingBoxUtil.h"
#include "../src/camera/Frustum.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Test3DEngine
{
	// BoundingBox Tests
	TEST(BoundingBoxTests, Creation)
	{
		AABBox box0;
		EXPECT_EQ(box0.d, glm::vec3(0,0,0));
		EXPECT_EQ(box0.p, glm::vec3(0, 0, 0));

		glm::vec3 v0(1, 2, 3);
		glm::vec3 v1(4, 6, 8);
		AABBox box(v0, v1);

		EXPECT_EQ(box.d, glm::vec3(1.5f, 2.f, 2.5f)) << "d is wrong";
		EXPECT_EQ(box.p, v0 + box.d) << "p is wrong";

		EXPECT_EQ(v0, box.Minimum());
		EXPECT_EQ(v1, box.Maximum());
	}

	TEST(BoundingBoxTests, HullBox)
	{
		glm::vec3 v00(1, 2, 3);
		glm::vec3 v01(4, 6, 8);
		glm::vec3 v10(-1, -2, -3);
		glm::vec3 v11(10, 0, -2);

		AABBox box0(v00, v01);
		AABBox box1(v10, v11);

		AABBox hull_box = box0.HullBox(box1);

		EXPECT_EQ(v10, hull_box.Minimum());
		EXPECT_EQ(glm::vec3(10, 6, 8), hull_box.Maximum());

		AABBox hull_box2 = box0 + box1;
		EXPECT_EQ(hull_box, hull_box2);
	}

	TEST(BoundingBoxTests, Transform)
	{
		glm::vec3 v0(-1, -2, -3);
		glm::vec3 v1(3, 2, 1);
		AABBox box(v0, v1);

		//rot 90 deg around x
		glm::mat4 rmat(glm::vec4(1, 0, 0, 0),
			glm::vec4(0, 0, 1, 0),
			glm::vec4(0, -1, 0, 0),
			glm::vec4(0, 0, 0, 1));
		auto tbox = box.Transform(rmat);
		EXPECT_EQ(glm::vec3(-1, -1, -2), tbox.Minimum());

		auto tbox2 = rmat * box;
		EXPECT_EQ(tbox2, tbox);
	}

	// BoundingBoxUtil Tests
	TEST(BoundingBoxUtilTests, DirectionalLightFrustumOrtho)
	{
		AABBox box;
		box.d = glm::vec3(1, 2, 3);
		glm::vec3 ldir(0, -1, 0);

		OrthogonalFrustum Frustum;
		EXPECT_TRUE(BoundingBoxUtil::DirectionalLightFrustum(box, ldir, Frustum));
		EXPECT_EQ(glm::vec3(0, 2, 0), Frustum.position);
		EXPECT_FLOAT_EQ(0.f, Frustum.nearPlane);
		EXPECT_FLOAT_EQ(4.f, Frustum.farPlane);
		EXPECT_FLOAT_EQ(-3.f, Frustum.left);
		EXPECT_FLOAT_EQ(3.f, Frustum.right);
		EXPECT_FLOAT_EQ(1.f, Frustum.top);
		EXPECT_FLOAT_EQ(-1.f, Frustum.bottom);
	}

	TEST(BoundingBoxUtilTests, DirectionalLightFrustumSkewed)
	{
		AABBox box;
		box.d = glm::vec3(2, 2, 2);
		glm::vec3 ldir(-1, -1, -1);

		OrthogonalFrustum Frustum;
		EXPECT_TRUE(BoundingBoxUtil::DirectionalLightFrustum(box, ldir, Frustum));
		EXPECT_FLOAT_EQ(0.f, Frustum.nearPlane);
		EXPECT_FLOAT_EQ(glm::length(box.d) * 2.f, Frustum.farPlane);
		EXPECT_LT(-Frustum.right + Frustum.left, 1e-5f);
		EXPECT_LT(-Frustum.top + Frustum.bottom, 1e-5f);
	}

	TEST(BoundingBoxUtilTests, BasisFromDirection)
	{
		glm::vec3 dir(1.4, 0, 0);
		glm::mat3 basis = BoundingBoxUtil::BasisFromDirection(dir);
		EXPECT_EQ(glm::normalize(dir), basis[0]);
		EXPECT_EQ(glm::vec3(0, 1, 0), basis[1]);
		EXPECT_EQ(glm::vec3(0, 0, 1), basis[2]);
		EXPECT_FLOAT_EQ(1.f, glm::length(basis[0])) << "Basis vector not normalized";
		EXPECT_FLOAT_EQ(1.f, glm::length(basis[1])) << "Basis vector not normalized";
		EXPECT_FLOAT_EQ(1.f, glm::length(basis[2])) << "Basis vector not normalized";

		dir = glm::vec3(2, 1, 2);
		basis = BoundingBoxUtil::BasisFromDirection(dir);
		EXPECT_EQ(glm::normalize(dir), basis[0]);
		EXPECT_EQ(glm::vec3(-0.235702261, 0.942808986, -0.235702261), basis[1]);

		const float eps = 1e-7f;
		EXPECT_LT(std::abs(glm::dot(basis[0], basis[1])), eps) << "Basis vectors not normal";
		EXPECT_LT(std::abs(glm::dot(basis[1], basis[2])), eps) << "Basis vectors not normal";
		EXPECT_LT(std::abs(glm::dot(basis[0], basis[2])), eps) << "Basis vectors not normal";
		EXPECT_LT(std::abs(1 - glm::length(basis[0])), eps) << "Basis vector not normalized";
		EXPECT_LT(std::abs(1 - glm::length(basis[1])), eps) << "Basis vector not normalized";
		EXPECT_LT(std::abs(1 - glm::length(basis[2])), eps) << "Basis vector not normalized";
	}

	// Frustum Tests
	TEST(FrustumTest, OrthoFrustumCorners)
	{
		OrthogonalFrustum frust;
		frust.left = 1;
		frust.right = 7;
		frust.top = 5;
		frust.bottom = 2;
		frust.nearPlane = 3;
		frust.farPlane = 6;

		glm::vec3 p = glm::vec3(1, 2, 3);
		frust.position = p;

		//90 deg around y axis
		auto rmat = glm::mat3(glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0)));
		frust.frame *= rmat;

		float eps = 1e-6f;

		auto corners = frust.CornerPoints();
		EXPECT_LT(glm::length(corners[0] - glm::vec3(-frust.nearPlane, frust.top, -frust.right)), eps);
		EXPECT_LT(glm::length(corners[1] - glm::vec3(-frust.nearPlane, frust.top, -frust.left)), eps);
		EXPECT_LT(glm::length(corners[2] - glm::vec3(-frust.nearPlane, frust.bottom, -frust.right)), eps);
		EXPECT_LT(glm::length(corners[3] - glm::vec3(-frust.nearPlane, frust.bottom, -frust.left)), eps);
		EXPECT_LT(glm::length(corners[4] - glm::vec3(-frust.farPlane, frust.top, -frust.right)), eps);
		EXPECT_LT(glm::length(corners[5] - glm::vec3(-frust.farPlane, frust.top, -frust.left)), eps);
		EXPECT_LT(glm::length(corners[6] - glm::vec3(-frust.farPlane, frust.bottom, -frust.right)), eps);
		EXPECT_LT(glm::length(corners[7] - glm::vec3(-frust.farPlane, frust.bottom, -frust.left)), eps);
	}

	TEST(FrustumTest, PerspectiveFrustumCorners)
	{
		PerspectiveFrustum frust;
		frust.nearPlane = 3;
		frust.farPlane = 6;
		frust.fovY = 60;
		frust.aspectRatio = 0.5f;

		glm::vec3 p = glm::vec3(1, 2, 3);
		frust.position = p;

		float gk_y = sqrtf(3.f);
		float gk_x = 3.f * (2.f - sqrtf(3.f));

		//90 deg around y axis
		auto rmat = glm::mat3(glm::rotate(glm::mat4(1), glm::half_pi<float>(), glm::vec3(0, 1, 0)));
		frust.frame *= rmat;

		float eps = 1e-6f;

		auto corners = frust.CornerPoints();
		EXPECT_LT(glm::length(corners[0] - glm::vec3(-frust.nearPlane, gk_y, -gk_x)), eps);
		EXPECT_LT(glm::length(corners[1] - glm::vec3(-frust.nearPlane, gk_y, gk_x)), eps);
		EXPECT_LT(glm::length(corners[2] - glm::vec3(-frust.nearPlane, -gk_y, -gk_x)), eps);
		EXPECT_LT(glm::length(corners[3] - glm::vec3(-frust.nearPlane, -gk_y, gk_x)), eps);
		EXPECT_LT(glm::length(corners[4] - glm::vec3(-frust.farPlane, 2.f * gk_y, 2.f * -gk_x)), eps);
		EXPECT_LT(glm::length(corners[5] - glm::vec3(-frust.farPlane, 2.f * gk_y, 2.f * gk_x)), eps);
		EXPECT_LT(glm::length(corners[6] - glm::vec3(-frust.farPlane, 2.f * -gk_y, 2.f * -gk_x)), eps);
		EXPECT_LT(glm::length(corners[7] - glm::vec3(-frust.farPlane, 2.f * -gk_y, 2.f * gk_x)), eps);
	}
}
