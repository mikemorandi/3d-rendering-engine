#pragma once

#include <tinyxml2.h>
#include <memory>
#include <string>
#include "../shader/SHDiffuseShader.h"

using tinyxml2::XMLElement;

class ShCoeffParser;
using ShCoeffParserPtr = std::shared_ptr<ShCoeffParser>;
using ShCoeffParserConstPtr = std::shared_ptr<const ShCoeffParser>;
using ShCoeffParserWeakPtr = std::weak_ptr<ShCoeffParser>;

class ShCoeffParser
{
public:

	static ShDiffuseShaderCoeffsPtr Parse(const std::string& coeffXmlDocument);


};

