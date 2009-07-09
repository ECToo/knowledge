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

#ifndef __WII__

#include "texture.h"
#include "root.h"
#include "logger.h"

namespace k {

void materialStage::draw()
{
	bool notFound = true;
	for (int i = 0; i < K_MAX_STAGE_TEXTURES; i++)
	{
		if (mTextures[i])
		{
			notFound = false;
			break;
		}
	}

	if (notFound)
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();
	rs->bindTexture(getTexture(0)->getPointer(), mIndex);

	switch (mTexEnv)
	{
		default:
		case TEXENV_REPLACE:
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			break;

		case TEXENV_MODULATE:
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			break;

		case TEXENV_BLEND:
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
			break;

		case TEXENV_DECAL:
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			break;

		case TEXENV_ADD:
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
			break;
	}

	if (mBlendSrc || mBlendDst)
	{
		rs->setBlend(true);
		rs->setBlendMode(mBlendSrc, mBlendDst);
	}
	else
	{
		rs->setBlend(false);
	}

	switch (mCoordType)
	{
		default:
		case TEXCOORD_UV:
			// Dont touch texture matrix
			if (!mRotate && !mScroll.x && !mScroll.y && !mScale.x && !mScale.y)
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

			if (mScale.x || mScale.y)
			{
				glScalef(mScale.x, mScale.y, 1);
			}
			break;
		case TEXCOORD_SPHERE:
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);
			break;
	}
}

void materialStage::finish()
{
	bool notFound = true;
	for (int i = 0; i < K_MAX_STAGE_TEXTURES; i++)
	{
		if (mTextures[i])
		{
			notFound = false;
			break;
		}
	}

	if (notFound)
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();

	// unBind texture
	rs->unBindTexture(mIndex);

	// Reset params
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Reset Blending
	if (mBlendSrc || mBlendDst)
		rs->setBlend(false);

	switch (mCoordType)
	{
		default:
		case TEXCOORD_NONE:
		case TEXCOORD_UV:
			glMatrixMode(GL_TEXTURE);
			glLoadIdentity();
			break;

		case TEXCOORD_SPHERE:
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			break;
	}
}

}

#endif

