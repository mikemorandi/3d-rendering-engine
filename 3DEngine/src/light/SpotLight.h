#pragma once

#include "PointLight.h"

#include "../camera/Frame.h"
#include <memory>

class SpotLight;
using SpotLightPtr = std::shared_ptr<SpotLight>;
using SpotLightConstPtr = std::shared_ptr<const SpotLight>;
using SpotLightWeakPtr = std::weak_ptr<SpotLight>;

/**
*      pos
*       .
*      /|\		t = theta, the cutoff angle
*     / | \		is an angle that specifies the angular extent
*    /  |  \	measured from the centerline (direction)
*   / t | t \
*       v
*        dir
*
*/
class SpotLight : public PointLight, public std::enable_shared_from_this<SpotLight>
{
public:

	static SpotLightPtr Create(const glm::vec3& direction, float cutoffAngle, float exponent);

	SpotLight(const glm::vec3& direction, float cutoffAngle, float exponent);

	virtual void SetDirection(const glm::vec3& dir);

	virtual void SetPosition(const glm::vec4& pos) override;

	const CoordinateFrame& Frame() const { return frame; };

	virtual float CutoffAngle() const;

	/// Sets the cutoff angle theta
	virtual void SetCutoffAngle(float theta);
	virtual void SetExponent(float exponent);
	virtual float Exponent() const;

	// Shadow mapping overrides
	virtual glm::mat4 GetLightViewMatrix() const override;
	virtual glm::mat4 GetLightProjectionMatrix() const override;
	void UpdateLightMatrices();

protected:

	void UpdateVisMesh();

	CoordinateFrame frame;

	float cutoffAngle;
	float exponent;

};

