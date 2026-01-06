#include "GBufferShader.h"

GBufferShader::GBufferShader()
	: ShaderBase("gBufferShader")
{
	hasMVM = true;
	hasNM = true;
	hasPM = true;
	hasMVP = true;
}


GBufferShader::~GBufferShader()
{
}
