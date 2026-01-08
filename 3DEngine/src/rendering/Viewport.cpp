#include "Viewport.h"
#include "../core/gl.h"


ViewportPtr Viewport::Create(int width, int height)
{
	return std::make_shared<Viewport>(width, height);
}

Viewport::Viewport(int width, int height)
: width(width)
, height(height)
{

}

void Viewport::Apply()
{
	glViewport(0, 0, width, height);
}


Viewport::~Viewport()
{
}
