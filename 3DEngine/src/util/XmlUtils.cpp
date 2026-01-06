#include "XmlUtils.h"

#include "../error.h"

#include <iostream>
#include <glm/vec3.hpp>

bool XmlUtils::GetVector3(tinyxml2::XMLElement* element, glm::vec3& vec)
{
	if(element != nullptr)
	{
		bool success = true;

		success &= GetFloatAttrib(element,"r",vec.x);
		success &= GetFloatAttrib(element,"g",vec.y);
		success &= GetFloatAttrib(element,"b",vec.z);

		return success;
	}
	else
		return false;
	
}

bool XmlUtils::GetColorVector3(tinyxml2::XMLElement* element, glm::vec3& vec)
{
	if(element == nullptr)
		return false;
	
	bool success = true;

	success &= GetFloatAttrib(element,"r",vec.x);
	success &= GetFloatAttrib(element,"g",vec.y);
	success &= GetFloatAttrib(element,"b",vec.z);

	return success;
}

bool XmlUtils::GetFloatAttrib(tinyxml2::XMLElement* element, const char* attribName, float& value)
{
	if(element == nullptr)
		return false;
	
	std::istringstream isstr(element->Attribute(attribName));
	return !(isstr >> value).fail();
}

bool XmlUtils::GetIntAttrib(tinyxml2::XMLElement* element, const char* attribName, int& value)
{
	if(element == nullptr)
		return false;
	
	std::istringstream isstr(element->Attribute(attribName));
	return !(isstr >> value).fail();
}