/*
Copyright (c) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "texture.h"
#include "root.h"
#include "logger.h"
	
// FreeImage
#include <FreeImage.h>

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
	if (imgFormat == FIF_PNG)
		imgFlags |= PNG_IGNOREGAMMA;

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
		delete [] (char*) mRawData;
		return;
	}
		
	glEnable(GL_TEXTURE_2D);
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

	delete [] (char*) mRawData;
	mRawData = NULL;
}
			
texture::texture(void* data, unsigned int w, unsigned int h, int flags, int format)
{
	mWidth = w;
	mHeight = h;
	mFormat = format;
	mFlags = flags;
	mRawData = NULL;

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
		
	glEnable(GL_TEXTURE_2D);
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

