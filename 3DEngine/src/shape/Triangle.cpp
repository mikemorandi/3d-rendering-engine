#include "stdafx.h"
#include "Triangle.h"
#include <glm/gtc/matrix_transform.hpp>

Triangle::Triangle() : RenderMesh()
{

}

void Triangle::Init()  {

	RenderMesh::Init();

	float radius = 0.5f;

	float ninety_deg = glm::radians(90.f);
	float twoten_deg = glm::radians(210.f);
	float threethree_deg = glm::radians(330.f);

	std::vector<glm::vec3> positionData = {
		glm::vec3(cos(ninety_deg), sin(ninety_deg), 0.f) * radius,
		glm::vec3(cos(twoten_deg), sin(twoten_deg), 0.f) * radius,
		glm::vec3(cos(threethree_deg), sin(threethree_deg), 0.f) * radius };

	std::vector<glm::vec3> normalData = {
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f) };

    float colorData[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f };
		
	int indices[] = { 0, 1, 2};

	std::vector<float> col(colorData, colorData+9);
	std::vector<int> idx(indices, indices+3);

	
	SetPositions(positionData, idx);
	SetNormals(normalData);
	SetColors(col);

	bboxModelSpace = AABBox(positionData[0], -positionData[0]);
}
