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

#ifndef __WII__
texture::texture(ILuint* src, unsigned int width, unsigned int height, unsigned short index)
#else
texture::texture(GXTexObj* src, unsigned int width, unsigned int height, unsigned short index)
#endif
{
	assert(src != NULL);

	#ifndef __WII__
	mDevilTextureId = src;
	ilBindImage(*mDevilTextureId);
	ilutRenderer(ILUT_OPENGL);
	mTextureId = ilutGLBindTexImage();
	#else
	guMtxIdentity(mTransRotate);
	mTextureId = src;
	#endif

	mIndex = index;
	mAngle = 0;
	mWidth = width;
	mHeight = height;
	mBlendSrc = 0;
	mBlendDst = 0;
	mRotate = 0;
	mScroll.x = 0;
	mScroll.y = 0;
}
			
void texture::setBlendMode(unsigned short src, unsigned short dst)
{
	mBlendSrc = src;
	mBlendDst = dst;
}

unsigned short texture::getBlendSrc()
{
	return mBlendSrc;
}

unsigned short texture::getBlendDst()
{
	return mBlendDst;
}

#ifndef __WII__
GLuint& texture::getTextureId()
#else
GXTexObj* texture::getTextureId()
#endif
{
	return mTextureId;
}
			
unsigned int texture::getWidth()
{
	return mWidth;
}

void texture::setProgram(const std::string& name)
{
	mProgram = name;	
}

unsigned int texture::getHeight()
{
	return mHeight;
}
			
void texture::setTexCoordType(texCoordType type)
{
	mTexCoordType = type;
}

void texture::setScroll(vector2 scroll)
{
	mScroll = scroll;
}

void texture::setRotate(vec_t angle)
{
	mRotate = angle;
}

#ifdef __WII__
void texture::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	if (mScroll.x || mScroll.y || mRotate)
	{
		assert(mIndex < 8);

		vec_t sinAngle = sin(mAngle);
		vec_t cosAngle = cos(mAngle);
		mAngle += mRotate;

		mTransRotate[0][0] = cosAngle;
		mTransRotate[1][0] = -sinAngle;

		mTransRotate[0][1] = sinAngle;
		mTransRotate[1][1] = cosAngle;

		mTransRotate[0][2] = 0;
		mTransRotate[1][2] = 0;

		mTransRotate[0][3] += mScroll.x;
		mTransRotate[1][3] += mScroll.y;

		GX_LoadTexMtxImm(mTransRotate, GX_TEXMTX0 + mIndex, GX_TG_MTX2x4);
		GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_TEX0 + mIndex, GX_TEXMTX0 + mIndex);
	}
	else 
	{
		GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_TEX0 + mIndex, GX_IDENTITY);
	}

	// TEV
	tev* thisTev = tevManager::getSingleton().getTev(mProgram);
	if (!mProgram.length() || !thisTev)
	{
		GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_REPLACE);
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

	rs->bindTexture(mTextureId, mIndex);
}
#else
void texture::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	glClientActiveTextureARB(GL_TEXTURE0_ARB + mIndex);
	glActiveTextureARB(GL_TEXTURE0_ARB + mIndex);

	if (mProgram.length())
	{
		if (mProgram == "replace")
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		else
		if (mProgram == "modulate")
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		else
		if (mProgram == "blend")
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		else
		if (mProgram == "decal")
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		else
		if (mProgram == "add")
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		else
		{
			// TODO - setup a glsl program here
		}
	}
	else
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}

	if (mBlendSrc && mBlendDst)
	{
		rs->setBlend(true);
		rs->setBlendMode(mBlendSrc, mBlendDst);
	}
	else
	{
		rs->setBlend(false);
	}

	rs->bindTexture(mTextureId, mIndex);

	switch (mTexCoordType)
	{
		case TEXCOORD_UV:
			// Dont touch texture matrix
			if (!mRotate || !mScroll.x || !mScroll.y)
				break;

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();

			if (mRotate)
			{
				glRotatef(mAngle, 0, 0, 1);
				mAngle += mRotate;
			}

			if (mScroll.x || mScroll.y)
			{
				glTranslatef(mScrolled.x, mScrolled.y, 0);
				mScrolled.x += mScroll.x;
				mScrolled.y += mScroll.y;
			}
			break;
		case TEXCOORD_SPHERE:
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			break;
	}
}
#endif

#ifdef __WII__
void texture::finish()
{
}
#else
void texture::finish()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	glClientActiveTextureARB(GL_TEXTURE0_ARB + mIndex);
	glActiveTextureARB(GL_TEXTURE0_ARB + mIndex);
		
	// Bind Texture 0
	rs->bindTexture(0, mIndex);

	// Reset params
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Reset Blending
	rs->setBlend(false);

	switch (mTexCoordType)
	{
		case TEXCOORD_UV:
			if (!mRotate || !mScroll.x || !mScroll.y)
				break;

			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			break;

		case TEXCOORD_SPHERE:
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			break;
	}
}
#endif

}

