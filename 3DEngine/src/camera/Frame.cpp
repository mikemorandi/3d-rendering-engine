
#include "Frame.h"

#include <glm/glm.hpp>

glm::vec3& CoordinateFrame::ViewDir() { return operator[](ViewAxis); }

glm::vec3& CoordinateFrame::Up() { return operator[](UpAxis); }

glm::vec3& CoordinateFrame::Side() { return operator[](SideAxis); }

const glm::vec3& CoordinateFrame::ViewDir() const { return operator[](ViewAxis); }

const glm::vec3& CoordinateFrame::Up() const { return operator[](UpAxis); }

const glm::vec3& CoordinateFrame::Side() const { return operator[](SideAxis); }

/// Creates an OpenGL  coordinate system (view direction is along negative Z)
CoordinateFrame::CoordinateFrame()
{
	//OpenGL convention
	ViewDir() = glm::vec3(0, 0, -1);
	Up() = glm::vec3(0, 1, 0);
	Side() = glm::vec3(1, 0, 0);
}

CoordinateFrame::CoordinateFrame(const glm::vec3& view, const glm::vec3& side, const glm::vec3& up)
	: glm::mat3(view, up, side) {}
