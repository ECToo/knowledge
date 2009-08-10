/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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

// Jpeg is not C++ safe
extern "C" 
{
	#include <stdio.h>
	#include <stdlib.h>
	#include <jpeglib.h>
	#include <jerror.h>
	#include "pngu.h"
}

#include "texture.h"
#include "root.h"
#include "logger.h"
#include "tev.h"
#include "resourceManager.h"

// Those came from libogc tpl.c
// texture header
typedef struct _tplimgheader TPLImgHeader;

struct _tplimgheader 
{
	u16 height;
	u16 width;
	u32 fmt;
	void *data;
	u32 wraps;
	u32 wrapt;
	u32 minfilter;
	u32 magfilter;
	f32 lodbias;
	u8 edgelod;
	u8 minlod;
	u8 maxlod;
	u8 unpacked;
} ATTRIBUTE_PACKED;

// texture palette header
typedef struct _tplpalheader TPLPalHeader;

struct _tplpalheader 
{
	u16 nitems;
	u8 unpacked;
	u8 pad;
	u32 fmt;
	void *data;
} ATTRIBUTE_PACKED;

// texture descriptor
typedef struct _tpldesc TPLDescHeader;

struct _tpldesc 
{
	TPLImgHeader *imghead;
	TPLPalHeader *palhead;
} ATTRIBUTE_PACKED;

namespace k {
			
texture::texture(const std::string& filename, int flags)
{
	// Clean Fields
	mFormat = 0;
	mRawData = NULL;
	mPointer = NULL;

	// Check if file exists
	FILE* fileExist = fopen(filename.c_str(), "rb");

	if (!fileExist)
	{
		S_LOG_INFO("Failed to open file " + filename + ", it doesnt exist or you dont have permissions to access it.");
		return;
	}

	fclose(fileExist);

	// Set Flags
	mFlags = flags;

	// Discover wich texture type it is
	std::string extension = getExtension(filename);
	if (extension == ".tpl" || extension == ".TPL")
	{
		try
		{
			mFormat = TEX_WII_TPL;
			mPointer = new platformTexturePointer;
		}
		catch (...)
		{
			S_LOG_INFO("Failed to allocate TPL texture pointer.");
			return;
		}

		try
		{
			mRawData = (void*) new TPLFile;
			TPL_OpenTPLFromFile((TPLFile*)mRawData, filename.c_str());

			// Fill in details here
			TPLDescHeader* tplHeader = (TPLDescHeader*)((TPLFile*)(mRawData))->texdesc;
			if (tplHeader)
			{
				mWidth = tplHeader[0].imghead->width;
				mHeight = tplHeader[0].imghead->height;
			}

			TPL_GetTexture((TPLFile*)mRawData, 0, mPointer);
		}

		catch (...)
		{
			delete mPointer;

			S_LOG_INFO("Failed to allocate TPLFile texture pointer.");
			return;
		}
	}
	else
	if (extension == ".jpg" || extension == ".JPG" || extension == ".jpeg" || extension == ".JPEG")
	{
		try
		{
			mPointer = new platformTexturePointer;
		}

		catch (...)
		{
			S_LOG_INFO("Failed to allocate JPEG texture pointer.");
			return;
		}

		struct jpeg_decompress_struct jInfo;
		struct jpeg_error_mgr jError;

		jInfo.err = jpeg_std_error(&jError);
		jpeg_create_decompress(&jInfo);

		FILE* texFile = fopen(filename.c_str(), "rb");
		if (!texFile)
		{
			delete mPointer;

			S_LOG_INFO("Failed to open file " + filename + ", it doesnt exist or you dont have permissions to access it.");
			return;
		}

		jpeg_stdio_src(&jInfo, texFile);
		jpeg_read_header(&jInfo, true);

		mWidth = jInfo.image_width;
		mHeight = jInfo.image_height;
		mFormat = TEX_RGBA;
		
		unsigned int colorSpace = jInfo.jpeg_color_space;

		char* tempData;
		try
		{
			tempData = new char[colorSpace * mWidth * mHeight];
		}

		catch (...)
		{
			fclose(texFile);
			delete mPointer;

			S_LOG_INFO("Failed to allocate JPEG texture data for " + filename);
			return;
		}

		// Just a pointer
		char* textureMemory = tempData;

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
		mRawData = (char*) memalign(32, 4 * mWidth * mHeight);
		if (!mRawData)
		{
			S_LOG_INFO("Failed to allocate JPEG aligned memory for " + filename);

			delete mPointer;
			delete [] tempData;

			return;
		}

		// Copy from jpeg source and free 
		char* finalWii = (char*)mRawData;
		for (int y = 0; y < mHeight; y += 4)
		{
			char* line = &tempData[y * mHeight * colorSpace];
			for (int x = 0; x < mWidth; x += 4)
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
					color += mWidth * colorSpace - 16;
				}
				finalWii += 32;
			}
		}

		// We dont need raw jpeg data anymore
		delete [] tempData;

