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

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "prerequisites.h"
#include "vector2.h"
#include "logger.h"

namespace k
{
	enum TextureCoordType 
	{
		TEXCOORD_NONE,
		TEXCOORD_UV,
		TEXCOORD_EYE_LINEAR,
		TEXCOORD_SPHERE,
		TEXCOORD_CUBEMAP,
		TEXCOORD_POS,
		TEXCOORD_NORMAL,
		TEXCOORD_BINORMAL,
		TEXCOORD_TANGENT
	};

	enum CubeTextureOrdering
	{
		CUBE_FRONT,
		CUBE_BACK,
		CUBE_LEFT,
		CUBE_RIGHT,
		CUBE_UP,
		CUBE_DOWN
	};

	enum TextureFlags
	{
		FLAG_REPEAT_S = 0,
		FLAG_REPEAT_T,
		FLAG_REPEAT_R,
		FLAG_CLAMP_EDGE_S,
		FLAG_CLAMP_EDGE_T,
		FLAG_CLAMP_EDGE_R,
		FLAG_CLAMP_S,
		FLAG_CLAMP_T,
		FLAG_CLAMP_R,
	};

	enum TextureFormats
	{
		TEX_NONE,

		TEX_RGB,
		TEX_RGBA,

		TEX_BGR,
		TEX_BGRA,

		TEX_WII_TPL,
		TEX_WII_RGBA8,

		TEX_MAX_FORMATS
	};

	class DLL_EXPORT texture
	{
		protected:
			unsigned int mWidth;
			unsigned int mHeight;
			unsigned int mFormat;
			unsigned int mFlags;

			/**
			 * Platform specific texture pointer, used
			 * by the render library to bind textures.
			 */
			platformTexturePointer* mPointer;

			/**
			 * Data allocated by texture library 
			 * or read from file.
			 */
			void* mRawData;

			/**
			 * Name of texture file. If this is a cubic
			 * texture.
			 */
			std::string mFilename;

		public:
			/**
			 * On each platform, it will load the right texture file and 
			 * set its flags according.
			 */
			texture(const std::string& filename, int flags);

			/**
			 * Start a texture made from the render system.
			 */
			texture(platformTexturePointer* ptr, unsigned int w, unsigned int h, int format)
			{
				kAssert(ptr);

				mPointer = ptr;
				mWidth = w;
				mHeight = h;
				mFormat = format;
			}

			/**
			 * Start a texture with data set already.
			 */
			texture(void* data, unsigned int w, unsigned int h, int flags, int format);

			/**
			 * Must free pointers and raw data.
			 */
			~texture();

			/**
			 * Needs platform specific implementations.
			 */
			void setFlags(unsigned int flags);

			const unsigned int getFlags() const
			{ return mFlags; }

			platformTexturePointer* getPointer() const
			{ return mPointer; }

			void* getRaw() const
			{ return mRawData; }

			const std::string& getFilename() const
			{ return mFilename; }

			const bool containsFilename(const std::string& filename) const
			{
				if (mFilename.find(filename) != std::string::npos)
					return true;
				else
					return false;
			}

			unsigned int getFormat() const
			{ return mFormat; }

			unsigned int getWidth() const
			{ return mWidth; }

			unsigned int getHeight() const
			{ return mHeight; }
	};

}

#endif

