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
	std::list<texture*>::iterator it;
	for (it = mTextures.begin(); it != mTextures.end();)
	{
		texture* tex = (*it);
		it = mTextures.erase(it);
		delete tex;
	}

	// Deinitialize freeimage
	#ifndef __WII__
	FreeImage_DeInitialise();
	#endif
}

texture* textureManager::getTexture(const std::string& filename)
{
	std::list<texture*>::iterator it = mTextures.begin();
	for (; it != mTextures.end(); it++)
	{
		if ((*it)->containsFilename(filename))
			return (*it);
	}
		
	return NULL;
}

texture* textureManager::createEmptyTexture()
{
	texture* newTexture = new texture;
	if (newTexture)
	{
		mTextures.push_back(newTexture);
		return newTexture;
	}
	else
	{
		S_LOG_INFO("Failed to allocate new texture.");
		return NULL;
	}
}
			
bool textureManager::allocateTextureData(const std::string& filename, int wrapBits)
{
	std::string fullPath = filename;

	// Get Path from resource manager (if any)
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc)
	{
		fullPath = rsc->getRoot() + filename;
	}

	texture* newTexture = getTexture(fullPath);
	if (newTexture)
	{
		S_LOG_INFO("Texture data for " + fullPath + " is already allocated.");
		return true;
	}
	else
	{
		newTexture = loadTexture(fullPath, wrapBits);
		if (newTexture)
		{
			mTextures.push_back(newTexture);
			return true;
		}
	}
		
	S_LOG_INFO("Failed to allocate texture data for " + fullPath + ".");
	return false;
}

void textureManager::deallocateTextureData(const std::string& filename)
{
	std::string fullPath = filename;

	// Get Path from resource manager (if any)
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc)
	{
		fullPath = rsc->getRoot() + filename;
	}

	std::list<texture*>::iterator it;
	for (it = mTextures.begin(); it != mTextures.end(); it++)
	{
		texture* tex = (*it);
		mTextures.erase(it);

		delete tex;
		break;
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
	platTextureStage* newStage = new platTextureStage(index);
	if (!newStage)
	{
		S_LOG_INFO("Failed to allocate texture stage.");
		return NULL;
	}

	newStage->setTexture(rawTex);
	return newStage;
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
	platTextureStage* newStage = new platTextureStage(index);
	if (!newStage)
	{
		S_LOG_INFO("Failed to allocate texture stage.");
		return NULL;
	}

	newStage->setTexture(rawTex);
	return newStage;
}

}

