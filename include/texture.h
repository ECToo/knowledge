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

	enum texFlags
	{
		FLAG_CLAMP_EDGE_S = 0,
		FLAG_CLAMP_EDGE_T,
		FLAG_CLAMP_EDGE_R,
		FLAG_CLAMP_S,
		FLAG_CLAMP_T,
		FLAG_CLAMP_R,
		FLAG_REPEAT_S,
		FLAG_REPEAT_T,
		FLAG_REPEAT_R,
		FLAG_RGB,
		FLAG_RGBA,
		FLAG_BGR,
		FLAG_BGRA
	};

	class DLL_EXPORT texture
	{
		private:
			unsigned short mWidth;
			unsigned short mHeight;

			std::vector<char*> mTextureData;
			std::vector<std::string> mFilenames;
			std::vector<kTexture*> mId;

		public:
			texture();
			~texture();

			void push(kTexture* tex, unsigned short w, unsigned short h);
			void push(const std::string& filename);
			void push(char* data);

			unsigned short getWidth();
			unsigned short getHeight();
			unsigned short getSize();

			char* getData(int i);
			kTexture* getId(int i);

			bool containsFilename(const std::string& name);
	};

	class DLL_EXPORT textureStage
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
			 * Textures
			 */
			texture* mTexture;

		public:
			textureStage(unsigned short index);

			void setProgram(const std::string& name);
			void setTexCoordType(texCoordType type);
			void setBlendMode(unsigned short src, unsigned short dst);
			void setScroll(vector2 scroll);
			void setRotate(vec_t angle);
			void setTexture(texture* tex);

			bool containsTexture(const std::string& name);

			unsigned short getWidth();
			unsigned short getHeight();
			unsigned short getImagesCount();

			kTexture* getTexture(int i);
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
	class DLL_EXPORT wiiTexture : public textureStage
	{
		private:
			Mtx mTransRotate;
			void setTexCoordGen();

		public:
			wiiTexture(unsigned short index);

			void draw();
			void finish();
	};
	typedef wiiTexture platTextureStage;
	#else
	class DLL_EXPORT glTexture : public textureStage
	{
		public:
			glTexture(unsigned short index);

			void draw();
			void finish();
	};
	typedef glTexture platTextureStage;
	#endif
}

#endif

