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

#include "textureManager.h"
#include "logger.h"
#include "root.h"

namespace k {

void unloadTexture(kTexture* tex)
{
	kAssert(tex);
	glDeleteTextures(1, tex);
}

kTexture* loadWithFreeImage(const std::string& filename, unsigned short* w, unsigned short* h, int wrapBits)
{
	FREE_IMAGE_FORMAT imgFormat = FreeImage_GetFileType(filename.c_str(), 0);
	if (imgFormat == FIF_UNKNOWN)
	{
		S_LOG_INFO("Unable to load texture " + filename);
		return NULL;
	}

	FIBITMAP* image = FreeImage_Load(imgFormat, filename.c_str(), 0);
	if (!image)
	{
		S_LOG_INFO("Failed to load file with FreeImage.");
		return NULL;
	}

	unsigned short width = FreeImage_GetWidth(image);
	unsigned short height = FreeImage_GetHeight(image);
	unsigned short bpp = FreeImage_GetBPP(image);

	if (bpp != 32)
	{
		FIBITMAP* tmp = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(image);
		image = tmp;
	}

	unsigned char* imgData = (unsigned char*) malloc(width * height * 4);
	if (!imgData)
	{
		S_LOG_INFO("Could not allocate memory for image data.");
		FreeImage_Unload(image);

		return NULL;
	}

	// Copy Pixel data
	unsigned char* copyImgData = imgData;
	for (int j = (height - 1); j >= 0; j--)
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

	kTexture* glImage = (kTexture*) malloc(sizeof(kTexture));
	if (!glImage)
	{
		S_LOG_INFO("Failed to allocate GLuint for image.");
		free(imgData);

		return NULL;
	}
		
	glGenTextures(1, glImage);
	glBindTexture(GL_TEXTURE_2D, *glImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);

	// Wrapping S
	if (wrapBits & FLAG_CLAMP_EDGE_S)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	else
	if (wrapBits & FLAG_CLAMP_S)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// Wrapping T
	if (wrapBits & FLAG_CLAMP_EDGE_T)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	else
	if (wrapBits & FLAG_CLAMP_T)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Wrapping R
	if (wrapBits & FLAG_CLAMP_EDGE_R)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	else
	if (wrapBits & FLAG_CLAMP_R)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	free(imgData);

	if (w && h)
	{
		*w = width;
		*h = height;
	}

	return glImage;
}

texture* loadTexture(const std::string& filename, int wrapBits)
{
	unsigned short width, height;
	kTexture* tex = loadWithFreeImage(filename, &width, &height, wrapBits);
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

	newTexture->push(tex, width, height);
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
	kTexture* tempTex = NULL;
		
	// Front
	tempName = newName + "_front" + extension;	
	tempTex = loadWithFreeImage(tempName, NULL, NULL, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex, 0, 0);

	// Back
	tempName = newName + "_back" + extension;	
	tempTex = loadWithFreeImage(tempName, NULL, NULL, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex, 0, 0);

	// Left 
	tempName = newName + "_left" + extension;	
	tempTex = loadWithFreeImage(tempName, NULL, NULL, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex, 0, 0);

	// Right
	tempName = newName + "_right" + extension;	
	tempTex = loadWithFreeImage(tempName, NULL, NULL, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex, 0, 0);

	// Up
	tempName = newName + "_up" + extension;	
	tempTex = loadWithFreeImage(tempName, NULL, NULL, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex, 0, 0);

	// Final Size
	unsigned short width = 0;
	unsigned short height = 0;

	// Down 
	tempName = newName + "_down" + extension;	
	tempTex = loadWithFreeImage(tempName, &width, &height, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}

	newTexture->push(tempTex, width, height);
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

	kTexture* glImage = (kTexture*) malloc(sizeof(kTexture));
	if (!glImage)
	{
		S_LOG_INFO("Failed to allocate GLuint for texture.");
		return NULL;
	}

	glGenTextures(1, glImage);
	glBindTexture(GL_TEXTURE_2D, *glImage);

	GLuint format = GL_RGB;
	if (flags & FLAG_RGBA)
		format = GL_RGBA;
	else
	if (flags & FLAG_RGB)
		format = GL_RGB;
	else
	if (flags & FLAG_BGR)
		format = GL_BGR;
	else
	if (flags & FLAG_BGRA)
		format = GL_BGRA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, 
			format, GL_UNSIGNED_BYTE, data);

	// Wrapping S
	if (flags & FLAG_CLAMP_EDGE_S)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	else
	if (flags & FLAG_CLAMP_S)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// Wrapping T
	if (flags & FLAG_CLAMP_EDGE_T)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	else
	if (flags & FLAG_CLAMP_T)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Wrapping R
	if (flags & FLAG_CLAMP_EDGE_R)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	else
	if (flags & FLAG_CLAMP_R)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	texture* newTexture = new texture;
	if (!newTexture)
	{
		S_LOG_INFO("Failed to allocate new texture.");
		free(glImage);

		return NULL;
	}

	newTexture->push(glImage, w, h);
	return newTexture;
}

}

#endif

