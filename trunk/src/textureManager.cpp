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
#include "root.h"

namespace k {

template<> textureManager* singleton<textureManager>::singleton_instance = 0;

textureManager& textureManager::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

textureManager::textureManager()
{
	// TODO: Remove this from here
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
			
texture* textureManager::getTexture(const std::string& filename)
{
	textureHash::const_iterator it = mTextures.find(getHashKey(filename));
	if (it != mTextures.end())
		return it->second;
	else
		return NULL;
}

texture* textureManager::allocateTexture(const std::string& filename, int wrapBits)
{
	texture* newTexture = getTexture(filename);

	// Check for existence
	if (newTexture)
		return newTexture;

	// It doesnt exist, allocate it
	std::string fullPath = filename;

	// Get Path from resource manager (if any)
	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) fullPath = rsc->getRoot() + filename;

	newTexture = getTexture(filename);
	if (newTexture)
	{
		return newTexture;
	}
	else
	{
		newTexture = new texture(fullPath, wrapBits);
		if (newTexture)
		{
			mTextures[getHashKey(filename)] = newTexture;
			return newTexture;
		}
	}
		
	S_LOG_INFO("Failed to allocate texture data for " + fullPath + ".");
	return NULL;
}

void textureManager::createSystemTextures()
{
	// Register some basic textures (k_base_white, k_base_black, k_base_null)
	// RGBA: 4x4
	const char whiteTextureData[64] = {
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255
	};

	try
	{
		texture* whiteTexture = new texture((void*)whiteTextureData, 4, 4, FLAG_REPEAT_S | FLAG_REPEAT_T | FLAG_REPEAT_R, TEX_RGBA);
		mTextures[getHashKey("k_base_white")] = whiteTexture;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate white texture");
		return;
	}

	const unsigned char blackTextureData[64] = { 
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255,
		0, 0, 0, 255
	};

	try
	{
		texture* blackTexture = new texture((void*)blackTextureData, 4, 4, FLAG_REPEAT_S | FLAG_REPEAT_T | FLAG_REPEAT_R, TEX_RGBA);
		mTextures[getHashKey("k_base_black")] = blackTexture;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate black texture");
		return;
	}

	const unsigned char nullTextureData[64] = {
		255, 0, 255, 255,
		255, 0, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 0, 255, 255,
		255, 0, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 0, 255, 255,
		255, 0, 255, 255,
		255, 255, 255, 255,
		255, 255, 255, 255,
		255, 0, 255, 255,
		255, 0, 255, 255
	};

	try
	{
		texture* nullTexture = new texture((void*)nullTextureData, 4, 4, FLAG_REPEAT_S | FLAG_REPEAT_T | FLAG_REPEAT_R, TEX_RGBA);
		mTextures[getHashKey("k_base_null")] = nullTexture;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate null texture");
		return;
	}
}

}

