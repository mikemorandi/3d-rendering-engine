#include "stdafx.h"

#include "ObjLoader.h"
#include "RawMesh.h"
#include "Util.h"

#include "../error.h"

#include <fstream>
#include <filesystem>
#include <regex>
#include <sstream>
#include <array>
#include <deque>
#include <glm/glm.hpp>

using glm::vec2;
using glm::vec3;
using glm::ivec3;

namespace fs = std::filesystem;

IndexedRawMesh_ptr ObjLoader::LoadObj(std::istream& istr, const std::filesystem::path& path)
{	
	int lineCount = 0;

	IndexedRawMesh_ptr newMesh = IndexedRawMesh::Create();
	newMesh->meshPath = path.string();

	std::string groupName;	
	std::string currentMtl;
	int groupFaceStartIdx = 0;

	auto create_group = [&](const std::string& groupName)
	{
		if (newMesh->faces.size() > 0)
		{
			int groupEndIndex = static_cast<int>(newMesh->faces.size() - 1);

			if (groupEndIndex - groupFaceStartIdx > 0 )
			{
				IndexedRawMesh::Range range(groupFaceStartIdx, groupEndIndex);

				newMesh->AddGroup(groupName, currentMtl, range);

				//next group
				groupFaceStartIdx = groupEndIndex + 1;
			}
		}
	};

	unsigned int usemtl_counter = 0;
	unsigned int tot_mtls = 0;
	std::string current_group = "DefaultGroup";

	std::string line;
	while ( istr.good() )
	{
		std::getline(istr,line);
		auto num_chars = line.size();
		lineCount++;

		if (line[0] == 'o' && num_chars > 2)
		{
			if (newMesh->faces.size() > 0)
			{
				create_group(newMesh->name);
			}

			newMesh->name = std::string(line.begin() + 2, line.end());
			usemtl_counter = 0;
		}
		if (line[0] == 'g' && num_chars > 2)
		{
			if (newMesh->faces.size() > 0)
			{				
				create_group((usemtl_counter > 1) ? (current_group + "-" + currentMtl + std::to_string(tot_mtls)) : current_group);
			}

			current_group = std::string(line.begin() + 2, line.end());
			usemtl_counter = 0;
		}
		else if (line[0] == 'v' && line[1] == ' ')
		{
			glm::vec3 v;
			char* end_p1, *end_p2;
			v.x = std::strtof(&line[2], &end_p1);
			v.y = std::strtof(end_p1, &end_p2);
			v.z = std::strtof(end_p2, nullptr);
			newMesh->vertices.push_back(v);
		}
		else if (line[0] == 'v' && line[1] == 'n')
		{
			glm::vec3 v;
			char* end_p1, *end_p2;
			v.x = std::strtof(&line[3], &end_p1);
			v.y = std::strtof(end_p1, &end_p2);
			v.z = std::strtof(end_p2, nullptr);
			newMesh->normals.push_back(v);
		}
		else if (line[0] == 'v' && line[1] == 't')
		{
			glm::vec2 v;
			char* end_p1;
			v.x = std::strtof(&line[3], &end_p1);
			v.y = std::strtof(end_p1, nullptr);
			newMesh->texCoords.push_back(v);
		}
		else if (line[0] == 'f' && line[1] == ' ')
		{
			std::istringstream str_stream(line);
			str_stream.ignore(2);

			std::deque<glm::ivec3> vtx_queue;

			bool first_vtx_read = false;
			glm::ivec3 first_vtx;
			glm::ivec3 current_vtx(-1,-1,-1);
			int comp = 0;

			while (!str_stream.eof())
			{
				str_stream >> std::ws >> current_vtx[comp];
				current_vtx[comp]--;

				if (str_stream.peek() == '/')
				{
					str_stream.ignore();
					int skip_comp = 1;

					if (str_stream.peek() == '/')
					{
						str_stream.ignore();
						skip_comp++;
					}

					comp += skip_comp;
				}
				else
				{
					str_stream >> std::ws;

					comp = 0;

					if (!first_vtx_read)
					{
						first_vtx = current_vtx;
						first_vtx_read = true;
					}
					else					
						vtx_queue.push_back(current_vtx);											

					current_vtx = glm::ivec3(-1, -1, -1);

					if (vtx_queue.size() == 2)
					{
						Tri triangle;
						triangle.Set(0, first_vtx);
						triangle.Set(1, vtx_queue[0]);
						triangle.Set(2, vtx_queue[1]);
						vtx_queue.pop_front();

						newMesh->faces.push_back(triangle);
					}
				}
			}
		}
		else if(line.substr(0,7) == "mtllib ")
		{			
			fs::path mtlFile = Util::ExtractBaseFolder(currentFile) / fs::path(line.substr(7));

			std::ifstream mtlFileStream(mtlFile);
			if(mtlFileStream && LoadMtllib(mtlFileStream,newMesh))
				std::cout << "Material lib loaded: " << mtlFile << std::endl;
			else
				std::cout << "Could not load mtllib: " << mtlFile << std::endl;
		}
		else if(line.substr(0,7) == "usemtl ")
		{	
			if (usemtl_counter <= 1)
				create_group(current_group);
			else if (usemtl_counter > 1)
				create_group(current_group + "-" + currentMtl + std::to_string(tot_mtls));
	
			currentMtl = line.substr(7);
			usemtl_counter++;
			tot_mtls++;
		}
	}

	//Complete last index group
	create_group(groupName);

	if (newMesh->name.empty())
		newMesh->name = Util::ExtractFileName(newMesh->meshPath).string();

	std::cout << "Wavefront OBJ mesh loaded, f=" 
		<< newMesh->faces.size() 
		<< ", v=" << newMesh->vertices.size() 
		<< ", n=" << newMesh->normals.size() 
		<< ", tex coords=" 
		<< newMesh->texCoords.size() 
		<< std::endl;

	return newMesh;
}

