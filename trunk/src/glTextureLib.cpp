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

#ifndef __WII__
#include "textureLib.h"
#include "logger.h"
#include "root.h"

namespace k {

template<> textureLoader* singleton<textureLoader>::singleton_instance = 0;

textureLoader& textureLoader::getSingleton()
{  
	if (!singleton_instance)
		new textureLoader();

	assert(singleton_instance);
	return (*singleton_instance);  
}

void textureLoader::unLoadTexture(kTexture* tex)
{
	glDeleteTextures(1, tex);
}

kTexture* textureLoader::loadTexture(const char* file, unsigned short* w, unsigned short* h)
{
	FREE_IMAGE_FORMAT imgFormat = FreeImage_GetFileType(file, 0);
	if (imgFormat == FIF_UNKNOWN)
	{
		S_LOG_INFO("Unable to load texture " + std::string(file));
		return NULL;
	}

	FIBITMAP* image = FreeImage_Load(imgFormat, file, 0);
	assert(image);

	uint32_t width = FreeImage_GetWidth(image);
	uint32_t height = FreeImage_GetHeight(image);
	uint32_t pitch = FreeImage_GetPitch(image);
	uint32_t bpp = FreeImage_GetBPP(image);

	if (bpp != 32)
	{
		FIBITMAP* tmp = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(image);
		image = tmp;
	}

	char* imgData = (char*) malloc(width * height * 4);
	if (!imgData)
	{
		S_LOG_INFO("Could not allocate memory for image data.");
		return NULL;
	}

	// Copy Pixel data
	char* copyImgData = imgData;
	for (unsigned int j = (height-1); j > 0; j--)
	{
		for (unsigned int i = 0; i < width; i++)
		{
			RGBQUAD pixelColor;
			FreeImage_GetPixelColor(image, i, j, &pixelColor);

			copyImgData[0] = pixelColor.rgbRed;
			copyImgData[1] = pixelColor.rgbGreen;
			copyImgData[2] = pixelColor.rgbBlue;
			copyImgData[3] = pixelColor.rgbReserved;
			copyImgData += 4;
		}
	}

	FreeImage_Unload(image);

	kTexture* glImage = new kTexture;
	if (!glImage)
	{
		S_LOG_INFO("Failed to allocate gl image.");
		free(imgData);
		
		return NULL;
	}

	glGenTextures(1, glImage);
	glBindTexture(GL_TEXTURE_2D, *glImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	free(imgData);

	return glImage;

	/*
	 * Old DevIL code
	 *

	ILuint* newImage = new ILuint;
	if (!newImage)
		return NULL;

	ilGenImages(1, newImage);
	ilBindImage(newImage[0]);
	ilLoadImage((char*)file);

	if (ilGetError() == IL_NO_ERROR)
	{
		if (w && h)
		{
			*w = ilGetInteger(IL_IMAGE_WIDTH);
			*h = ilGetInteger(IL_IMAGE_HEIGHT);
		}

		ilBindImage(newImage[0]);
	
		kTexture* tex = new kTexture;
		if (tex)
		{
			*tex = ilutGLBindTexImage();

			// TODO: Make functions for that ;)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			ilDeleteImages(1, &newImage[0]);

			delete newImage;
			return tex;
		}
		else
		{
			ilDeleteImages(1, &newImage[0]);
			delete newImage;
		}
	}

	delete newImage;
	return NULL;
	*/
}

}

#endif

