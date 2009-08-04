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

#include "textureManager.h"
#include "logger.h"
#include "resourceManager.h"

namespace k {

template<> textureManager* singleton<textureManager>::singleton_instance = 0;

textureManager& textureManager::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

textureManager::textureManager()
{
	#ifdef __WII__
	GX_InvalidateTexAll();
	#else
	FreeImage_Initialise(false);
	#endif

	mTextures.clear();
}

textureManager::~textureManager()
{
	std::map<int, texture*>::iterator it;
	while (!mTextures.empty())
	{
		it = mTextures.begin();
		delete it->second;
		mTextures.erase(it);
	}

	mTextures.clear();

	// Deinitialize freeimage
	#ifndef __WII__
	FreeImage_DeInitialise();
	#endif
}
			
unsigned int textureManager::getKey(const std::string& filename)
{
	unsigned int hashKey = 0;

	// Generic key generation
	for (unsigned int i = 0; i < filename.length(); i++)
		hashKey += filename[i] * i;

	return hashKey;
}

texture* textureManager::getTexture(const std::string& filename)
{
	textureHash::const_iterator it = mTextures.find(getKey(filename));
	if (it != mTextures.end())
		return it->second;
	else
		return NULL;
}

bool textureManager::allocateTexture(const std::string& filename, int wrapBits)
{
	std::string fullPath = filename;

	// Get Path from resource manager (if any)
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) fullPath = rsc->getRoot() + filename;

	texture* newTexture = getTexture(filename);
	if (newTexture)
	{
		return true;
	}
	else
	{
		newTexture = new texture(fullPath, wrapBits);
		if (newTexture)
		{
			mTextures.insert(textureHash::value_type(getKey(filename), newTexture));
			return true;
		}
	}
		
	S_LOG_INFO("Failed to allocate texture data for " + fullPath + ".");
	return false;
}

/*
texture* textureManager::createEmptyTexture()
{
	try
	{
		texture* newTexture = new texture;

		mTextures.push_back(newTexture);
		return newTexture;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate new texture.");
		return NULL;
	}
}
			

textureStage* textureManager::createCubicTexture(const std::string& filename, unsigned short index, int wrapBits)
{
	// 
	std::string fullPath = filename;

	// Get Path from resource manager (if any)
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc)
	{
		fullPath = rsc->getRoot() + filename;
	}

	texture* rawTex = getTexture(fullPath);
	if (!rawTex)
	{
		rawTex = loadCubemap(fullPath, wrapBits);
		if (!rawTex)
		{
			S_LOG_INFO("Failed to allocate cubemap texture " + fullPath);
			return NULL;
		}

		mTextures.push_back(rawTex);
	}

	// Ok our texture is valid, create the real thing now
	try
	{
		platTextureStage* newStage = new platTextureStage(index);
		newStage->setTexture(rawTex);
		return newStage;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate texture stage.");
		return NULL;
	}
}

textureStage* textureManager::createTexture(const std::string& filename, unsigned short index, int wrapBits)
{
	std::string fullPath = filename;

	// Get Path from resource manager (if any)
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc)
	{
		fullPath = rsc->getRoot() + filename;
	}

	texture* rawTex = getTexture(fullPath);
	if (!rawTex)
	{
		rawTex = loadTexture(fullPath, wrapBits);
		if (!rawTex)
		{
			S_LOG_INFO("Failed to allocate texture " + fullPath);
			return NULL;
		}

		mTextures.push_back(rawTex);
	}

	// Ok our texture is valid, create the real thing now
	try 
	{
		platTextureStage* newStage = new platTextureStage(index);
		newStage->setTexture(rawTex);
		return newStage;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate texture stage.");
		return NULL;
	}
}
			
textureStage* textureManager::createStage(unsigned short index)
{
	try
	{
		platTextureStage* newStage = new platTextureStage(index);
		return newStage;
	}
	catch (...)
	{
		S_LOG_INFO("Failed to allocate texture stage.");
		return NULL;
	}
}

void textureManager::setStageTexture(textureStage* newStage, const std::string& filename, int wrapBits)
{
	if (!newStage)
	{
		S_LOG_INFO("Invalid pointer to textureStage.");
		return;
	}

	// Copy filename
	std::string fullPath = filename;

	// Get Path from resource manager (if any)
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) fullPath = rsc->getRoot() + filename;

	texture* rawTex = getTexture(fullPath);
	if (!rawTex)
	{
		rawTex = loadTexture(fullPath, wrapBits);
		if (!rawTex)
		{
			S_LOG_INFO("Failed to allocate texture " + fullPath);
			return;
		}

		mTextures.push_back(rawTex);
	}

	newStage->setTexture(rawTex);
}

void textureManager::setStageCubicTexture(textureStage* newStage, const std::string& filename, int wrapBits)
{
	if (!newStage)
	{
		S_LOG_INFO("Invalid pointer to textureStage.");
		return;
	}

	std::string fullPath = filename;

	// Get Path from resource manager (if any)
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc)
	{
		fullPath = rsc->getRoot() + filename;
	}

	texture* rawTex = getTexture(fullPath);
	if (!rawTex)
	{
		rawTex = loadCubemap(fullPath, wrapBits);
		if (!rawTex)
		{
			S_LOG_INFO("Failed to allocate cubemap texture " + fullPath);
			return;
		}

		mTextures.push_back(rawTex);
	}

	newStage->setTexture(rawTex);
}
*/

}

