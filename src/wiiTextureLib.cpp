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

#include <iostream>

// Jpeg is not C++ safe
extern "C" 
{
	#include <jpeglib.h>
	#include <jerror.h>
}

#include "textureLib.h"
#include "pngu.h"
#include "root.h"

namespace k {

template<> textureLoader* singleton<textureLoader>::singleton_instance = 0;

textureLoader& textureLoader::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

kTexture* loadTextureJPEG(const char* file, unsigned short* w, unsigned short* h)
{
	struct jpeg_decompress_struct jInfo;
	struct jpeg_error_mgr jError;

	jInfo.err = jpeg_std_error(&jError);
	jpeg_create_decompress(&jInfo);

	FILE* texFile = fopen(file, "rb");
	if (!texFile)
	{
		S_LOG_INFO("Failed to open texture file " + std::string(file));
		return NULL;
	}

	jpeg_stdio_src(&jInfo, texFile);
	jpeg_read_header(&jInfo, true);

	unsigned int width = jInfo.image_width;
	unsigned int height = jInfo.image_height;
	unsigned int colorSpace = jInfo.jpeg_color_space;

	if (*w && *h)
	{
		*w = jInfo.image_width;
		*h = jInfo.image_height;
	}

	char* textureData = (char*) malloc(colorSpace * width * height);
	if (!textureData)
	{
		fclose(texFile);

		S_LOG_INFO("Failed to allocate texture data for " + std::string(file));
		return NULL;
	}

	// Just a pointer
	char* textureMemory = textureData;

	// Decompress =]
	jpeg_start_decompress(&jInfo);
	while (jInfo.output_scanline < jInfo.output_height)
	{
		jpeg_read_scanlines(&jInfo, (JSAMPLE**)&textureMemory, 1);
		textureMemory += colorSpace * jInfo.output_width;
	}

	fclose(texFile);
	jpeg_finish_decompress(&jInfo);
	jpeg_destroy_decompress(&jInfo);

	// We are done with jpeg =]
	char* wiiTexture = (char*) memalign(32, 4 * width * height);
	if (!wiiTexture)
	{
		free(textureData);
		S_LOG_INFO("Failed to allocate aligned memory for " + std::string(file));

		return NULL;
	}

	// Copy from jpeg source and free 
	char* finalWii = wiiTexture;
	for (int y = 0; y < height; y += 4)
	{
		char* line = &textureData[y * height * colorSpace];
		for (int x = 0; x < width; x += 4)
		{
			char* color = line + (x * colorSpace);
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 4; tx++)
				{
					finalWii[0] = 0xff;
					finalWii[1] = *color;
					finalWii[32] = *(color + 1);
					finalWii[33] = *(color + 2);

					finalWii += 2;
					color += colorSpace;
				}
				color += width * colorSpace - 16;
			}
			finalWii += 32;
		}
	}

	// We dont need raw jpeg data anymore
	free(textureData);

	// New GXTexObj
	kTexture* newKTexture = (kTexture*) memalign(32, sizeof(kTexture));	
	if (!newKTexture)
	{
		S_LOG_INFO("Failed to allocate GX texture object");
		free(wiiTexture);

		return NULL;
	}

	// Flush and set GX Object
	DCFlushRange(wiiTexture, width * height * 4);
	GX_InitTexObj(newKTexture, wiiTexture, width, height, GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, GX_FALSE);
	GX_InitTexObjLOD(newKTexture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

	// Save data for deallocation
	textureLoader::getSingleton().pushTextureData(newKTexture, textureData);
	
	// Syncronization
	GX_InvalidateTexAll();

	return newKTexture;
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

	uint32_t texNumBytes = imgProperties.imgWidth * imgProperties.imgHeight * 4;
	char* textureData = (char*) memalign(32, texNumBytes);
	if (!textureData)
	{
		std::stringstream dbgTex;
		dbgTex << "Failed to allocate data (";
		dbgTex << texNumBytes << "for texture";

		S_LOG_INFO(dbgTex.str());

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

	GX_InitTexObjLOD(newKTexture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

	// Save data for deallocation
	textureLoader::getSingleton().pushTextureData(newKTexture, textureData);
	
	// Syncronization
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
	if (extension == ".jpg" || extension == ".JPG" || extension == ".jpeg" || extension == ".JPEG")
		return loadTextureJPEG(file, w, h);
	else
		return NULL;
}

}

#endif

