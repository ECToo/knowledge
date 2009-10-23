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

#ifndef __WII__

#include "texture.h"
#include "root.h"
#include "logger.h"

namespace k {

void materialStage::draw()
{
	bool notFound = true;
	for (unsigned int i = 0; i < mTexturesCount; i++)
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
	rs->bindTexture(getTexture((uint32_t)mCurrentFrame)->getPointer(), mIndex);

	switch (mTexEnv)
	{
		default:
		case TEXENV_REPLACE:
			if (rs->isLightOn())
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			else
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
	for (unsigned int i = 0; i < mTexturesCount; i++)
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
			glDisable(GL_TEXTURE_GEN_R);
			break;
	}
}

}

#endif

