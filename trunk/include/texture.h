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
#include "tev.h"

namespace k
{
	enum texCoordType 
	{
		TEXCOORD_NONE,
		TEXCOORD_UV,
		TEXCOORD_POS,
		TEXCOORD_NORMAL,
		TEXCOORD_BINORMAL,
		TEXCOORD_TANGENT
	};

	class texture 
	{
		private:
			/**
			 * Amount to scroll per frame on x and y
			 */
			vector2 mScroll;

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

			#ifdef __WII__
				Mtx mTransRotate;
				GXTexObj* mTextureId;
			#else
				vector2 mScrolled;
				GLuint mTextureId;
				ILuint* mDevilTextureId;
			#endif

		public:
			#ifndef __WII__
			texture(ILuint* src, unsigned int width, unsigned int height, unsigned short index);
			#else
			texture(GXTexObj* src, unsigned int width, unsigned int height, unsigned short index);
			#endif

			#ifndef __WII__
			GLuint& getTextureId();
			#else
			GXTexObj* getTextureId();
			#endif

			void setProgram(const std::string& name);
			void setTexCoordType(texCoordType type);
			void setBlendMode(unsigned short src, unsigned short dst);
			void setScroll(vector2 scroll);
			void setRotate(vec_t angle);
			void draw();

			unsigned short getBlendSrc();
			unsigned short getBlendDst();

			unsigned int getWidth();
			unsigned int getHeight();
	};
}

#endif

