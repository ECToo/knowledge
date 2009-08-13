/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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
#include "root.h"

namespace k {

template<> materialManager* singleton<materialManager>::singleton_instance = 0;

materialManager& materialManager::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

materialManager::materialManager()
{
	mMaterials.clear();
}

materialManager::~materialManager()
{
	std::map<std::string, material*>::iterator it;
	for (it = mMaterials.begin(); it != mMaterials.end(); it++)
	{
		delete it->second;
	}
}

void materialManager::createSystemMaterials()
{
	// Create base materials (k_base_white, k_base_black and k_base_null)
	texture* whiteTexture = textureManager::getSingleton().getTexture("k_base_white");
	if (whiteTexture) 
	{
		material* whiteMaterial = createMaterial("k_base_white", whiteTexture);
		if (whiteMaterial) whiteMaterial->setReceiveLight(false);
	}

	texture* blackTexture = textureManager::getSingleton().getTexture("k_base_black");
	if (blackTexture)
	{
		material* blackMaterial = createMaterial("k_base_black", blackTexture);
		if (blackMaterial) blackMaterial->setReceiveLight(false);
	}

	texture* nullTexture = textureManager::getSingleton().getTexture("k_base_null");
	if (nullTexture)
	{
		material* nullMaterial = createMaterial("k_base_null", nullTexture);
		if (nullMaterial) nullMaterial->setReceiveLight(false);
	}
}
			
material* materialManager::createMaterial(const std::string& name)
{
	material* newMaterial = getMaterial(name);
	if (newMaterial)
	{
		// Material already exists
		return newMaterial;
	}
		
	// Allocate a new material
	try
	{
		newMaterial = new material();
		S_LOG_INFO("Material " + name + " created.");
		mMaterials[name] = newMaterial;
		return newMaterial;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to create material " + name + ".");
		return NULL;
	}
}
			
material* materialManager::createMaterial(const std::string& name, texture* tex)
{
	material* newMaterial = getMaterial(name);
	if (newMaterial)
	{
		// Material already exists
		return newMaterial;
	}
		
	// Allocate a new material
	try
	{
		newMaterial = new material(tex);
		S_LOG_INFO("Material " + name + " created.");
		mMaterials[name] = newMaterial;
		return newMaterial;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to create material " + name + ".");
		return NULL;
	}
}

material* materialManager::createMaterial(const std::string& name, const std::string& filename)
{
	material* newMaterial = getMaterial(name);
	if (newMaterial)
	{
		// Material already exists
		return newMaterial;
	}
		
	// Allocate a new material
	try
	{
		newMaterial = new material(filename);
		S_LOG_INFO("Material " + name + " created.");
		mMaterials[name] = newMaterial;
		return newMaterial;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to create material " + name + ".");
		return NULL;
	}
}

material* materialManager::getMaterial(const std::string& name) const
{
	std::map<std::string, material*>::const_iterator it = mMaterials.find(name);
	if (it != mMaterials.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}
			
material* materialManager::getMaterialWithFilename(const std::string& filename) const
{
	std::map<std::string, material*>::const_iterator it;
	for (it = mMaterials.begin(); it != mMaterials.end(); it++)
	{
		if (it->first == filename || it->second->containsTexture(filename))
			return it->second;
	}
		
	return NULL;
}

void materialManager::destroyMaterial(const std::string& name)
{
	std::map<std::string, material*>::iterator it = mMaterials.find(name);
	if (it != mMaterials.end())
	{
		delete it->second;
		mMaterials.erase(it);
	}
}
		
#ifdef __WII__
unsigned short getBlendMode(const std::string& mode)
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
unsigned short getBlendMode(const std::string& mode)
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
				
			if (openBraces == 0)
				break;
		}
	}
}
			
