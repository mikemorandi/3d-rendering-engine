#include "Framebuffer.h"
#include "Texture2D.h"

#include <cassert>

Framebuffer::Framebuffer()
{
	glGenFramebuffers(1, &bufferHandle);
	memset(&renderBuferHandle, 0, 3 * sizeof(GLuint) );
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &bufferHandle);
}

bool Framebuffer::IsComplete() const
{
	return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

bool Framebuffer::IsBound() const
{
	GLint boundFbo = 0;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFbo);
	return static_cast<GLuint>(boundFbo) == bufferHandle;
}

bool Framebuffer::Attach(const Texture2DPtr& texture, Attachment target)
{
	
	if (texture && texture->IsValid())
	{
		bool bound = IsBound();		
		if (!bound)
			Bind();

		

		auto get_gl_enum = [](Attachment target)->GLenum
		{
			switch (target)
			{
			case Attachment::Color:
				return GL_COLOR_ATTACHMENT0;
				break;
			case Attachment::Depth:
				return GL_DEPTH_ATTACHMENT;
				break;
			case Attachment::Stencil:
				return GL_STENCIL_ATTACHMENT;
				break;
			}

			return GL_COLOR_ATTACHMENT0;
		};

		attachedTexture[TextureIndex(target)] = texture;
		glFramebufferTexture2D(GL_FRAMEBUFFER, get_gl_enum(target), GL_TEXTURE_2D, texture->Handle(), 0);

		if(!bound)
			Unbind();
		return true;
	}
	return false;	
}

bool Framebuffer::AttachDepthRenderBuffer()
{
	GLuint bufHandle = renderBuferHandle[static_cast<GLuint>(Attachment::Depth)];

	if (!glIsRenderbuffer(bufHandle))
	{
		if ((bufHandle = CreateRenderBuffer(GL_DEPTH_COMPONENT)))
		{
			bool isRbuf = glIsRenderbuffer(bufHandle) == GL_TRUE;
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, bufHandle);
			return isRbuf;
		}
	}

	return false;
}

GLuint Framebuffer::CreateRenderBuffer(GLenum format)
{
	Texture2DPtr colorTex = TextureAttachment(Attachment::Color);

	if (colorTex && colorTex->IsValid())
	{
		GLuint handle;

		const glm::ivec2& colorTexDim = colorTex->Dimensions();

		assert(colorTexDim.x > 0);
		assert(colorTexDim.y > 0);

		glGenRenderbuffers(1, &handle);
		glBindRenderbuffer(GL_RENDERBUFFER, handle);
		glRenderbufferStorage(GL_RENDERBUFFER, format, colorTexDim.x, colorTexDim.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		return handle;
	}

	return 0;
}

void Framebuffer::SetDrawToColorBufferEnabled(bool enabled)
{
	if (enabled)
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
	else
		glDrawBuffer(GL_NONE);
}

bool Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferHandle);
	return IsComplete();
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint Framebuffer::TextureIndex(Attachment target) const
{
	return static_cast<GLuint>(target);
}

GLuint Framebuffer::Renderbuffer(Attachment target) const
{
	return renderBuferHandle[TextureIndex(target)];
}

Texture2DPtr Framebuffer::TextureAttachment(Attachment target) const
{
	return attachedTexture[TextureIndex(target)];
}