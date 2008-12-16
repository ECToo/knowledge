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

#ifdef __WII__
#include "textureLib.h"
#include "pngu.h"
#include "root.h"

namespace k {

template<> textureLoader* singleton<textureLoader>::singleton_instance = 0;

textureLoader& textureLoader::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}

kTexture* loadTexturePNG(const char* file, unsigned short* w, unsigned short* h)
{
	IMGCTX textureCtx = PNGU_SelectImageFromDevice(file);
	if (!textureCtx)
	{
		S_LOG_INFO("Failed to retrieve PNG information.");
		return NULL;
	}
		
	PNGUPROP imgProperties;
	PNGU_GetImageProperties(textureCtx, &imgProperties);

	if (w && h)
	{
		*w = imgProperties.imgWidth;
		*h = imgProperties.imgHeight;
	}

	char* textureData = (char*) memalign(32, imgProperties.imgWidth * imgProperties.imgHeight * 4);
	if (!textureData)
	{
		S_LOG_INFO("Failed to allocate data for texture");

		PNGU_ReleaseImageContext(textureCtx);
		return NULL;
	}

	kTexture* newKTexture = (kTexture*) memalign(32, sizeof(kTexture));	
	if (!newKTexture)
	{
		S_LOG_INFO("Failed to allocate GX texture object");

		free(textureData);

		PNGU_ReleaseImageContext(textureCtx);
		return NULL;
	}
				
	PNGU_DecodeTo4x4RGBA8(textureCtx, imgProperties.imgWidth, imgProperties.imgHeight, textureData, 0xff);
	PNGU_ReleaseImageContext(textureCtx);

	DCFlushRange(textureData, imgProperties.imgWidth * imgProperties.imgHeight * 4);

	GX_InitTexObj(newKTexture, textureData, imgProperties.imgWidth, 
			imgProperties.imgHeight, GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, GX_FALSE);

	// Save data for deallocation
	textureLoader::getSingleton().pushTextureData(newKTexture, textureData);
	
	// Invalidate texture cache since we changed texture data.
	GX_InvalidateTexAll();

	return newKTexture;
}

void textureLoader::unLoadTexture(kTexture* tex)
{
	/*
	std::map<kTexture*, char*>::iterator it = mTextureData.find(tex);
	if (it != mTextureData.end())
	{
		char* data = it->second;
		mTextureData.erase(it);

		free(data);
		free(tex);
	}
	*/
}

kTexture* textureLoader::loadTexture(const char* file, unsigned short* w, unsigned short* h)
{
	std::string extension = getExtension(std::string(file));
	if (extension == ".png" || extension == ".PNG")
		return loadTexturePNG(file, w, h);
	else
		return NULL;
}

}

#endif