void materialManager::parseTextureSection(material* mat, parsingFile* file, unsigned short index)
{
	kAssert(mat != NULL);
	kAssert(file != NULL);

	std::string token;
	unsigned int openBraces = 1;

	file->skipNextToken(); // { 

	// Start texture stage
	materialStage* activeStage;
	unsigned int stageTextures = 0;
	try
	{
		activeStage = new materialStage(index);
	}

	catch (...)
	{
		S_LOG_INFO("Failed to create a new material stage.");
		return;
	}

	// insert into material
	mat->pushStage(activeStage);

	while (openBraces)
	{
		if (file->eof())
			return;

		if (token == "filename")
		{
			token = file->getNextToken();
			texture* newTexture = textureManager::getSingleton().allocateTexture(token);

			if (newTexture)
				activeStage->setTexture(newTexture, stageTextures++);
		}
		else
		if (token == "animname")
		{
			bool readingWords = false;
			unsigned int numberOfImages = 0;
			vec_t frameRate = 0;

			std::string filename = file->getNextToken();
			token = file->getNextToken();

			if (!isNumeric(token))
			{
				// Register first filename
				texture* tempTex = textureManager::getSingleton().allocateTexture(filename);
				if (tempTex) activeStage->setTexture(tempTex, 0);

				numberOfImages++;
				readingWords = true;
			}

			if (isNumeric(token))
			{
				// We got the number of frames
				numberOfImages = atoi(token.c_str());
				frameRate = atof(file->getNextToken().c_str());

				if (numberOfImages > K_MAX_STAGE_TEXTURES)
					numberOfImages = K_MAX_STAGE_TEXTURES;
			
				std::string extension = getExtension(filename);
				filename.erase(filename.length() - extension.length(), extension.length());

				for (unsigned int i = 0; i < numberOfImages; i++)
				{
					texture* tempTex = NULL;
					char numStr[6];
	
					memset(numStr, 0, 6);
					sprintf(numStr, "_%d", i);

					std::string fullName = filename + numStr + extension;
					tempTex = textureManager::getSingleton().allocateTexture(fullName);
					if (tempTex)
						activeStage->setTexture(tempTex, i);
				}
			}
			else
			while (!isNumeric(token))
			{
				if (token == "}")
				{
					openBraces--;
					continue;
				}

				if (numberOfImages < K_MAX_STAGE_TEXTURES)
				{
					// We havent got the number of frames, so we are getting file names =]
					texture* tempTex = textureManager::getSingleton().allocateTexture(token);
					if (tempTex) activeStage->setTexture(tempTex, numberOfImages);
				
					numberOfImages++;
				}

				token = file->getNextToken();
			}	

			// We got the frameRate last =]
			if (readingWords)
				frameRate = atoi(token.c_str());

			activeStage->setFrameRate(frameRate);
			activeStage->setNumberOfFrames(numberOfImages);
		}
		else
		if (token == "cubename")
		{
			token = file->getNextToken();

			std::string extension = getExtension(token);
			token.erase(token.length() - extension.length(), extension.length());

			int flags = FLAG_CLAMP_EDGE_S | FLAG_CLAMP_EDGE_T;

			// Temporary
			std::string tempName;
			texture* tempTex;

			// Front
			tempName = token + "_front" + extension;
			tempTex = textureManager::getSingleton().allocateTexture(tempName, flags);
			if (!tempTex)
				goto endOfCubeName;
			else
				activeStage->setTexture(tempTex, CUBE_FRONT);

			// Back 
			tempName = token + "_back" + extension;
			tempTex = textureManager::getSingleton().allocateTexture(tempName, flags);
			if (!tempTex)
				goto endOfCubeName;
			else
				activeStage->setTexture(tempTex, CUBE_BACK);

			// Left
			tempName = token + "_left" + extension;
			tempTex = textureManager::getSingleton().allocateTexture(tempName, flags);
			if (!tempTex)
				goto endOfCubeName;
			else
				activeStage->setTexture(tempTex, CUBE_LEFT);

			// Right
			tempName = token + "_right" + extension;
			tempTex = textureManager::getSingleton().allocateTexture(tempName, flags);
			if (!tempTex)
				goto endOfCubeName;
			else
				activeStage->setTexture(tempTex, CUBE_RIGHT);

			// Up
			tempName = token + "_up" + extension;
			tempTex = textureManager::getSingleton().allocateTexture(tempName, flags);
			if (!tempTex)
				goto endOfCubeName;
			else
				activeStage->setTexture(tempTex, CUBE_UP);

			// Down
			tempName = token + "_down" + extension;
			tempTex = textureManager::getSingleton().allocateTexture(tempName, flags);
			if (!tempTex)
				goto endOfCubeName;
			else
				activeStage->setTexture(tempTex, CUBE_DOWN);

// i hate this but lets make the code more concise
endOfCubeName:
			flags = 0;
		}
		else
		if (token == "texenv")
		{
			token = file->getNextToken();

			if (token == "replace")
				activeStage->setEnv(TEXENV_REPLACE);
			else
			if (token == "modulate")
				activeStage->setEnv(TEXENV_MODULATE);
			else
			if (token == "add")
				activeStage->setEnv(TEXENV_ADD);
			else
			if (token == "decal")
				activeStage->setEnv(TEXENV_DECAL);
			else
			if (token == "blend")
				activeStage->setEnv(TEXENV_BLEND);
		}
		else
		if (token == "scale")
		{
			vector2 scale;
			token = file->getNextToken();
			scale.x = atof(token.c_str());
			token = file->getNextToken();
			scale.y = atof(token.c_str());
				
			activeStage->setScale(scale);
		}
		else
		if (token == "scroll")
		{
			vector2 scroll;
			token = file->getNextToken();
			scroll.x = atof(token.c_str());
			token = file->getNextToken();
			scroll.y = atof(token.c_str());
				
			activeStage->setScroll(scroll);
		}
		else
		if (token == "rotate")
		{
			vec_t angle;
			token = file->getNextToken();
			angle = atof(token.c_str());

			activeStage->setRotate(angle);
		}
		else
		if (token == "texcoord")
		{
			token = file->getNextToken();

			TextureCoordType type = TEXCOORD_NONE;
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

			activeStage->setCoordType(type);
		}
		else
		if (token == "blendfunc")
		{
			unsigned short src = 0, dst = 0;

			token = file->getNextToken();
			src = getBlendMode(token);
			token = file->getNextToken();
			dst = getBlendMode(token);

			activeStage->setBlendMode(src, dst);
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
	kAssert(mat != NULL);
	kAssert(file != NULL);

	std::string token = file->getNextToken(); // {
	unsigned int openBraces = 1;
	unsigned short textureIndex = 0;

	while (openBraces)
	{
		if (file->eof())
			return;

		if (token == "nodraw")
		{
			mat->setNoDraw(true);
		}
		else
		if (token == "receiveLight")
		{
			token = file->getNextToken();
			if (token == "no" || token == "false" || token == "off")
				mat->setReceiveLight(false);
		}
		else
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
		if (token == "depthTest")
		{
			token = file->getNextToken();
			if (token == "true" || token == "yes" || token == "on")
				mat->setDepthTest(true);
			else
				mat->setDepthTest(false);
		}
		else
		if (token == "depthWrite")
		{
			token = file->getNextToken();
			if (token == "true" || token == "yes" || token == "on")
				mat->setDepthWrite(true);
			else
				mat->setDepthWrite(false);
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
}

void materialManager::parseMaterialScript(const std::string& filename, materialList* map)
{
	try
	{
		parsingFile* newFile = new parsingFile(filename);
		parseMaterialScript(newFile, map);

		delete newFile;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to create new parsing file for " + filename);
	}
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

inline std::string translateQ3Blend(const std::string& mode)
{
	if (mode == "GL_ONE")
		return std::string("one");
	else
	if (mode == "GL_ZERO")
		return std::string("zero");
	else
	if (mode == "GL_DST_COLOR")
		return std::string("dstclr");
	else
	if (mode == "GL_ONE_MINUS_DST_COLOR")
		return std::string("invdstclr");
	else
	if (mode == "GL_SRC_ALPHA")
		return std::string("srcalpha");
	else
	if (mode == "GL_ONE_MINUS_SRC_ALPHA")
		return std::string("invsrcalpha");
	else
		return std::string("one");
};
			
void materialManager::parseQ3TextureSection(material* mat, parsingFile* file, unsigned short index)
{
	kAssert(mat != NULL);
	kAssert(file != NULL);

	std::string token;
	unsigned int openBraces = 1;

	materialStage* activeStage;
	unsigned int stageTextures = 0;
	try
	{
		activeStage = new materialStage(index);
	}

	catch (...)
	{
		S_LOG_INFO("Failed to create a new material stage.");
		return;
	}

	// insert into material
	mat->pushStage(activeStage);

	while (openBraces)
	{
		if (file->eof())
			return;

		if (token == "map")
		{
			token = file->getNextToken();
			if (token  == "$lightmap")
			{
				// TODO
			}
			else
			{
				texture* newTexture = textureManager::getSingleton().getTexture(token);
				
				if (!newTexture && textureManager::getSingleton().allocateTexture(token, FLAG_REPEAT_S | FLAG_REPEAT_T | FLAG_REPEAT_R))
					newTexture = textureManager::getSingleton().getTexture(token);
				
				if (newTexture)
					activeStage->setTexture(newTexture, stageTextures++);
			}
		}
		else
		if (token == "clampmap")
		{
			token = file->getNextToken();
			if (token  == "$lightmap")
			{
				// TODO
			}
			else
			{
				texture* newTexture = textureManager::getSingleton().getTexture(token);
				
				if (!newTexture && textureManager::getSingleton().allocateTexture(token))
					newTexture = textureManager::getSingleton().getTexture(token);
				
				if (newTexture)
					activeStage->setTexture(newTexture, stageTextures++);
			}
		}
		else
		if (token == "tcMod")
		{
			token = file->getNextToken();
			if (activeStage && token == "scroll")
			{
				vector2 scroll;
				token = file->getNextToken();
				scroll.x = atof(token.c_str());
				token = file->getNextToken();
				scroll.y = atof(token.c_str());
				
				activeStage->setScroll(scroll);
			}
			else
			if (activeStage && token == "scale")
			{
				vector2 scale;
				token = file->getNextToken();
				scale.x = atof(token.c_str());
				token = file->getNextToken();
				scale.y = atof(token.c_str());
				
				activeStage ->setScale(scale);
			}
			else
			if (activeStage && token == "rotate")
			{
				vec_t angle;
				token = file->getNextToken();
				angle = atof(token.c_str());

				activeStage->setRotate(angle);
			}
		}
		else
		if (token == "blendfunc")
		{
			token = file->getNextToken();
			if (activeStage && token == "add")
			{
				activeStage->setBlendMode(getBlendMode("one"), getBlendMode("one"));
			}
			else
			if (activeStage && token == "filter")
			{
				activeStage->setBlendMode(getBlendMode("dstclr"), getBlendMode("zero"));
			}
			else
			if (activeStage && token == "blend")
			{
				activeStage->setBlendMode(getBlendMode("srcalpha"), getBlendMode("invsrcalpha"));
			}
			else
			if (activeStage)
			{
				unsigned short src = 0, dst = 0;

				src = getBlendMode(translateQ3Blend(token));
				token = file->getNextToken();
				dst = getBlendMode(translateQ3Blend(token));

				activeStage->setBlendMode(src, dst);
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

void materialManager::parseQ3Material(material* mat, parsingFile* file)
{
	kAssert(mat != NULL);
	kAssert(file != NULL);

	std::string token = file->getNextToken(); // {
	unsigned int openBraces = 1;
	unsigned short textureIndex = 0;

	while (openBraces)
	{
		if (file->eof())
			return;

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
		if (token == "{")
		{
			parseQ3TextureSection(mat, file, textureIndex++);
		}

		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
	}
}

void materialManager::parseQ3MaterialScript(const std::string& filename, materialList* map)
{
	try
	{
		parsingFile* newFile = new parsingFile(filename);
		parseQ3MaterialScript(newFile, map);

		delete newFile;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to create parsing file for " + filename);
	}
}

void materialManager::parseQ3MaterialScript(parsingFile* file, materialList* map)
{
	if (!file->isReady())
	{
		S_LOG_INFO("File " + file->getFilename() + " is not ready to be parsed.");
		return;
	}

	std::string token = file->getNextToken();
	while (!file->eof())
	{
		// Q3 Material
		if (token != "{" && token != "{")
		{
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

				parseQ3Material(currentMaterial, file);
			}
		}
		// if (token == "material")
		
		// Next Token
		token = file->getNextToken();
	}
	// while (!file->eof())
}

}

