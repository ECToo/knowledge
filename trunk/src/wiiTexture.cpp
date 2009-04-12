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

wiiTexture::wiiTexture(unsigned short index) : textureStage(index)
{
	guMtxIdentity(mTransRotate);
	mProgram.clear();
}
			
void wiiTexture::setTexCoordGen()
{
	switch (mTexCoordType)
	{
		default:
			S_LOG_INFO("Undefined texture coordinate type, fallbacking to UV.");
		case TEXCOORD_UV:
			if (mScroll.x || mScroll.y || mRotate || mScale.x || mScale.y)
			{
				kAssert(mIndex < 8);

				vec_t sinAngle = sin(mAngle);
				vec_t cosAngle = cos(mAngle);
				mAngle += mRotate * M_PI / 180.0f;

				mScrolled.x += mScroll.x;
				mScrolled.y += mScroll.y;

				mTransRotate[0][0] = cosAngle * mScale.x;
				mTransRotate[0][1] = -sinAngle;
				mTransRotate[0][2] = 0;

				mTransRotate[1][0] = sinAngle;
				mTransRotate[1][1] = cosAngle * mScale.y;
				mTransRotate[1][2] = 0;

				mTransRotate[2][0] = 0;
				mTransRotate[2][1] = 0;
				mTransRotate[2][2] = 1;
				
				mTransRotate[0][3] = 0.5 - 0.5 * cosAngle + 0.5 * sinAngle + mScrolled.x;
				mTransRotate[1][3] = 0.5 - 0.5 * sinAngle - 0.5 * cosAngle + mScrolled.y;
				mTransRotate[2][3] = 0;

				GX_LoadTexMtxImm(mTransRotate, GX_TEXMTX0 + mIndex * 3, GX_TG_MTX2x4);
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_TEX0 + mIndex, GX_TEXMTX0 + mIndex * 3);
			}
			else 
			{
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_TEX0 + mIndex, GX_IDENTITY);
			}
			break;

		case TEXCOORD_NORMAL:
			{
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_NRM, GX_IDENTITY);
			}
			break;

		case TEXCOORD_POS:
			{
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_POS, GX_IDENTITY);
			}
			break;

		case TEXCOORD_BINORMAL:
			{
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_BINRM, GX_IDENTITY);
			}
			break;

		case TEXCOORD_TANGENT:
			{
				GX_SetTexCoordGen(GX_TEXCOORD0 + mIndex, GX_TG_MTX2x4, GX_TG_TANGENT, GX_IDENTITY);
			}
			break;

		case TEXCOORD_SPHERE:
			{
				renderSystem* rs = root::getSingleton().getRenderSystem();
				kAssert(rs != NULL);

				Mtx s, t, destMtx;
				Mtx postMtx;

				rs->getModelView(postMtx);

				guMtxScale(s, 0.5f, -0.5f, 0.0f);
				guMtxTrans(t, 0.5f, 0.5f, 1.0f);
				guMtxConcat(s, postMtx, postMtx);
				guMtxConcat(t, postMtx, postMtx);

				guMtxIdentity(destMtx);

				GX_LoadTexMtxImm(postMtx, GX_TEXMTX0 + mIndex * 3, GX_TG_MTX3x4);
				GX_LoadTexMtxImm(destMtx, GX_DTTMTX0 + mIndex * 3, GX_TG_MTX3x4);

				GX_SetTexCoordGen2(GX_TEXCOORD0 + mIndex, GX_TG_MTX3x4, GX_TG_NRM, 
						GX_TEXMTX0 + mIndex * 3, GX_TRUE, GX_DTTMTX0 + mIndex * 3);
			}
			break;
	}
}

void wiiTexture::draw()
{
	if (!mTexture)
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();
	kAssert(rs != NULL);

	// Texture Coordinate
	setTexCoordGen();

	// TEV
	tev* thisTev = tevManager::getSingleton().getTev(mProgram);
	if (!mProgram.length() || !thisTev || mProgram == "replace")
	{
		GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_REPLACE);
	}
	else
	if (mProgram == "decal")
	{
		GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_DECAL);
	}
	else
	if (mProgram == "blend")
	{
		GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_BLEND);
	}
	else
	if (mProgram == "modulate")
	{
		GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_MODULATE);
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

	rs->bindTexture(getTexture(0), mIndex);
}

void wiiTexture::finish() 
{
}

}

#endif

