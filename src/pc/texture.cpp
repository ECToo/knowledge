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

#include "texture.h"
#include "root.h"
#include "logger.h"

namespace k {
			
texture::texture(const std::string& filename, int flags)
{
	// Check if file exists
	FILE* fileExist = fopen(filename.c_str(), "rb");

	if (!fileExist)
	{
		S_LOG_INFO("Failed to open file " + filename + ", it doesnt exist or you dont have permissions to access it.");
		return;
	}

	fclose(fileExist);

	FREE_IMAGE_FORMAT imgFormat = FreeImage_GetFileType(filename.c_str(), 0);
	if (imgFormat == FIF_UNKNOWN)
	{
		S_LOG_INFO("FreeImage was unable to load texture " + filename);
		return;
	}

	int imgFlags = 0;
	if (imgFormat == FIF_JPEG)
		imgFlags = JPEG_ACCURATE;

	FIBITMAP* image = FreeImage_Load(imgFormat, filename.c_str(), imgFlags);
	if (!image)
	{
		S_LOG_INFO("FreeImage was unable to load texture " + filename);
		return;
	}

	mWidth = FreeImage_GetWidth(image);
	mHeight = FreeImage_GetHeight(image);
	mFormat = TEX_RGBA;
	mFlags = flags;

	unsigned int bpp = FreeImage_GetBPP(image);

	if (!mWidth || !mHeight)
	{
		std::stringstream tempStr;
		tempStr << "Invalid texture size (" << filename;
		tempStr << ", " << mWidth << "x" << mHeight << ")";

		S_LOG_INFO(tempStr.str());
		return;
	}

	if (bpp != 32 && bpp != 24)
	{
		FIBITMAP* tmp = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(image);
		image = tmp;
	}

	mRawData = NULL;
	try 
	{
		mRawData = new unsigned char[mWidth * mHeight * 4];
	}

	catch (...)
	{
		S_LOG_INFO("Could not allocate memory for texture (" + filename + ") data.");
		FreeImage_Unload(image);
		return;
	}

	// Copy Pixel data
	unsigned char* copyImgData = (unsigned char*)mRawData;
	for (int j = (mHeight - 1); j >= 0; j--)
	{
		for (unsigned int i = 0; i < mWidth; i++)
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

	try
	{
		mPointer = new platformTexturePointer;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate GLuint texture for " + filename);
		delete [] mRawData;
		return;
	}
		
	glGenTextures(1, mPointer);
	glBindTexture(GL_TEXTURE_2D, *mPointer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, mRawData);

	// Wrapping S
	if (mFlags & (1 << FLAG_CLAMP_EDGE_S))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	else
	if (mFlags & (1 << FLAG_CLAMP_S))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// Wrapping T
	if (mFlags & (1 << FLAG_CLAMP_EDGE_T))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	else
	if (mFlags & (1 << FLAG_CLAMP_T))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Wrapping R
	if (mFlags & (1 << FLAG_CLAMP_EDGE_R))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	else
	if (mFlags & (1 << FLAG_CLAMP_R))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	delete [] mRawData;
	mRawData = NULL;
}
			
texture::texture(void* data, unsigned int w, unsigned int h, int flags, int format)
{
	mWidth = w;
	mHeight = h;
	mFormat = format;
	mFlags = flags;

	try
	{
		mPointer = new platformTexturePointer;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate GLuint texture for dynamic data.");
		return;
	}

	int realFormat = GL_RGBA;
	switch (mFormat)
	{
		case TEX_RGB:
			realFormat = GL_RGB;
			break;
		case TEX_RGBA:
			realFormat = GL_RGBA;
			break;
		case TEX_BGR:
			realFormat = GL_BGR;
			break;
		case TEX_BGRA:
			realFormat = GL_BGRA;
			break;
		default:
			S_LOG_INFO("Texture Format not supported by openGL");
			return;
	}
		
	glGenTextures(1, mPointer);
	glBindTexture(GL_TEXTURE_2D, *mPointer);
	glTexImage2D(GL_TEXTURE_2D, 0, realFormat, mWidth, mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

	// Wrapping S
	if (mFlags & (1 << FLAG_CLAMP_EDGE_S))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	else
	if (mFlags & (1 << FLAG_CLAMP_S))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// Wrapping T
	if (mFlags & (1 << FLAG_CLAMP_EDGE_T))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	else
	if (mFlags & (1 << FLAG_CLAMP_T))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Wrapping R
	if (mFlags & (1 << FLAG_CLAMP_EDGE_R))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	else
	if (mFlags & (1 << FLAG_CLAMP_R))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	mRawData = NULL;
}

texture::~texture()
{
	if (mPointer)
		delete mPointer;
}

void texture::setFlags(unsigned int flags)
{
	if (!mPointer)
		return;

	mFlags = flags;
	glBindTexture(GL_TEXTURE_2D, *mPointer);

	// Wrapping S
	if (mFlags & (1 << FLAG_CLAMP_EDGE_S))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	else
	if (mFlags & (1 << FLAG_CLAMP_S))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	// Wrapping T
	if (mFlags & (1 << FLAG_CLAMP_EDGE_T))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	else
	if (mFlags & (1 << FLAG_CLAMP_T))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Wrapping R
	if (mFlags & (1 << FLAG_CLAMP_EDGE_R))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	else
	if (mFlags & (1 << FLAG_CLAMP_R))
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
}
			
}