std::string& trim(std::string& str)
{
	str.erase(str.begin(), find_if(str.begin(), str.end(),
		[](char& ch)->bool { return !isspace(ch); }));
	str.erase(find_if(str.rbegin(), str.rend(),
		[](char& ch)->bool { return !isspace(ch); }).base(), str.end());
	return str;
}

bool ObjLoader::LoadMtllib(std::istream& istr, IndexedRawMesh_ptr newMesh)
{
	std::string line;

	WavefrontObjMaterial_ptr mat;
	
	while ( istr.good() )
	{
		getline (istr,line);
		trim(line);

		auto num_chars = line.size();

		if(line.substr(0,6) == "newmtl")
		{
			if(mat)			
				newMesh->materials.push_back(mat);
			
			mat = WavefrontObjMaterial::Create(line.substr(7));
		}
		else if(mat && num_chars > 0)
		{
			if(num_chars > 3 && line.substr(0,2) == "Ka")
			{
				std::istringstream s(line.substr(2));
				s >> mat->ambient.r; s >>  mat->ambient.g; s >>  mat->ambient.b;
			}
			else if(num_chars > 3 && line.substr(0,2) == "Kd")
			{
				std::istringstream s(line.substr(2));
				s >> mat->diffuse.r; s >>  mat->diffuse.g; s >>  mat->diffuse.b;
			}
			else if(num_chars > 3 && line.substr(0,2) == "Ks")
			{
				std::istringstream s(line.substr(2));
				s >> mat->specular.r; s >>  mat->specular.g; s >>  mat->specular.b;
			}
			else if(num_chars > 2 && line.substr(0,1) == "d")
			{
				std::istringstream s(line.substr(1));
				s >> mat->opacity;
			}
			else if(num_chars > 3 && line.substr(0,2) == "Ns")
			{
				std::istringstream s(line.substr(2));
				s >> mat->shininess;
			}
			else if(num_chars > 6 && line.substr(0,5) == "illum")
			{
				std::istringstream s(line.substr(5));
				int illuminationModel;
				s >> illuminationModel;
				mat->specularEnabled = illuminationModel > 1;
			}
			else if(num_chars > 7 && line.substr(0,6) == "map_Ka")
			{
				mat->ambientColorTexture = line.substr(7);
			}
			else if(num_chars > 7 && line.substr(0,6) == "map_Kd")
			{
				mat->diffuseColorTexture = line.substr(7);
			}
			else if(num_chars > 7 && line.substr(0,6) == "map_Ks")
			{
				mat->specularColorTexture = line.substr(7);
			}
			else if(num_chars > 7 && line.substr(0,6) == "map_Ns")
			{
				mat->specularHightlightTexture = line.substr(7);
			}
			else if(num_chars > 6 && line.substr(0,5) == "map_d")
			{
				mat->alphaMapTexture = line.substr(6);
			}
			else if(num_chars > 9 && line.substr(0,8) == "map_bump")
			{
				mat->bumpMapTexture = line.substr(7);
			}
			else if(num_chars > 5 && line.substr(0,4) == "bump")
			{
				mat->bumpMapTexture = line.substr(5);
			}
			else if(num_chars > 5 && line.substr(0,4) == "disp")
			{
				mat->displacementMapTexture = line.substr(5);
			}
		}
	}	
	newMesh->materials.push_back(mat);
	return newMesh->materials.size() > 0;
}

IndexedRawMesh_ptr ObjLoader::LoadObjFile(const std::filesystem::path& path)
{
	currentFile = path.string();

	std::ifstream myfile;
	myfile.open(path);

	IndexedRawMesh_ptr newMesh;

	std::string line;
	if (myfile.is_open())
	{
		newMesh = LoadObj(myfile, path);
		myfile.close();
	}
	else 
		std::cerr << "Unable to open OBJ file: "  << path.string() << std::endl;

	return newMesh;
}