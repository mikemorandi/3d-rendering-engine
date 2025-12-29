#include "stdafx.h"
#include "WireCube.h"

#include <glm/gtc/matrix_transform.hpp>

void WireCube::Init() {

	RenderMesh::Init();

	name = "WireCube";

	std::vector<glm::vec3> vertex_data;
	std::vector<int> index_data;
	
	float d = 1.0f;

	vertex_data.resize(8);
	index_data.resize(24);

	vertex_data[0] = glm::vec3(-d, -d, -d);
	vertex_data[1] = glm::vec3(d, -d, -d);
	vertex_data[2] = glm::vec3(d, d, -d);
	vertex_data[3] = glm::vec3(-d, d, -d);
	vertex_data[4] = glm::vec3(-d, -d, d);
	vertex_data[5] = glm::vec3(d, -d, d);
	vertex_data[6] = glm::vec3(d, d, d);
	vertex_data[7] = glm::vec3(-d, d, d);

	index_data[0] = 0; index_data[1] = 1;
	index_data[1] = 1; index_data[2] = 2;

	for (int i = 0; i < 4; i++)
	{
		index_data[2*i] = i; 
		index_data[2*i+1] = (i+1)%4;
		index_data[8+2*i] = 4+i;
		index_data[8+2*i + 1] = 4 + index_data[2*i + 1];
		index_data[16+2*i] = i;
		index_data[16+2*i+1] = 4 + i;
	}
	SetPositions(vertex_data, index_data);

	bboxModelSpace = AABBox(glm::vec3(-d, -d, -d), glm::vec3(d, d, d));
}

WireCube::WireCube() 
: RenderMesh(DrawMode::Lines)
{

}