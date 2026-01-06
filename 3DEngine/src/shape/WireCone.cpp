#include "WireCone.h"
#include <glm/gtc/matrix_transform.hpp>


WireCone_ptr WireCone::Create(float opening_angle, float height)
{
	return std::make_shared<WireCone>(opening_angle, height);
}

void WireCone::Init() {

	RenderMesh::Init();

	std::vector<glm::vec3> vertex_data;
	std::vector<int> index_data;
	
	const int circle_segments = 12;
	float angle_increment = glm::two_pi<float>() / circle_segments;
	float radius = std::tanf(glm::radians(openingAngle)) * height;

	vertex_data.resize(circle_segments+1);
	index_data.resize(circle_segments*4);

	for (int i = 0; i < circle_segments; i++)
	{
		vertex_data[i].x = cosf(i*angle_increment) * radius;
		vertex_data[i].y = sinf(i*angle_increment) * radius;
		vertex_data[i].z = height;

		index_data[2 * i] = i;
		index_data[2 * i + 1] = (i + 1) % circle_segments;

		index_data[2* circle_segments + 2 * i] = circle_segments;
		index_data[2* circle_segments + 2 * i + 1] = i;
	}

	vertex_data[circle_segments] = glm::vec3(0, 0, 0);

	SetPositions(vertex_data, index_data);

	bboxModelSpace = AABBox(glm::vec3(-radius, -radius, 0), glm::vec3(radius, radius, height));
}

WireCone::WireCone(float opening_angle,float height) 
: RenderMesh(DrawMode::Lines)
, openingAngle(opening_angle)
, height(height)
{

}