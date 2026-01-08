#pragma once

#include "../util/SharedPointer.h"
#include <memory>
#include "../core/gl.h"

class Framebuffer;
using FramebufferPtr = std::shared_ptr<Framebuffer>;
using FramebufferConstPtr = std::shared_ptr<const Framebuffer>;
using FramebufferWeakPtr = std::weak_ptr<Framebuffer>;
class Texture2D;
using Texture2DPtr = std::shared_ptr<Texture2D>;
using Texture2DConstPtr = std::shared_ptr<const Texture2D>;
using Texture2DWeakPtr = std::weak_ptr<Texture2D>;

/** 
* \brief a class that wraps an OpenGL Framebuffer Object (FBO)
*
* The member functions only work correctly if the Framebuffer
* is bound
*/
class Framebuffer
{
public:
	SHARED_PTR_FACTORY(Framebuffer);

	Framebuffer();

	virtual ~Framebuffer();

	enum class Attachment : unsigned int { Color = 0, Depth = 1, Stencil = 2 };

	/// Attaches the given texture to the specified buffer target, returns true on success
	bool Attach(const Texture2DPtr& texture, Attachment target);

	/** Attaches a depth renderbuffer to the Framebuffer. Only applicabe
	 *  if a color attachement has been set, returns true on success */
	bool AttachDepthRenderBuffer();

	/// Enables/disables drawing to the color attachement of this buffer
	void SetDrawToColorBufferEnabled(bool enabled);

	/// Returns the attachement of a potentially bound renderbuffer, 0 if none is available
	inline GLuint Renderbuffer(Attachment target) const;

	/// Returns the attachement of a potentially bound texture, invalid ptr if none is available
	inline Texture2DPtr TextureAttachment(Attachment target) const;

	/// Returns true if framebuffer ist complete. 
	bool IsComplete() const;

	/// Binds the current framebuffer
	bool Bind();

	/// Uninds the current framebuffer
	void Unbind();

protected:

	GLuint CreateRenderBuffer(GLenum format);

	inline bool IsBound() const;

	inline GLuint TextureIndex(Attachment target) const;

	GLuint bufferHandle;

	GLuint renderBuferHandle[3];
	Texture2DPtr attachedTexture[3];

	//Texture2DPtr	colorTexture;
	//Texture2DPtr	depthTexture;
	//Texture2DPtr	stencilTexture;

	bool drawToColorBuffer;
	
};

