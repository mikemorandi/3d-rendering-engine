#include "UniformBufferShader.h"


UniformBufferShader::UniformBufferShader()
	: ShaderBase("uniformBuffer")
{

}


VertexAttributeInfo_ptr UniformBufferShader::GetVertexAttributeInfo() const
{
	VertexAttributeInfo_ptr vtxInfo = VertexAttributeInfo::Create();
	vtxInfo->mapping[GLSLShader::Position] = 0;
	vtxInfo->mapping[GLSLShader::Normal] = 1;
	vtxInfo->mapping[GLSLShader::Tangent] = 2;
	vtxInfo->mapping[GLSLShader::TextureCoord] = 3;

	return vtxInfo;
}