		// Flush and set GX Object
		DCFlushRange(mRawData, mWidth * mHeight * 4);

		int envS, envT;
		if (mFlags & (1 << FLAG_CLAMP_S) || mFlags & (1 << FLAG_CLAMP_EDGE_S))
			envS = GX_CLAMP;
		else
			envS = GX_REPEAT;

		if (mFlags & (1 << FLAG_CLAMP_T) || mFlags & (1 << FLAG_CLAMP_EDGE_T))
			envT = GX_CLAMP;
		else
			envT = GX_REPEAT;

		GX_InitTexObj(mPointer, mRawData, mWidth, mHeight, GX_TF_RGBA8, envS, envT, GX_FALSE);
		GX_InitTexObjLOD(mPointer, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

		// Syncronization
		GX_InvalidateTexAll();

		resourceManager* rsc = &resourceManager::getSingleton();
		if (rsc) rsc->addMemoryUse(4 * mWidth * mHeight);
	}
	else
	if (extension == ".png" || extension == ".PNG")
	{
		try
		{
			mPointer = new platformTexturePointer;
		}

		catch (...)
		{
			S_LOG_INFO("Failed to allocate JPEG texture pointer.");
			return;
		}

		IMGCTX textureCtx = PNGU_SelectImageFromDevice(filename.c_str());
		if (!textureCtx)
		{
			S_LOG_INFO("Failed to retrieve PNG information.");
			return;
		}
		
		PNGUPROP imgProperties;
		PNGU_GetImageProperties(textureCtx, &imgProperties);

		mWidth = imgProperties.imgWidth;
		mHeight = imgProperties.imgHeight;
		mFormat = TEX_RGBA;

		mRawData = (char*) memalign(32, 4 * mWidth * mHeight);
		if (!mRawData)
		{
			std::stringstream dbgTex;
			dbgTex << "Failed to allocate data (";
			dbgTex << 4 * mWidth * mHeight << "for texture";

			S_LOG_INFO(dbgTex.str());
			PNGU_ReleaseImageContext(textureCtx);
		
			delete mPointer;
			return;
		}
				
		PNGU_DecodeTo4x4RGBA8(textureCtx, mWidth, mHeight, mRawData, 0xff);
		DCFlushRange(mRawData, mWidth * mHeight * 4);

		int envS, envT;
		if (mFlags & (1 << FLAG_CLAMP_S) || mFlags & (1 << FLAG_CLAMP_EDGE_S))
			envS = GX_CLAMP;
		else
			envS = GX_REPEAT;

		if (mFlags & (1 << FLAG_CLAMP_T) || mFlags & (1 << FLAG_CLAMP_EDGE_T))
			envT = GX_CLAMP;
		else
			envT = GX_REPEAT;

		GX_InitTexObj(mPointer, mRawData, mWidth, mHeight, GX_TF_RGBA8, envS, envT, GX_FALSE);
		GX_InitTexObjLOD(mPointer, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

		// Syncronization
		PNGU_ReleaseImageContext(textureCtx);
		GX_InvalidateTexAll();

		resourceManager* rsc = &resourceManager::getSingleton();
		if (rsc) rsc->addMemoryUse(4 * mWidth * mHeight);
	}
	else
	{
		S_LOG_INFO("Texture Format not supported, be sure to check your file extensions in case you believe its either a jpg, png or tpl.");
		kAssert(0);
	}
}

texture::texture(void* data, unsigned int w, unsigned int h, int flags, int format)
{
	kAssert(data);

	// TODO: Support more than RGBA
	const unsigned int colorSpace = 4;
	mWidth = w;
	mHeight = h;
	mFormat = TEX_RGBA;
	mFlags = flags;

	try
	{
		mPointer = new platformTexturePointer;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate [user data] texture pointer.");
		return;
	}

	mRawData = (char*) memalign(32, 4 * mWidth * mHeight);
	if (!mRawData)
	{
		delete mPointer;

		S_LOG_INFO("Failed to allocate aligned memory to raw texture.");
		return;
	}

	char* finalWii = (char*)mRawData;
	for (int y = 0; y < mHeight; y += 4)
	{
		char* line = &((char*)data)[y * mHeight * colorSpace];
		for (int x = 0; x < mWidth; x += 4)
		{
			char* color = line + (x * colorSpace);
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 4; tx++)
				{
					if (mFormat == TEX_RGB || mFormat == TEX_RGBA)
					{
						if (mFormat == TEX_RGBA)
							finalWii[0] = *(color + 3);
						else
							finalWii[0] = 0xff;

						finalWii[1] = *color;
						finalWii[32] = *(color + 1);
						finalWii[33] = *(color + 2);
					}
					else
					if (mFormat == TEX_RGB || mFormat == TEX_RGBA)
					{
						if (mFormat == TEX_RGBA)
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
				color += mWidth * colorSpace - 16;
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

	GX_InitTexObj(mPointer, mRawData, mWidth, mHeight, GX_TF_RGBA8, wrapS, wrapT, GX_FALSE);
	GX_InitTexObjLOD(mPointer, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);

	// Syncronization
	GX_InvalidateTexAll();

	resourceManager* rsc = &resourceManager::getSingleton();
	if (rsc) rsc->addMemoryUse(4 * mWidth * mHeight);
}

texture::~texture()
{
	switch (mFormat)
	{
		case TEX_WII_TPL:
			TPL_CloseTPLFile((TPLFile*)mRawData);
			delete (TPLFile*)mRawData;
			break;
		default:
			if (mRawData)
				free(mRawData);
			break;
	}

	delete mPointer;
}

/*
wiiTexture::wiiTexture(unsigned short index) : textureStage(index)
{
	guMtxIdentity(mTransRotate);
	mProgram.clear();
}
			
void wiiTexture::setTexCoordGen()
{
	switch (mTexCoordType)
	{
		default:
			S_LOG_INFO("Undefined texture coordinate type, fallbacking to UV.");
		case TEXCOORD_UV:
			if (mScroll.x || mScroll.y || mRotate || mScale.x || mScale.y)
			{
				kAssert(mIndex < 8);

				vec_t sinAngle = sin(mAngle);
				vec_t cosAngle = cos(mAngle);
				mAngle += mRotate * M_PI / 180.0f;

				mScrolled.x += mScroll.x;
				mScrolled.y += mScroll.y;

				mTransRotate[0][0] = cosAngle * mScale.x;
				mTransRotate[0][1] = -sinAngle;
				mTransRotate[0][2] = 0;

				mTransRotate[1][0] = sinAngle;
				mTransRotate[1][1] = cosAngle * mScale.y;
				mTransRotate[1][2] = 0;

				mTransRotate[2][0] = 0;
				mTransRotate[2][1] = 0;
				mTransRotate[2][2] = 1;
				
				mTransRotate[0][3] = 0.5 - 0.5 * cosAngle + 0.5 * sinAngle + mScrolled.x;
				mTransRotate[1][3] = 0.5 - 0.5 * sinAngle - 0.5 * cosAngle + mScrolled.y;
				mTransRotate[2][3] = 0;

				GX_LoadTexMtxImm(mTransRotate, GX_TEXMTX0 + mIndex * 3, GX_TG_MTX2x4);
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_TEX0 + mIndex, GX_TEXMTX0 + mIndex * 3);
			}
			else 
			{
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_TEX0 + mIndex, GX_IDENTITY);
			}
			break;

		case TEXCOORD_NORMAL:
			{
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_NRM, GX_IDENTITY);
			}
			break;

		case TEXCOORD_POS:
			{
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_POS, GX_IDENTITY);
			}
			break;

		case TEXCOORD_BINORMAL:
			{
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_BINRM, GX_IDENTITY);
			}
			break;

		case TEXCOORD_TANGENT:
			{
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_TANGENT, GX_IDENTITY);
			}
			break;

		case TEXCOORD_SPHERE:
			{
				renderSystem* rs = root::getSingleton().getRenderSystem();

				Mtx s, t, destMtx;
				Mtx postMtx;

				rs->getModelView(postMtx);

				guMtxScale(s, 0.5f, -0.5f, 0.0f);
				guMtxTrans(t, 0.5f, 0.5f, 1.0f);
				guMtxConcat(s, postMtx, postMtx);
				guMtxConcat(t, postMtx, postMtx);

				guMtxIdentity(destMtx);

				GX_LoadTexMtxImm(postMtx, GX_TEXMTX0 + mIndex * 3, GX_TG_MTX3x4);
				GX_LoadTexMtxImm(destMtx, GX_DTTMTX0 + mIndex * 3, GX_TG_MTX3x4);

				GX_SetTexCoordGen2(GX_TEXCOORD0 + mIndex, GX_TG_MTX3x4, GX_TG_NRM, 
						GX_TEXMTX0 + mIndex * 3, GX_TRUE, GX_DTTMTX0 + mIndex * 3);
			}
			break;
	}
}

void wiiTexture::draw()
{
	if (!mTexture)
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();

	// Texture Coordinate
	setTexCoordGen();

	// TEV
	tev* thisTev = tevManager::getSingleton().getTev(mProgram);
	if (!mProgram.length() || !thisTev || mProgram == "replace")
	{
		GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_REPLACE);
	}
	else
	if (mProgram == "decal")
	{
		GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_DECAL);
	}
	else
	if (mProgram == "blend")
	{
		GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_BLEND);
	}
	else
	if (mProgram == "modulate")
	{
		GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_MODULATE);
	}
	else
	{
		thisTev->setup(mIndex);
	}

	if (mBlendSrc || mBlendDst)
	{
		rs->setBlend(true);
		rs->setBlendMode(mBlendSrc, mBlendDst);
		rs->setDepthMask(false);
	}
	else
	{
		rs->setBlend(false);
		rs->setDepthMask(true);
	}

	rs->bindTexture(getTexture(0), mIndex);
}

void wiiTexture::finish() 
{
}
*/

}

#endif

