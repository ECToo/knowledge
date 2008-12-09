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

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "prerequisites.h"
#include "vector2.h"

namespace k
{
	enum texCoordType 
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

	enum cubeTexOrdering
	{
		CUBE_FRONT,
		CUBE_BACK,
		CUBE_LEFT,
		CUBE_RIGHT,
		CUBE_UP,
		CUBE_DOWN
	};

	class texture
	{
		public:
			texture()
			{
				mWidth = mHeight = 0;
				mId.clear();
			}

			~texture();

			unsigned short mWidth;
			unsigned short mHeight;
			unsigned short mImagesCount;

			std::vector<kTexture*> mId;
	};

	/*
	class texture
	{
		protected:
			unsigned short mWidth;
			unsigned short mHeight;

			std::vector<kTexture*> mId;

		public:
			unsigned short getWidth()
			{
				return mWidth;
			}

			unsigned short getHeight()
			{
				return mHeight;
			}

			unsigned short getImagesCount()
			{
				return mId.size();
			}

			~texture()
			{
				textureLoader::getSingleton().unLoadTexture(mId);
			}

			void setTextureId(kTexture* id);
			kTexture* getId();

			void bind();
			void bindStage(unsigned short);
	};
	*/

	class textureStage
	{
		protected:
			/**
			 * Amount to scroll per frame on x and y
			 */
			vector2 mScroll;
			vector2 mScrolled;

			/**
			 * Amount to rotate per frame in degrees
			 */
			vec_t	mAngle;
			vec_t mRotate;

			/**
			 * How texture coordinates are defined for this texture
			 */
			texCoordType mTexCoordType;

			/**
			 * The texture index in multi texturing
			 */
			unsigned short mIndex;

			/**
			 * Internals of the texture file
			 */
			unsigned int mWidth, mHeight;

			/**
			 * Blendfunc
			 * Note that if both src and dst are ZERO
			 * blend will be disabled
			 */
			unsigned short mBlendSrc, mBlendDst;

			/**
			 * Tev unit or GLSL program for use on this texture stage. If this field is blank
			 * the default REPLACE stage is going to be used.
			 */
			std::string mProgram;

			/**
			 * How many images does the
			 * mTextureId array have
			 */
			unsigned short mImagesCount;

			/**
			 * Texture ID, specific to
			 * the low level implementation
			 */
			std::vector<kTexture*> mTextureId;

		public:
			textureStage(unsigned int width, unsigned int height,
					unsigned short index);

			void setProgram(const std::string& name);
			void setTexCoordType(texCoordType type);
			void setBlendMode(unsigned short src, unsigned short dst);
			void setScroll(vector2 scroll);
			void setRotate(vec_t angle);

			void setImagesCount(unsigned short count);
			unsigned short getImagesCount();

			unsigned int getWidth();
			unsigned int getHeight();
			void setId(std::vector<kTexture*>* id);
			std::vector<kTexture*>* getId();

			texCoordType getTexCoordType();

			/**
			 * Draw texture setting needed params.
			 */
			virtual void draw() = 0;

			/**
			 * Unset texture set params.
			 */
			virtual void finish() = 0;

	};

	#ifdef __WII__
	class wiiTexture : public textureStage
	{
		private:
			Mtx mTransRotate;

			void setTexCoordGen();

		public:
			wiiTexture(unsigned int width, unsigned int height,
					unsigned short index);

			void draw();
			void finish();
	};
	#else
	class glTexture : public textureStage
	{
		public:
			glTexture(unsigned int width, unsigned int height,
					unsigned short index);

			void draw();
			void finish();
	};
	#endif
}

#endif

