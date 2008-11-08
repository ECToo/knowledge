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

#ifdef __WII__

#include "texture.h"
#include "root.h"
#include "logger.h"
#include "tev.h"

namespace k {

wiiTexture::wiiTexture(unsigned int width, unsigned int height, unsigned short index)
	: textureStage(width, height, index)
{
	guMtxIdentity(mTransRotate);
}

void wiiTexture::setId(void* id)
{
	assert(id != NULL);
	mTextureId = (GXTexObj*)id;
}

void wiiTexture::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	if (mScroll.x || mScroll.y || mRotate)
	{
		assert(mIndex < 8);

		vec_t sinAngle = sin(mAngle);
		vec_t cosAngle = cos(mAngle);
		mAngle += mRotate * M_PI / 180.0f;

		mScrolled.x += mScroll.x;
		mScrolled.y += mScroll.y;

		mTransRotate[0][0] = cosAngle;
		mTransRotate[0][1] = -sinAngle;
		mTransRotate[0][2] = 0;

		mTransRotate[1][0] = sinAngle;
		mTransRotate[1][1] = cosAngle;
		mTransRotate[1][2] = 0;

		mTransRotate[2][0] = 0;
		mTransRotate[2][1] = 0;
		mTransRotate[2][2] = 1;
				
		mTransRotate[0][3] = 0.5 - 0.5 * cosAngle + 0.5 * sinAngle + mScrolled.x;
		mTransRotate[1][3] = 0.5 - 0.5 * sinAngle - 0.5 * cosAngle + mScrolled.y;
		mTransRotate[2][3] = 0;

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

void wiiTexture::finish()
{
}

// This shouldnt be here but i have no choice
texture::~texture()
{
}

}

#endif

