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

#include "pngu.h"
#include "root.h"
#include "resourceManager.h"

namespace k {

void unloadTexture(kTexture* tex) {}

typedef struct
{
	kTexture* tex;
	char* data;
} wiiKTexture;

wiiKTexture* loadTextureJPEG(const std::string& file, unsigned short* w, unsigned short* h, int wrapBits)
{
	struct jpeg_decompress_struct jInfo;
	struct jpeg_error_mgr jError;

	jInfo.err = jpeg_std_error(&jError);
	jpeg_create_decompress(&jInfo);

	FILE* texFile = fopen(file.c_str(), "rb");
	if (!texFile)
	{
		S_LOG_INFO("Failed to open texture file " + file);
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

		S_LOG_INFO("Failed to allocate texture data for " + file);
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
		S_LOG_INFO("Failed to allocate aligned memory for " + file);

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
	kTexture* newKTexture = (kTexture*) malloc(sizeof(kTexture));	
	if (!newKTexture)
	{
		S_LOG_INFO("Failed to allocate GX texture object");
		free(wiiTexture);

		return NULL;
	}

	// Flush and set GX Object
	DCFlushRange(wiiTexture, width * height * 4);

	int envS, envT;
	if (wrapBits & (1 << FLAG_CLAMP_S))
		envS = GX_CLAMP;
	else
		envS = GX_REPEAT;

	if (wrapBits & (1 << FLAG_CLAMP_T))
		envT = GX_CLAMP;
	else
		envT = GX_REPEAT;

	GX_InitTexObj(newKTexture, wiiTexture, width, height, GX_TF_RGBA8, envS, envT, GX_FALSE);
	GX_InitTexObjLOD(newKTexture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

	// Syncronization
	GX_InvalidateTexAll();

	wiiKTexture* newWiiKtexture = (wiiKTexture*)malloc(sizeof(wiiKTexture));
	if (!newWiiKtexture)
	{
		S_LOG_INFO("Failed to allocate wiikTexture.");
		free(wiiTexture);
		free(newKTexture);

		return NULL;
	}

	resourceManager::getSingleton().addMemoryUse(4 * width * height);

	newWiiKtexture->tex = newKTexture;
	newWiiKtexture->data = wiiTexture;

	return newWiiKtexture;
}

wiiKTexture* loadTexturePNG(const std::string& file, unsigned short* w, unsigned short* h, int wrapBits)
{
	IMGCTX textureCtx = PNGU_SelectImageFromDevice(file.c_str());
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

	kTexture* newKTexture = (kTexture*) malloc(sizeof(kTexture));	
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

	int envS, envT;
	if (wrapBits & (1 << FLAG_CLAMP_S))
		envS = GX_CLAMP;
	else
		envS = GX_REPEAT;

	if (wrapBits & (1 << FLAG_CLAMP_T))
		envT = GX_CLAMP;
	else
		envT = GX_REPEAT;

	GX_InitTexObj(newKTexture, textureData, imgProperties.imgWidth, 
			imgProperties.imgHeight, GX_TF_RGBA8, envS, envT, GX_FALSE);

	GX_InitTexObjLOD(newKTexture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

	// Syncronization
	GX_InvalidateTexAll();

	wiiKTexture* newWiiKtexture = (wiiKTexture*)malloc(sizeof(wiiKTexture));
	if (!newWiiKtexture)
	{
		S_LOG_INFO("Failed to allocate wiikTexture.");
		free(textureData);
		free(newKTexture);

		return NULL;
	}

	resourceManager::getSingleton().addMemoryUse(4 * imgProperties.imgWidth * imgProperties.imgHeight);

	newWiiKtexture->tex = newKTexture;
	newWiiKtexture->data = textureData;

	return newWiiKtexture;
}

wiiKTexture* loadWiiTexture(const std::string& file, unsigned short* w, unsigned short* h, int wrapBits)
{
	std::string extension = getExtension(std::string(file));
	if (extension == ".png" || extension == ".PNG")
		return loadTexturePNG(file, w, h, wrapBits);
	else
	if (extension == ".jpg" || extension == ".JPG" || extension == ".jpeg" || extension == ".JPEG")
		return loadTextureJPEG(file, w, h, wrapBits);
	else
		return NULL;
}

texture* loadTexture(const std::string& filename, int wrapBits)
{
	unsigned short width, height;
	wiiKTexture* tex = loadWiiTexture(filename, &width, &height, wrapBits);
	if (!tex)
	{
		S_LOG_INFO("Failed to load texture " + filename);
		return NULL;
	}

	texture* newTexture = new texture;
	if (!newTexture)
	{
		S_LOG_INFO("Failed to allocate memory for texture.");
		return NULL;
	}

	newTexture->push(tex->tex, width, height);
	newTexture->push(tex->data);
	newTexture->push(filename);

	return newTexture;
}

texture* loadCubemap(const std::string& filename, int wrapBits)
{
	texture* newTexture = new texture;
	if (!newTexture)
	{
		S_LOG_INFO("Failed to allocate texture for cubemap.");
		return NULL;
	}

	// Get Temp name
	std::string newName = filename;
	std::string extension = getExtension(filename);

	newName.erase(newName.length() - 4, 4);

	// Now we are going to get the filename,
	// append the suffixes and the extensions to
	// retrieve the 6 textures
	std::string tempName;	

	// Raw Textures
	wiiKTexture* tempTex = NULL;
		
	// Front
	tempName = newName + "_front" + extension;	
	tempTex = loadWiiTexture(tempName, NULL, NULL, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex->tex, 0, 0);
	newTexture->push(tempTex->data);

	// Back
	tempName = newName + "_back" + extension;	
	tempTex = loadWiiTexture(tempName, NULL, NULL, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex->tex, 0, 0);
	newTexture->push(tempTex->data);

	// Left 
	tempName = newName + "_left" + extension;	
	tempTex = loadWiiTexture(tempName, NULL, NULL, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex->tex, 0, 0);
	newTexture->push(tempTex->data);

	// Right
	tempName = newName + "_right" + extension;	
	tempTex = loadWiiTexture(tempName, NULL, NULL, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex->tex, 0, 0);
	newTexture->push(tempTex->data);

	// Up
	tempName = newName + "_up" + extension;	
	tempTex = loadWiiTexture(tempName, NULL, NULL, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex->tex, 0, 0);
	newTexture->push(tempTex->data);

	// Final Size
	unsigned short width = 0;
	unsigned short height = 0;

	// Down 
	tempName = newName + "_down" + extension;	
	tempTex = loadWiiTexture(tempName, &width, &height, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}

	newTexture->push(tempTex->tex, width, height);
	newTexture->push(tempTex->data);
	newTexture->push(filename);

	if (!isPowerOfTwo(width) || !isPowerOfTwo(height))
	{
		std::stringstream warn;
		warn << "WARNING! The texture " << filename << " dimensions(";
		warn << width << "," << height;
		warn << ") are not power of 2";

		S_LOG_INFO(warn.str());
	}

	return newTexture;
}

texture* createRawTexture(unsigned char* data, int w, int h, int flags)
{
	kAssert(data);

	kTexture* gxImage = (kTexture*) malloc(sizeof(kTexture));
	if (!gxImage)
	{
		S_LOG_INFO("Failed to allocate GXTexObj for texture.");
		return NULL;
	}

	char* wiiData = (char*) memalign(32, 4 * w * h);
	if (!wiiData)
	{
		S_LOG_INFO("Failed to allocate aligned memory to raw texture.");
		free(gxImage);

		return NULL;
	}

	char* finalWii = wiiData;
	for (int y = 0; y < height; y += 4)
	{
		char* line = &data[y * height * colorSpace];
		for (int x = 0; x < width; x += 4)
		{
			char* color = line + (x * colorSpace);
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 4; tx++)
				{
					if ((flags & (1 << FLAG_RGB)) || (flags & (1 << FLAG_RGBA)))
					{
						if (flags & (1 << FLAG_RGBA))
							finalWii[0] = *(color + 3);
						else
							finalWii[0] = 0xff;

						finalWii[1] = *color;
						finalWii[32] = *(color + 1);
						finalWii[33] = *(color + 2);
					}
					else
					if ((flags & (1 << FLAG_BGR)) || (flags & (1 << FLAG_BGRA)))
					{
						if (flags & (1 << FLAG_BGRA))
							finalWii[0] = *(color + 3);
						else
							finalWii[0] = 0xff;

						finalWii[1] = *(color + 2);
						finalWii[32] = *(color + 1);
						finalWii[33] = *color;
					}


					finalWii += 2;
					color += colorSpace;
				}
				color += width * colorSpace - 16;
			}
			finalWii += 32;
		}
	}

	// Wrapping S
	int wrapS, wrapT;
	if (flags & (1 << FLAG_CLAMP_EDGE_S))
		wrapS = GX_CLAMP;
	else
	if (flags & (1 << FLAG_CLAMP_S))
		wrapS = GX_CLAMP;
	else
		wrapS = GX_REPEAT;

	// Wrapping T
	if (flags & (1 << FLAG_CLAMP_EDGE_T))
		wrapT = GX_CLAMP;
	else
	if (flags & (1 << FLAG_CLAMP_T))
		wrapT = GX_CLAMP;
	else
		wrapT = GX_REPEAT;

	GX_InitTexObj(gxImage, wiiData, w, h, GX_TF_RGBA8, wrapS, wrapT, GX_FALSE);
	GX_InitTexObjLOD(gxImage, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

	// Syncronization
	GX_InvalidateTexAll();

	texture* newTexture = new texture;
	if (!newTexture)
	{
		S_LOG_INFO("Failed to allocate new texture.");
		free(gxImage);

		return NULL;
	}

	resourceManager::getSingleton().addMemoryUse(4 * w * h);

	newTexture->push(gxImage, w, h);
	newTexture->push(wiiData);

	return newTexture;
}

}

#endif

