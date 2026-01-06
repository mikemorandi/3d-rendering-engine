

#include "HaltonSphere.h"

#include <ctime>
#include <random>

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

void HaltonSphere::GenerateRays(std::vector<glm::vec3> &points, int p2/*=7*/)
{		
	float p, t, st, phi, phirad, ip;
	int k, kk, a;
	for (k=0 ; k < static_cast<int>(points.size()) ; k++)
	{
		t = 0;
		for (p=0.5, kk=k ; kk ; p*=0.5f, kk>>=1)
			if (kk & 1)								// kk mod 2 == 1
				t += p;
		t = 2.0f * t - 1.0f;						// map from [0,1] to [-1,1]
		st = sqrt(1.0f-t*t);
		phi = 0;
		ip = 1.0f/p2;								// inverse of p2
		for (p=ip, kk=k ; kk ; p*=ip, kk/=p2)		// kk = (int)(kk/p2)
			if ((a = kk % p2))
				phi += a * p;
		phirad = (float)(phi * 4.0 * glm::pi<float>());	// map from [0,0.5] to [0, 2 pi)

		points[k].x = st * cos(phirad);
		points[k].y = st * sin(phirad);
		points[k].z = t;
	}
}

void ShuffleRays(std::vector<glm::vec3>& rays)
{
	std::mt19937 rng;
	rng.seed(clock());
	std::uniform_real_distribution<float> distribution(0, 2*glm::pi<float>());  
		
	glm::mat4 m = glm::eulerAngleYXZ(distribution(rng), distribution(rng), distribution(rng));
	glm::mat3 rotM(m);
	
	for(size_t i=0; i < rays.size(); i++)
	{
		rays[i] = rotM * rays[i];
	}
}