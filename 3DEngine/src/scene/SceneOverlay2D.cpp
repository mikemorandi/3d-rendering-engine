#include "SceneOverlay2D.h"

#include "../materials/Material.h"
#include  "../util/Util.h"
#include  "../util/TimeManager.h"
#include "../camera/OrthogonalCamera.h"
#include  "../shape/RenderMesh.h"
#include  "../light/SpotLight.h"
#include  "../light/DirectionalLight.h"
#include  "../texture/DepthTexture.h"
#include  "../texture/Texture2D.h"

#include <memory>
#include <glm/gtc/matrix_transform.hpp>

SceneOverlay2D::SceneOverlay2D(ScenePtr scene)
: Scene(std::shared_ptr<OrthogonalCamera>(new OrthogonalCamera()),false)
{
	renderBoundingBoxes = false;
	renderLightRepresentation = false;

	if (scene)
	{
		lightModel->directionalLight = DirectionalLight::Create(glm::vec3(0, 0, -1));
	}
}


SceneOverlay2D::~SceneOverlay2D()
{
}

SceneOverlay2DPtr SceneOverlay2D::Create(ScenePtr scene)
{
	return std::make_shared<SceneOverlay2D>(scene);
}

void SceneOverlay2D::TimeUpdate(double time)
{
	(void)time;  // Overlay doesn't animate
}
