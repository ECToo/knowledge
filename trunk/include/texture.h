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

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "prerequisites.h"
#include "vector2.h"
#include "logger.h"

namespace k
{
	/**
	 * Texture coordinate types.
	 */
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

	/**
	 * Ordering of cube textures.
	 */
	enum CubeTextureOrdering
	{
		CUBE_FRONT,
		CUBE_BACK,
		CUBE_LEFT,
		CUBE_RIGHT,
		CUBE_UP,
		CUBE_DOWN
	};

	/**
	 * Texture flags
	 */
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

	/**
	 * Texture formats
	 */
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

	/**
	 * \brief Holds a hardware texture or a collection of textures related to each other.
	 */
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
			 * Set the texture flags, in case you want to relate the texture
			 * with special effects or any other stuff you want ;)
			 */
			void setFlags(unsigned int flags);

			/**
			 * Returns the texture flags.
			 */
			const unsigned int getFlags() const
			{ return mFlags; }

			/**
			 * Returns the texture pointer.
			 */
			platformTexturePointer* getPointer() const
			{ return mPointer; }

			/**
			 * Returns raw texture data (pixel data).
			 */
			void* getRaw() const
			{ return mRawData; }

			/**
			 * Returns the texture filename.
			 */
			const std::string& getFilename() const
			{ return mFilename; }

			/**
			 * Check if the texture contains data from the filename.
			 */
			const bool containsFilename(const std::string& filename) const
			{
				if (mFilename.find(filename) != std::string::npos)
					return true;
				else
					return false;
			}

			/**
			 * Return texture format. @see TextureFormats
			 */
			unsigned int getFormat() const
			{ return mFormat; }

			/**
			 * Return texture width.
			 */
			unsigned int getWidth() const
			{ return mWidth; }

			/**
			 * Return texture height.
			 */
			unsigned int getHeight() const
			{ return mHeight; }
	};

}

#endif

