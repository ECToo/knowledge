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
	#include "pngu.h"
}

#include "root.h"
#include "resourceManager.h"

namespace k {

void unloadTexture(const kTexture* tex) {}

typedef struct
{
	kTexture* tex;
	char* data; // must be aligned to 32 bytes
} wiiKTexture;

static inline wiiKTexture* loadTextureTPL(const std::string& file, unsigned short* w, unsigned short* h, int wrapBits)
{
	TPLFile* tpl;
	TPL_OpenTPLFromFile(tpl, file.c_str());
}

static inline wiiKTexture* loadTextureJPEG(const std::string& file, unsigned short* w, unsigned short* h, int wrapBits)
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

	char* textureData;
	try
	{
		textureData = new char[colorSpace * width * height];
	}

	catch (...)
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
		delete [] textureData;
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
	delete [] textureData;

	// New GXTexObj
	kTexture* newKTexture;
	try
	{
		newKTexture = new kTexture;	
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate GX texture object");
		free(wiiTexture);

		return NULL;
	}

	// Flush and set GX Object
	DCFlushRange(wiiTexture, width * height * 4);

	int envS, envT;
	if (wrapBits & (1 << FLAG_CLAMP_S) || wrapBits & (1 << FLAG_CLAMP_EDGE_S))
		envS = GX_CLAMP;
	else
		envS = GX_REPEAT;

	if (wrapBits & (1 << FLAG_CLAMP_T) || wrapBits & (1 << FLAG_CLAMP_EDGE_T))
		envT = GX_CLAMP;
	else
		envT = GX_REPEAT;

	GX_InitTexObj(newKTexture, wiiTexture, width, height, GX_TF_RGBA8, envS, envT, GX_FALSE);
	GX_InitTexObjLOD(newKTexture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

	// Syncronization
	GX_InvalidateTexAll();

	wiiKTexture* newWiiKtexture = (wiiKTexture*) memalign(32, sizeof(wiiKTexture));
	if (!newWiiKtexture)
	{
		S_LOG_INFO("Failed to allocate wiikTexture.");

		free(wiiTexture);
		delete [] newKTexture;

		return NULL;
	}

	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) rsc->addMemoryUse(4 * width * height);

	newWiiKtexture->tex = newKTexture;
	newWiiKtexture->data = wiiTexture;

	return newWiiKtexture;
}

static inline wiiKTexture* loadTexturePNG(const std::string& file, unsigned short* w, unsigned short* h, int wrapBits)
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

	kTexture* newKTexture;
	try
	{
		newKTexture = new kTexture;	
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate GX texture object");

		free(textureData);

		PNGU_ReleaseImageContext(textureCtx);
		return NULL;
	}
				
	PNGU_DecodeTo4x4RGBA8(textureCtx, imgProperties.imgWidth, imgProperties.imgHeight, textureData, 0xff);
	DCFlushRange(textureData, imgProperties.imgWidth * imgProperties.imgHeight * 4);

	int envS, envT;
	if (wrapBits & (1 << FLAG_CLAMP_S) || wrapBits & (1 << FLAG_CLAMP_EDGE_S))
		envS = GX_CLAMP;
	else
		envS = GX_REPEAT;

	if (wrapBits & (1 << FLAG_CLAMP_T) || wrapBits & (1 << FLAG_CLAMP_EDGE_T))
		envT = GX_CLAMP;
	else
		envT = GX_REPEAT;

	GX_InitTexObj(newKTexture, textureData, imgProperties.imgWidth, 
			imgProperties.imgHeight, GX_TF_RGBA8, envS, envT, GX_FALSE);
	GX_InitTexObjLOD(newKTexture, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

	// Syncronization
	PNGU_ReleaseImageContext(textureCtx);
	GX_InvalidateTexAll();

	wiiKTexture* newWiiKtexture;
	try
	{
		newWiiKtexture = new wiiKTexture;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate wiikTexture.");

		delete newKTexture;
		free(textureData);

		return NULL;
	}
		
	delete newKTexture;
	free(textureData);
	return NULL;

	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) rsc->addMemoryUse(4 * imgProperties.imgWidth * imgProperties.imgHeight);

	newWiiKtexture->tex = newKTexture;
	newWiiKtexture->data = textureData;

	return newWiiKtexture;
}

wiiKTexture* loadWiiTexture(const std::string& file, unsigned short* w, unsigned short* h, int wrapBits)
{
	std::string extension = getExtension(file);
	if (extension == ".tpl" || extension == ".TPL")
		return loadTextureTPL(file, w, h, wrapBits);
	else
	if (extension == ".png" || extension == ".PNG")
		return loadTexturePNG(file, w, h, wrapBits);
	else
	if (extension == ".jpg" || extension == ".JPG" || extension == ".jpeg" || extension == ".JPEG")
		return loadTextureJPEG(file, w, h, wrapBits);
	else
	{
		S_LOG_INFO("Uknown texture format for file " + file);
		return NULL;
	}
}

texture* loadTexture(const std::string& filename, int wrapBits)
{
	printf("wii filename: %s\n", filename.c_str());

	unsigned short width = 0;
	unsigned short height = 0;
	wiiKTexture* tex = loadWiiTexture(filename, &width, &height, wrapBits);
	if (!tex)
	{
		S_LOG_INFO("Failed to load texture " + filename);
		return NULL;
	}

	texture* newTexture;
	try
	{
		newTexture = new texture;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate memory for texture.");
		return NULL;
	}

	newTexture->push(tex->tex, width, height);
	newTexture->push(tex->data);
	newTexture->push(filename);

	// We dont need the structure anymore
	delete tex;

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

texture* loadCubemap(const std::string& filename, int wrapBits)
{
	texture* newTexture;

	try
	{
		newTexture = new texture;
	}

	catch (...)
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
		delete newTexture;

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
		delete newTexture;

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
		delete newTexture;

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
		delete newTexture;

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
		delete newTexture;

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
		delete newTexture;

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

texture* createRawTexture(unsigned char* data, int width, int height, int flags)
{
	kAssert(data);

	// RGBA
	const unsigned int colorSpace = 4;

	kTexture* gxImage;
	try
	{
		gxImage = new kTexture;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate GXTexObj for texture.");
		return NULL;
	}

	char* wiiData = (char*) memalign(32, 4 * width * height);
	if (!wiiData)
	{
		S_LOG_INFO("Failed to allocate aligned memory to raw texture.");
		delete gxImage;

		return NULL;
	}

	char* finalWii = wiiData;
	for (int y = 0; y < height; y += 4)
	{
		char* line = (char*)&data[y * height * colorSpace];
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

	GX_InitTexObj(gxImage, wiiData, width, height, GX_TF_RGBA8, wrapS, wrapT, GX_FALSE);
	GX_InitTexObjLOD(gxImage, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

	// Syncronization
	GX_InvalidateTexAll();

	texture* newTexture = (texture*) memalign(32, sizeof(texture));
	if (!newTexture)
	{
		S_LOG_INFO("Failed to allocate new texture.");
		delete gxImage;

		return NULL;
	}

	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) rsc->addMemoryUse(4 * width * height);

	newTexture->push(gxImage, width, height);
	newTexture->push(wiiData);

	return newTexture;
}

}

#endif

