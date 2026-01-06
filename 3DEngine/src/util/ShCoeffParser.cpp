#include "ShCoeffParser.h"

#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "../shader/SHDiffuseShader.h"
#include "../util/XmlUtils.h"
#include "../error.h"


using tinyxml2::XMLDocument;
using tinyxml2::XMLElement;
using tinyxml2::XMLNode;
using std::string;
using std::vector;

ShDiffuseShaderCoeffs_ptr ShCoeffParser::Parse(const std::string& coeffXmlDocument)
{
	ShDiffuseShaderCoeffs_ptr shCoeffs;
	
	tinyxml2::XMLDocument doc;
	if(doc.Parse(coeffXmlDocument.c_str()) == tinyxml2::XML_SUCCESS)
	{	
		XMLElement* root = doc.RootElement();

		if( strcmp( root->Name(), "shcoeffs" ) == 0 )
		{
			shCoeffs = ShDiffuseShaderCoeffs::Create();

			shCoeffs->name = std::string(root->Attribute("name"));
			std::string skymap = std::string(root->Attribute("skymap"));

			if(const char* exp = root->Attribute("exposure"))
			{
				std::string exposure = std::string(exp);
			}

			XMLElement* vecElement = root->FirstChildElement("vec3");;
			do
			{
				glm::vec3 v;
				XmlUtils::GetColorVector3(vecElement,v);
				shCoeffs->m_Coeffs.push_back(v);
			}				
			while ((vecElement = vecElement->NextSiblingElement("vec3")));

			

		}
		else 
		{
			Error("Not a valid coeff file");
		}
	}
	else
	{
		Error("File could not be parsed" );
	}

	return shCoeffs;
}
