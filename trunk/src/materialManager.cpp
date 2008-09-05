/*
    Copyright (C) 2008 Rômulo Fernandes Machado <romulo@castorgroup.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "materialManager.h"
#include "textureManager.h"
#include "logger.h"

namespace k {

template<> materialManager* singleton<materialManager>::singleton_instance = 0;

materialManager& materialManager::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}

materialManager::materialManager()
{
	mMaterials.clear();
}

materialManager::~materialManager()
{
	//TODO: Free all allocated materials
}
			
material* materialManager::createMaterial(const std::string& name)
{
	material* newMaterial = getMaterial(name);
	if (newMaterial)
	{
		// Material already exists
		return newMaterial;
	}
	else
	{
		// Allocate a new material
		newMaterial = new material();
		if (newMaterial)
		{
			mMaterials[name] = newMaterial;
			return newMaterial;
		}
	}

	return NULL;
}

material* materialManager::getMaterial(const std::string& name)
{
	std::map<std::string, material*>::iterator it = mMaterials.find(name);
	if (it != mMaterials.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}

void materialManager::destroyMaterial(const std::string& name)
{
	std::map<std::string, material*>::iterator it = mMaterials.find(name);
	if (it != mMaterials.end())
	{
		material* mat = it->second;

		mMaterials.erase(it);
		delete mat;
	}
}
		
			
void materialManager::parseTextureSection(material* mat, parsingFile* file)
{
	assert(mat != NULL);
	assert(file != NULL);

	std::string token;
	unsigned int openBraces = 1;

	file->skipNextToken(); // { 
	texture* activeTexture = NULL;

	while (openBraces && !file->eof())
	{
		if (token == "filename")
		{
			token = file->getNextToken();
			activeTexture = textureManager::getSingleton().createTexture(token);
			if (!activeTexture)
			{
				S_LOG_INFO("Failled to allocate material texture " + token);
			}
			else
			{
				mat->pushTexture(activeTexture);
			}
		}
		else
		if (token == "scroll")
		{
			if (activeTexture)
			{
				vector2 scroll;
				token = file->getNextToken();
				scroll.x = atof(token.c_str());
				token = file->getNextToken();
				scroll.y = atof(token.c_str());
				
				activeTexture->setScroll(scroll);
			}
			else
			{
				S_LOG_INFO("Error in parsing material texture: You must define first the texture filename");
			}
		}
		else
		if (token == "rotate")
		{
			if (activeTexture)
			{
				vec_t angle;
				token = file->getNextToken();
				angle = atof(token.c_str());

				activeTexture->setRotate(angle);
			}
			else
			{
				S_LOG_INFO("Error in parsing material texture: You must define first the texture filename");
			}
		}
		else
		if (token == "texcoord")
		{
			if (activeTexture)
			{
				token = file->getNextToken();

				texCoordType type = TEXCOORD_NONE;
				if (token == "uv")
					type = TEXCOORD_UV;

				activeTexture->setTexCoordType(type);
			}
			else
			{
				S_LOG_INFO("Error in parsing material texture: You must define first the texture filename");
			}
		}

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;

		token = file->getNextToken();
	}
}

void materialManager::parseMaterial(material* mat, parsingFile* file)
{
	assert(mat != NULL);
	assert(file != NULL);

	std::string token = file->getNextToken();

	file->skipNextToken(); // {
	unsigned int openBraces = 1;

	while (openBraces && !file->eof())
	{
		if (token == "ambient")
		{
			vec_t r, g, b;

			token = file->getNextToken();
			r = atof(token.c_str());
			token = file->getNextToken();
			g = atof(token.c_str());
			token = file->getNextToken();
			b = atof(token.c_str());

			mat->setAmbient(vector3(r, g, b));
		}
		else
		if (token == "diffuse")
		{
			vec_t r, g, b;

			token = file->getNextToken();
			r = atof(token.c_str());
			token = file->getNextToken();
			g = atof(token.c_str());
			token = file->getNextToken();
			b = atof(token.c_str());

			mat->setDiffuse(vector3(r, g, b));
		}
		else
		if (token == "specular")
		{
			vec_t r, g, b;

			token = file->getNextToken();
			r = atof(token.c_str());
			token = file->getNextToken();
			g = atof(token.c_str());
			token = file->getNextToken();
			b = atof(token.c_str());

			mat->setSpecular(vector3(r, g, b));
		}
		else
		if (token == "texture")
		{
			parseTextureSection(mat, file);
		}

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;

		token = file->getNextToken();
	}
}

void materialManager::parseMaterialScript(parsingFile* file)
{
	if (!file->isReady())
	{
		S_LOG_INFO("File " + file->getFilename() + " is not ready to be parsed.");
		return;
	}

	std::string token = file->getNextToken();
	while (!file->eof())
	{
		if (token == "material")
		{
			token = file->getNextToken();
			material* searchMaterial = getMaterial(token);
			if (searchMaterial)
			{
				unsigned int openBraces = 0;

				S_LOG_INFO("Material " + token + " already exists.");
				file->skipNextToken(); // {
				while(true)
				{
					token = file->getNextToken();
					if (token == "{")
						openBraces++;
					else
					if (token == "}")
					{
						if (openBraces == 0)
							break;
						else
							openBraces--;
					}
				}
			}
			else
			{
				material* currentMaterial = createMaterial(token);
				parseMaterial(currentMaterial, file);
			}
		}
		// if (token == "material")
	}
	// while (!file->eof())
}

}

