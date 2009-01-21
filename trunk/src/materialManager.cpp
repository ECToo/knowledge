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

	#ifdef __WII__
	mTev = new tevManager();
	assert (mTev);
	#endif
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
			S_LOG_INFO("Material " + name + " created.");
			mMaterials[name] = newMaterial;
			return newMaterial;
		}
	}

	S_LOG_INFO("Failed to create material " + name + ".");
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

		mMaterials.erase(it++);
		delete mat;
	}
}
		
#ifdef __WII__
unsigned short getBlendMode(std::string& mode)
{
	if (mode == "zero")
	{
		return GX_BL_ZERO;
	}
	else 
	if (mode == "one")
	{
		return GX_BL_ONE;
	}
	else 
	if (mode == "srcclr")
	{
		return GX_BL_SRCCLR;
	}
	else 
	if (mode == "invsrcclr")
	{
		return GX_BL_INVSRCCLR;
	}
	else 
	if (mode == "srcalpha")
	{
		return GX_BL_SRCALPHA;
	}
	else 
	if (mode == "invsrcalpha")
	{
		return GX_BL_INVSRCALPHA;
	}
	else 
	if (mode == "dstalpha")
	{
		return GX_BL_DSTALPHA;
	}
	else 
	if (mode == "invdstalpha")
	{
		return GX_BL_INVDSTALPHA;
	}
	else 
	if (mode == "dstclr")
	{
		return GX_BL_DSTCLR;
	}
	else 
	if (mode == "invdstclr")
	{
		return GX_BL_INVDSTCLR;
	}

	return GX_BL_ZERO;
}
#else
unsigned short getBlendMode(std::string& mode)
{
	if (mode == "zero")
	{
		return GL_ZERO;
	}
	else 
	if (mode == "one")
	{
		return GL_ONE;
	}
	else 
	if (mode == "srcclr")
	{
		return GL_SRC_COLOR;
	}
	else 
	if (mode == "invsrcclr")
	{
		return GL_ONE_MINUS_SRC_COLOR;
	}
	else 
	if (mode == "srcalpha")
	{
		return GL_SRC_ALPHA;
	}
	else 
	if (mode == "invsrcalpha")
	{
		return GL_ONE_MINUS_SRC_ALPHA;
	}
	else 
	if (mode == "dstalpha")
	{
		return GL_DST_ALPHA;
	}
	else 
	if (mode == "invdstalpha")
	{
		return GL_ONE_MINUS_DST_ALPHA;
	}
	else 
	if (mode == "dstclr")
	{
		return GL_DST_COLOR;
	}
	else 
	if (mode == "invdstclr")
	{
		return GL_ONE_MINUS_DST_COLOR;
	}

	return GL_ZERO;
}
#endif

static inline void parseUntilEndOfSection(parsingFile* file)
{
	unsigned int openBraces = 0;

	while(true)
	{
		std::string token = file->getNextToken();
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
			
void materialManager::parseTextureSection(material* mat, parsingFile* file, unsigned short index)
{
	assert(mat != NULL);
	assert(file != NULL);

	std::string token;
	unsigned int openBraces = 1;

	file->skipNextToken(); // { 
	textureStage* activeTexture = NULL;

	while (openBraces)
	{
		if (file->eof())
			return;

		if (token == "filename")
		{
			token = file->getNextToken();
			activeTexture = textureManager::getSingleton().createTexture(token, index);
			if (!activeTexture)
			{
				S_LOG_INFO("Failed to allocate material texture " + token);
			}
			else
			{
				mat->pushTexture(activeTexture);
			}
		}
		else
		if (token == "cubename")
		{
			token = file->getNextToken();
			activeTexture = textureManager::getSingleton().createCubicTexture(token, index);
			if (!activeTexture)
			{
				S_LOG_INFO("Failed to allocate material cubic texture " + token);
			}
			else
			{
				mat->pushTexture(activeTexture);
			}
		}
		else
		if (token == "program")
		{
			token = file->getNextToken();
			if (!activeTexture)
			{
				S_LOG_INFO("Error in parsing material texture: You must define first the texture filename");
			}
			else
			{
				activeTexture->setProgram(token);
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
				else
				if (token == "pos")
					type = TEXCOORD_POS;
				else
				if (token == "sphere")
					type = TEXCOORD_SPHERE;
				else
				if (token == "cubemap")
					type = TEXCOORD_CUBEMAP;
				else
				if (token == "eyeLinear")
					type = TEXCOORD_EYE_LINEAR;
				else
				if (token == "normal")
					type = TEXCOORD_NORMAL;
				else
				if (token == "binormal")
					type = TEXCOORD_BINORMAL;
				else
				if (token == "tangent")
					type = TEXCOORD_TANGENT;

				activeTexture->setTexCoordType(type);
			}
			else
			{
				S_LOG_INFO("Error in parsing material texture: You must define first the texture filename");
			}
		}
		else
		if (token == "blendfunc")
		{
			if (activeTexture)
			{
				unsigned short src = 0, dst = 0;

				token = file->getNextToken();
				src = getBlendMode(token);
				token = file->getNextToken();
				dst = getBlendMode(token);

				activeTexture->setBlendMode(src, dst);
			}
			else
			{
				S_LOG_INFO("Error in parsing material texture: You must define first the texture filename");
			}
		}

		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;
	}
}

void materialManager::parseMaterial(material* mat, parsingFile* file)
{
	assert(mat != NULL);
	assert(file != NULL);

	std::string token = file->getNextToken(); // {
	unsigned int openBraces = 1;
	unsigned short textureIndex = 0;

	while (openBraces)
	{
		if (file->eof())
			return;

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
		if (token == "cull")
		{
			token = file->getNextToken();
			if (token == "none" || token == "disabled")
			{
				mat->setCullMode(CULLMODE_NONE);
			}
			else
			if (token == "front")
				mat->setCullMode(CULLMODE_FRONT);
			else
			if (token == "back")
				mat->setCullMode(CULLMODE_BACK);
			else
			if (token == "both")
				mat->setCullMode(CULLMODE_BOTH);
		}
		else
		if (token == "depth_enabled")
		{
			token = file->getNextToken();
			if (token == "true" || token == "yes")
				mat->setDepthTest(true);
			else
				mat->setDepthTest(false);
		}
		else
		if (token == "texture")
		{
			parseTextureSection(mat, file, textureIndex++);
		}

		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;
	}

	mat->setTextureUnits(textureIndex);
}

void materialManager::parseMaterialScript(const std::string& filename, materialList* map)
{
	parsingFile* newFile = new parsingFile(filename);

	assert(newFile != NULL);
	parseMaterialScript(newFile, map);

	delete newFile;
}

void materialManager::parseMaterialScript(parsingFile* file, materialList* map)
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
				S_LOG_INFO("Material " + token + " already exists.");
				file->skipNextToken(); // {
				parseUntilEndOfSection(file);
			}
			else
			{
				material* currentMaterial = createMaterial(token);
				if (currentMaterial && map)
					map->push_back(token);

				parseMaterial(currentMaterial, file);
			}
		}
		// if (token == "material")
		
		// Next Token
		token = file->getNextToken();
	}
	// while (!file->eof())
}

}

