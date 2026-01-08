#pragma once

#include "Texture2D.h"
#include <memory>

class DepthTexture;
using DepthTexturePtr = std::shared_ptr<DepthTexture>;
using DepthTextureConstPtr = std::shared_ptr<const DepthTexture>;
using DepthTextureWeakPtr = std::weak_ptr<DepthTexture>;

class DepthTexture : public Texture2D
{
public:

	static DepthTexturePtr Create(int width, int height);

	DepthTexture(int width, int height, Format format);

	enum class CompareMode : unsigned int { RefToTexture =  static_cast<unsigned int>(GL_COMPARE_REF_TO_TEXTURE), Value = static_cast<unsigned int>(GL_NONE)};

	/// Determines how the texture will be evaluated. Compare always returns 0 or 1 depending on the result of the depth test. Value returns the actual value
	void SetCompareMode(const CompareMode& mode);

	const CompareMode& TexCompareMode() const;

protected:

	CompareMode compareMode;
};

