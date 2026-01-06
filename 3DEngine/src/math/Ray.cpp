
#include "Ray.h"


Ray::Ray(const glm::vec3& o, const glm::vec3& d)
{
	origin = o;
	direction = d;
	inv_direction = glm::vec3(1.f / d.x, 1.f / d.y, 1.f / d.z);
	sign[0] = (inv_direction.x < 0);
	sign[1] = (inv_direction.y < 0);
	sign[2] = (inv_direction.z < 0);
}