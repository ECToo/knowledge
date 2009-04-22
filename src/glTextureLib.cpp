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

void unloadTexture(const kTexture* tex)
{
	kAssert(tex);
	glDeleteTextures(1, tex);
}

typedef struct
{
	kTexture* texture;
	unsigned int width;
	unsigned int height;
} freeImageTex_t;

freeImageTex_t* loadWithFreeImage(const std::string& filename, int wrapBits)
{
	freeImageTex_t* fiTexture;
	try
	{
		fiTexture = new freeImageTex_t;
	}
	catch (...)
	{
		S_LOG_INFO("Failed to allocate freeimage return structure.");
		return NULL;
	}

	// Check if file exists
	FILE* fileExist = fopen(filename.c_str(), "rb");
	if (!fileExist)
	{
		S_LOG_INFO("Failed to open file " + filename + ", it doesnt exist or you dont have permissions to access it.");

		delete fiTexture;

		return NULL;
	}
	fclose(fileExist);

	FREE_IMAGE_FORMAT imgFormat = FreeImage_GetFileType(filename.c_str(), 0);
	if (imgFormat == FIF_UNKNOWN)
	{
		S_LOG_INFO("Unable to load texture " + filename);

		delete fiTexture;

		return NULL;
	}

	int imgFlags = 0;
	if (imgFormat == FIF_JPEG)
		imgFlags = JPEG_ACCURATE;

	FIBITMAP* image = FreeImage_Load(imgFormat, filename.c_str(), imgFlags);
	if (!image)
	{
		S_LOG_INFO("Failed to load file with FreeImage.");

		delete fiTexture;

		return NULL;
	}

	fiTexture->width = FreeImage_GetWidth(image);
	fiTexture->height = FreeImage_GetHeight(image);
	unsigned short bpp = FreeImage_GetBPP(image);

	if (!fiTexture->width || !fiTexture->height)
	{
		std::stringstream tempStr;
		tempStr << "Invalid texture size (" << filename;
		tempStr << ", " << fiTexture->width << "x" << fiTexture->height << ")";

		S_LOG_INFO(tempStr.str());
		return NULL;
	}

	if (bpp != 32 && bpp != 24)
	{
		FIBITMAP* tmp = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(image);
		image = tmp;
	}

	unsigned char* imgData;
	try 
	{
		imgData = new unsigned char[fiTexture->width * fiTexture->height * 4];
	}

	catch (...)
	{
		S_LOG_INFO("Could not allocate memory for image data.");
		FreeImage_Unload(image);

		delete fiTexture;

		return NULL;
	}

	// Copy Pixel data
	unsigned char* copyImgData = imgData;
	for (int j = (fiTexture->height - 1); j >= 0; j--)
	{
		for (unsigned int i = 0; i < fiTexture->width; i++)
		{
			RGBQUAD pixelColor;
			FreeImage_GetPixelColor(image, i, j, &pixelColor);

			copyImgData[2] = pixelColor.rgbRed;
			copyImgData[1] = pixelColor.rgbGreen;
			copyImgData[0] = pixelColor.rgbBlue;
			copyImgData[3] = pixelColor.rgbReserved;
			copyImgData += 4;
		}
	}

	FreeImage_Unload(image);

	kTexture* glImage;
	
	try
	{
		glImage = new kTexture;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate GLuint for image.");
		delete [] imgData;
		delete fiTexture;

		return NULL;
	}
		
	glGenTextures(1, glImage);
	glBindTexture(GL_TEXTURE_2D, *glImage);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fiTexture->width, fiTexture->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, imgData);

	// Wrapping S
	if (wrapBits & (1 << FLAG_CLAMP_EDGE_S))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	else
	if (wrapBits & (1 << FLAG_CLAMP_S))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// Wrapping T
	if (wrapBits & (1 << FLAG_CLAMP_EDGE_T))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	else
	if (wrapBits & (1 << FLAG_CLAMP_T))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Wrapping R
	if (wrapBits & (1 << FLAG_CLAMP_EDGE_R))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	else
	if (wrapBits & (1 << FLAG_CLAMP_R))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	delete [] imgData;

	fiTexture->texture = glImage;
	return fiTexture;
}

texture* loadTexture(const std::string& filename, int wrapBits)
{
	freeImageTex_t* tex = loadWithFreeImage(filename, wrapBits);
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

	newTexture->push(tex->texture, tex->width, tex->height);
	newTexture->push(filename);

	// We dont need freeimage temp struct anymore
	delete tex;

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
	freeImageTex_t* tempTex = NULL;
		
	// Front
	tempName = newName + "_front" + extension;	
	tempTex = loadWithFreeImage(tempName, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex->texture, 0, 0);

	delete tempTex;

	// Back
	tempName = newName + "_back" + extension;	
	tempTex = loadWithFreeImage(tempName, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex->texture, 0, 0);

	delete tempTex;

	// Left 
	tempName = newName + "_left" + extension;	
	tempTex = loadWithFreeImage(tempName, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex->texture, 0, 0);

	delete tempTex;

	// Right
	tempName = newName + "_right" + extension;	
	tempTex = loadWithFreeImage(tempName, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex->texture, 0, 0);

	delete tempTex;

	// Up
	tempName = newName + "_up" + extension;	
	tempTex = loadWithFreeImage(tempName, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex->texture, 0, 0);

	delete tempTex;

	// Down 
	tempName = newName + "_down" + extension;	
	tempTex = loadWithFreeImage(tempName, wrapBits);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}

	newTexture->push(tempTex->texture, tempTex->width, tempTex->height);
	newTexture->push(filename);

	if (!isPowerOfTwo(tempTex->width) || !isPowerOfTwo(tempTex->height))
	{
		std::stringstream warn;
		warn << "WARNING! The texture " << filename << " dimensions(";
		warn << tempTex->width << "," << tempTex->height;
		warn << ") are not power of 2";

		S_LOG_INFO(warn.str());
	}

	delete tempTex;
	return newTexture;
}

texture* createRawTexture(unsigned char* data, int w, int h, int flags)
{
	kAssert(data);
	kTexture* glImage;
	texture* newTexture;

	try
	{
		glImage = new kTexture;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate GLuint for texture.");
		return NULL;
	}

	glGenTextures(1, glImage);
	glBindTexture(GL_TEXTURE_2D, *glImage);

	GLuint format = GL_RGB;
	if (flags & (1 << FLAG_RGBA))
		format = GL_RGBA;
	else
	if (flags & (1 << FLAG_RGB))
		format = GL_RGB;
	else
	if (flags & (1 << FLAG_BGR))
		format = GL_BGR;
	else
	if (flags & (1 << FLAG_BGRA))
		format = GL_BGRA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, 
			format, GL_UNSIGNED_BYTE, data);

	// Wrapping S
	if (flags & (1 << FLAG_CLAMP_EDGE_S))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	else
	if (flags & (1 << FLAG_CLAMP_S))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// Wrapping T
	if (flags & (1 << FLAG_CLAMP_EDGE_T))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	else
	if (flags & (1 << FLAG_CLAMP_T))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Wrapping R
	if (flags & (1 << FLAG_CLAMP_EDGE_R))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	else
	if (flags & (1 << FLAG_CLAMP_R))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	try
	{
		newTexture = new texture;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate new texture.");
		delete glImage;

		return NULL;
	}

	newTexture->push(glImage, w, h);
	return newTexture;
}

}

#endif

