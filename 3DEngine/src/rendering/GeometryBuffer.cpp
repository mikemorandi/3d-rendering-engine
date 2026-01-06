#include "GeometryBuffer.h"
#include "../shader/GBufferShader.h"

GeometryBuffer_ptr GeometryBuffer::Create(int width, int height)
{
	return std::make_shared<GeometryBuffer>(width,height);
}

GeometryBuffer::GeometryBuffer(int width, int height)
{
	glGenFramebuffers(1,&m_DeferredFboHandle);
	glGenRenderbuffers(1,&m_DepthBuffer);
	glGenTextures(2,m_Textures);

	glBindFramebuffer(GL_FRAMEBUFFER, m_DeferredFboHandle);

	//Depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER,m_DepthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT,width, height);

	//Position buffer
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,m_Textures[Position]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Normal buffer
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,m_Textures[Normal]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Attach to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, m_DepthBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Textures[Position],0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_Textures[Normal],0);

	GLenum drawBuffers[4] = {GL_NONE,GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
	glDrawBuffers(4, drawBuffers);

	m_Shader = GBufferShader::Create();

	glBindRenderbuffer(GL_RENDERBUFFER,0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GeometryBuffer::~GeometryBuffer()
{
	glDeleteTextures(3,m_Textures);
	glDeleteRenderbuffers(1,&m_DepthBuffer);
	glDeleteFramebuffers(1,&m_DeferredFboHandle);
}

 void GeometryBuffer::StartGBufferComp([[maybe_unused]] const Scene_ptr& scene)
 {
	 // TODO: Use scene for GBuffer composition
	 glBindFramebuffer(GL_FRAMEBUFFER, m_DeferredFboHandle);
	 //glm::mat4 m;
	 //m_Shader->Use(scene, m);
 }

 void GeometryBuffer::EndGBufferComp()
 {
	 m_Shader->UnUse();
	 glBindFramebuffer(GL_FRAMEBUFFER, 0);
 }
