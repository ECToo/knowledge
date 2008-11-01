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
#include "textureLib.h"

namespace k {

template<> textureManager* singleton<textureManager>::singleton_instance = 0;

textureManager& textureManager::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}

textureManager::textureManager()
{
	#ifdef __WII__
	GX_InvalidateTexAll();
	#else
	ilInit();
	ilEnable(IL_CONV_PAL);
	iluInit();
	ilutInit();
	#endif

	mTextures.clear();
}

textureManager::~textureManager()
{
	std::map<std::string, texture*>::iterator it;
	for (it = mTextures.begin(); it != mTextures.end(); )
	{
		texture* tex = it->second;
		mTextures.erase(it++);

		delete tex;
	}
}

texture* textureManager::getTexture(const std::string& filename)
{
	std::map<std::string, texture*>::iterator it = mTextures.find(filename);
	if (it != mTextures.end())
	{
		return it->second;
	}
	else	
	{
		return NULL;
	}
}

			
void textureManager::allocateTextureData(const std::string& filename)
{
	texture* newTexture = getTexture(filename);
	if (newTexture)
	{
		S_LOG_INFO("Texture data for " + filename + " is already allocated.");
	}
	else
	{
		newTexture = createRawTexture(filename);
		if (newTexture)
		{
			S_LOG_INFO("Texture data for " + filename + " allocated.");
			mTextures[filename] = newTexture;
		}
	}
}

void textureManager::deallocateTextureData(const std::string& filename)
{
	std::map<std::string, texture*>::iterator it = mTextures.find(filename);
	if (it != mTextures.end())
	{
		texture* tex = it->second;
		mTextures.erase(it++);

		delete tex;
	}
}

textureStage* textureManager::createTexture(const std::string& filename, unsigned short index)
{
	texture* rawTex = getTexture(filename);
	if (!rawTex)
	{
		rawTex = createRawTexture(filename);
		if (!rawTex)
		{
			S_LOG_INFO("Failed to allocate texture " + filename);
			return NULL;
		}
	}

	// Ok our texture is valid, create the real thing now
	#ifdef __WII__
	wiiTexture* newStage = new wiiTexture(rawTex->mWidth, rawTex->mHeight, index);
	#else
	glTexture* newStage = new glTexture(rawTex->mWidth, rawTex->mHeight, index);
	#endif

	if (!newStage)
	{
		S_LOG_INFO("Failed to allocate texture stage.");
		return NULL;
	}

	newStage->setId(rawTex->mId);
	return newStage;
}

}

