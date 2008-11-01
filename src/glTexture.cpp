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

#include "texture.h"
#include "root.h"
#include "logger.h"

namespace k {

glTexture::glTexture(unsigned int width, unsigned int height, unsigned short index)
	: textureStage(width, height, index)
{
}

void glTexture::setId(void* id)
{
	assert(id != NULL);

	mDevilTextureId = (ILuint*)id;
	ilBindImage(*mDevilTextureId);
	ilutRenderer(ILUT_OPENGL);

	mTextureId = ilutGLBindTexImage();
}

void glTexture::draw()
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

void glTexture::finish()
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

// This shouldnt be here but i have no choice
texture::~texture()
{
}

}

#endif

