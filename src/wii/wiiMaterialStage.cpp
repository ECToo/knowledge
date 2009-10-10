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

#ifdef __WII__

#include "texture.h"
#include "root.h"
#include "logger.h"

namespace k {

void materialStage::draw()
{
	bool notFound = true;
	for (int i = 0; i < mTexturesCount; i++)
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

	switch (mCoordType)
	{
		default: S_LOG_INFO("Undefined texture coordinate type, falling back to UV.");
		case TEXCOORD_UV:
			if (mScroll.x || mScroll.y || mRotate || mScale.x || mScale.y)
			{
				kAssert(mIndex < 8);
				static Mtx mTransRotate;

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
				// TODO: Something is wrong
				renderSystem* rs = root::getSingleton().getRenderSystem();

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

	switch (mTexEnv)
	{
		default:
		case TEXENV_REPLACE:
			GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_REPLACE);
			break;

		case TEXENV_MODULATE:
			GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_MODULATE);
			break;

		case TEXENV_BLEND:
			GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_BLEND);
			break;

		case TEXENV_DECAL:
			GX_SetTevOp(GX_TEVSTAGE0 + mIndex, GX_DECAL);
			break;

		case TEXENV_ADD:
			// TODO: Call tev manager with add =]
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

}

void materialStage::finish() {}

}

#endif

