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

// Jpeg is not C++ safe
extern "C" 
{
	#include <stdio.h>
	#include <jpeglib.h>
	#include <jerror.h>
}

#include "wiiRenderSystem.h"
#include "logger.h"

namespace k {

static inline void copyMatrix4(const matrix4& src, Mtx44 dst)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			dst[i][j] = src.m[i][j];
}

static inline void copyMtx44(Mtx44 src, Mtx44 dst)
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			dst[i][j] = src[i][j];
}

static inline void identityMtx44(Mtx44 mat)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (i == j)
				mat[i][j] = 1.0;
			else
				mat[i][j] = 0.0;
		}
	}
}

static inline void copyMtx(const matrix4& src, Mtx dest)
{
	guMtxIdentity(dest);

	dest[0][0] = src.m[0][0];
	dest[0][1] = src.m[0][1];
	dest[0][2] = src.m[0][2];
	dest[0][3] = src.m[0][3];

	dest[1][0] = src.m[1][0];
	dest[1][1] = src.m[1][1];
	dest[1][2] = src.m[1][2];
	dest[1][3] = src.m[1][3];

	dest[2][0] = src.m[2][0];
	dest[2][1] = src.m[2][1];
	dest[2][2] = src.m[2][2];
	dest[2][3] = src.m[2][3];
}

static inline void copyMtxTranspose(const matrix4& src, Mtx dest)
{
	guMtxIdentity(dest);

	dest[0][0] = src.m[0][0];
	dest[0][1] = src.m[1][0];
	dest[0][2] = src.m[2][0];
	dest[0][3] = src.m[3][0];

	dest[1][0] = src.m[0][1];
	dest[1][1] = src.m[1][1];
	dest[1][2] = src.m[2][1];
	dest[1][3] = src.m[3][1];

	dest[2][0] = src.m[0][2];
	dest[2][1] = src.m[1][2];
	dest[2][2] = src.m[2][2];
	dest[2][3] = src.m[3][2];
}

matrixStack::matrixStack()
{
	for (int i = 0; i < 32; i++)
	{
		guMtxIdentity(stack[i]);
	}

	// Top of the stack
	mPosition = 32;
}

void matrixStack::push(Mtx& matrix)
{
	if (mPosition <= 0)
		return;

	guMtxCopy(matrix, stack[--mPosition]);
}

void matrixStack::pop(Mtx& destination)
{
	if (mPosition < 32)
	{
		guMtxCopy(stack[mPosition++], destination);
	}
}

matrix44Stack::matrix44Stack()
{
	for (int i = 0; i < 16; i++)
	{
		identityMtx44(stack[i]);
	}

	// Top of the stack
	mPosition = 16;
}

void matrix44Stack::push(Mtx44 matrix)
{
	if (mPosition <= 0)
		return;

	copyMtx44(matrix, stack[--mPosition]);
}

void matrix44Stack::pop(Mtx44 destination)
{
	if (mPosition < 32)
		copyMtx44(stack[mPosition++], destination);
}

wiiRenderSystem::wiiRenderSystem()
{
}

wiiRenderSystem::~wiiRenderSystem()
{
	deinitialize();
}

void wiiRenderSystem::initialize()
{
	VIDEO_Init();
}

void wiiRenderSystem::deinitialize()
{
	//TODO
}

void wiiRenderSystem::configure()
{
	// Setup screen
	mVideoMode = VIDEO_GetPreferredMode(NULL);

	// Framebuffers
	mFrameBuffers[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(mVideoMode));
	mFrameBuffers[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(mVideoMode));

	// Starts on the first
	mBufferIndex = 0;

	// Configure it =]
	VIDEO_Configure(mVideoMode);
	VIDEO_SetNextFramebuffer(mFrameBuffers[mBufferIndex]);
	VIDEO_SetBlack(FALSE);

	// Clear FrameBuffers
	VIDEO_ClearFrameBuffer(mVideoMode, mFrameBuffers[0], COLOR_BLACK);
	VIDEO_ClearFrameBuffer(mVideoMode, mFrameBuffers[1], COLOR_BLACK);

	VIDEO_Flush();
	VIDEO_WaitVSync();

	if (mVideoMode->viTVMode & VI_NON_INTERLACE) 
		VIDEO_WaitVSync();

	// Allocate the FIFO
	mFifo = memalign(32, DEFAULT_FIFO_SIZE);
	memset(mFifo, 0, DEFAULT_FIFO_SIZE);

	// initialize GX
	GX_Init(mFifo, DEFAULT_FIFO_SIZE);

	// Set Default Clear Color to black
	setClearColor(vector3(0, 0, 0));
	setClearDepth(0x00ffffff);
	GX_SetCopyClear(mClearColor, mClearDepth);

	// Setup Default Viewport and Scissors (culling)
	GX_SetViewport(0, 0, mVideoMode->fbWidth, mVideoMode->efbHeight, 0, 1);
	f32 yscale = GX_GetYScaleFactor(mVideoMode->efbHeight, mVideoMode->xfbHeight);
	f32 xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0, 0, mVideoMode->fbWidth, mVideoMode->efbHeight);
	GX_SetDispCopyDst(mVideoMode->fbWidth, xfbHeight);
	GX_SetCopyFilter(mVideoMode->aa, mVideoMode->sample_pattern, GX_TRUE, mVideoMode->vfilter);
	
	if (mVideoMode->viHeight == (2 * mVideoMode->xfbHeight))
	{
		GX_SetFieldMode(mVideoMode->field_rendering, GX_ENABLE);
	}
	else
	{
		GX_SetFieldMode(mVideoMode->field_rendering, GX_DISABLE);
	}

	// Pixel Format
	if (mVideoMode->aa)
	{
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	}
	else
	{
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	}

	// Initial Culling is 0
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(mFrameBuffers[mBufferIndex], GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	// Reset Textures and Vertices
	GX_InvVtxCache();
	GX_InvalidateTexAll();

	// Setup perspective
	setMatrixMode(MATRIXMODE_PROJECTION);
	identityMatrix();

	// Depth Testing
	mDepthTest = mDepthMask = true;

	// Perspective
	setPerspective(90, 1.33f, 0.1f, 1000.0f);

	// Render to texture buffer
	mRttBuffer = (char*) memalign(32, mVideoMode->fbWidth * mVideoMode->efbHeight * 4);
	if (!mRttBuffer)
	{
		S_LOG_INFO("Failed to allocate texture for RTT buffer.");
	}

	// Clean Textures and Materials
	mActiveMaterial = NULL;
	mActiveTextures.clear();

	// RTT
	mRenderToTexture = false;
	mRttTarget = NULL;
	mRttDimensions[0] = getScreenWidth();
	mRttDimensions[1] = getScreenHeight();
}

void wiiRenderSystem::createWindow(const int w, const int h)
{}
			
void wiiRenderSystem::setBlendMode(unsigned short src, unsigned short dst)
{
	GX_SetBlendMode(GX_BM_BLEND, src, dst, GX_LO_CLEAR);
}

void wiiRenderSystem::setBlend(bool state)
{
	if (!state)
		GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);

	GX_SetAlphaUpdate(state);
}

void wiiRenderSystem::setDepthMask(bool state)
{
	GX_SetZMode(GX_TRUE, GX_LEQUAL, state);
}

void wiiRenderSystem::destroyWindow()
{}
			
void wiiRenderSystem::setWindowTitle(const std::string& title)
{}

void wiiRenderSystem::frameStart()
{
	GX_InvVtxCache();
}

void wiiRenderSystem::frameEnd()
{
	if (mRenderToTexture)
	{
		if (!mRttTarget)
		{
			S_LOG_INFO("Render to texture target not defined, aborting.");
			kAssert(mRttTarget);
		}

		copyBufferToTexture();

		// Clean Textures
		mActiveTextures.clear();
		mActiveMaterial = NULL;

		mRenderToTexture = false;
		mRttTarget = NULL;

		return;
	}

	GX_DrawDone();

	// Flip Buffers
	mBufferIndex ^= 1;
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(mFrameBuffers[mBufferIndex], GX_TRUE);

	VIDEO_SetNextFramebuffer(mFrameBuffers[mBufferIndex]);
 	VIDEO_Flush();
	VIDEO_WaitVSync();

	// Clean Textures
	mActiveTextures.clear();
	mActiveMaterial = NULL;
}
			
void wiiRenderSystem::setWireFrame(bool wire)
{
	// TODO
}

void wiiRenderSystem::setClearColor(const vector3& color)
{
	mClearColor.r = color.x*255;
	mClearColor.g = color.y*255;
	mClearColor.b = color.z*255;
	mClearColor.a = 255;
	GX_SetCopyClear(mClearColor, mClearDepth);
}

void wiiRenderSystem::setClearDepth(const vec_t amount)
{
	mClearDepth = amount;
	GX_SetCopyClear(mClearColor, mClearDepth);
}

void wiiRenderSystem::setDepthTest(bool test)
{
	// Nothing =D
}

void wiiRenderSystem::setShadeModel(ShadeModel model)
{
	// TODO
}

void wiiRenderSystem::setMatrixMode(MatrixMode mode)
{
	mActiveMatrix = mode;
}

void wiiRenderSystem::pushMatrix()
{
	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			mProjectionStack.push(mProjectionMatrix);
			break;
		case MATRIXMODE_MODELVIEW:
			mModelViewStack.push(mModelViewMatrix);
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::popMatrix()
{
	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			mProjectionStack.pop(mProjectionMatrix);
			break;
		case MATRIXMODE_MODELVIEW:
			mModelViewStack.pop(mModelViewMatrix);
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::identityMatrix()
{
	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			identityMtx44(mProjectionMatrix);
			break;
		case MATRIXMODE_MODELVIEW:
			guMtxIdentity(mModelViewMatrix);
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

matrix4 wiiRenderSystem::getModelView()
{
	matrix4 temp;
	guMtxCopy(mModelViewMatrix, temp.m);

	return temp;
}

void wiiRenderSystem::getModelView(float mat[][4])
{
	guMtxCopy(mModelViewMatrix, mat);
}

void wiiRenderSystem::setInverseTransposeModelview(const matrix4& mat)
{
	copyMtx(mat, mInverseModelViewMatrix);
}

void wiiRenderSystem::copyMatrix(const matrix4& mat)
{
	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			copyMatrix4(mat, mProjectionMatrix);
			break;
		case MATRIXMODE_MODELVIEW:
			copyMtxTranspose(mat, mModelViewMatrix);
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::multMatrix(const matrix4& mat)
{
	Mtx temp;

	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			copyMtxTranspose(mat, temp);
			guMtxConcat(mProjectionMatrix, temp, mProjectionMatrix);
			break;
		case MATRIXMODE_MODELVIEW:
			copyMtxTranspose(mat, temp);
			guMtxConcat(mModelViewMatrix, temp, mModelViewMatrix);
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::translateScene(vec_t x, vec_t y, vec_t z)
{
	if (!x && !y && !z)
		return;

	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			{
				Mtx44 temp;

				identityMtx44(temp);
				guMtxTrans(temp, x, y, z);
				guMtxConcat(mProjectionMatrix, temp, mProjectionMatrix);
			}
			break;
		case MATRIXMODE_MODELVIEW:
			{
				guMtxTransApply(mModelViewMatrix, mModelViewMatrix, x, y, z);
			}
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::rotateScene(vec_t angle, vec_t x, vec_t y, vec_t z)
{
	Mtx temp;
	Vector axis;

	if (angle == 0 || angle == M_PI*2)
		return;

	axis.x = x;
	axis.y = y;
	axis.z = z;

	guMtxIdentity(temp);
	guMtxRotAxisDeg(temp, &axis, angle);

	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			guMtxConcat(mProjectionMatrix, temp, mProjectionMatrix);
			break;
		case MATRIXMODE_MODELVIEW:
			guMtxConcat(mModelViewMatrix, temp, mModelViewMatrix);
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::scaleScene(vec_t x, vec_t y, vec_t z)
{
	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			{
				Mtx44 temp;

				guMtxIdentity(temp);
				guMtxScale(temp, x, y, z);
				guMtxConcat(mProjectionMatrix, temp, mProjectionMatrix);
			}
			break;
		case MATRIXMODE_MODELVIEW:
			{
				Mtx temp;

				guMtxIdentity(temp);
				guMtxScale(temp, x, y, z);
				guMtxConcat(mModelViewMatrix, temp, mModelViewMatrix);
			}
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::setViewPort(int x, int y, int w, int h)
{
	GX_SetViewport(x, y, w, h, 0, 1);
	GX_SetScissor(x, y, w, h);
}

void wiiRenderSystem::setPerspective(vec_t fov, vec_t aspect, vec_t nearP, vec_t farP)
{
	Mtx44 perspective;

	identityMtx44(perspective);
	guPerspective(perspective, fov, aspect, nearP, farP);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			copyMtx44(perspective, mProjectionMatrix);
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::setOrthographic(vec_t left, vec_t right, vec_t bottom, vec_t top, vec_t nearP, vec_t farP)
{
	Mtx44 ortho;

	// Setup black borders
	if (left == 0 && top == 0)
	{
		top = -40;
		left = -20;
	}

	guOrtho(ortho, top, bottom, left, right, nearP, farP);
	GX_LoadProjectionMtx(ortho, GX_ORTHOGRAPHIC);

	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			copyMtx44(ortho, mProjectionMatrix);
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::setCulling(CullMode culling)
{
	switch (culling)
	{
		case CULLMODE_NONE:
			GX_SetCullMode(GX_CULL_NONE);
			break;
		case CULLMODE_BACK:
			GX_SetCullMode(GX_CULL_FRONT);
			break;
		case CULLMODE_FRONT:
			GX_SetCullMode(GX_CULL_BACK);
			break;
		case CULLMODE_BOTH:
			GX_SetCullMode(GX_CULL_ALL);
			break;
	};
}

void wiiRenderSystem::startVertices(VertexMode mode)
{
	mRenderingMode = mode;

	mVertices.clear();
	mNormals.clear();
	mColors.clear();
	mTexCoords.clear();
}

void wiiRenderSystem::vertex(const vector3& vert)
{
	mVertices.push_back(vert);
}

void wiiRenderSystem::normal(const vector3& norm)
{
	mNormals.push_back(norm);
}

void wiiRenderSystem::color(const vector3& col)
{
	mColors.push_back(col);
}

void wiiRenderSystem::texCoord(const vector2& coord)
{
	mTexCoords.push_back(coord);
}

void wiiRenderSystem::endVertices()
{
	unsigned int mVertexSize = mVertices.size();

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

	u8 texCoord = GX_TEXCOORDNULL;
	u32 texMap = GX_TEXMAP_NULL;
	u8 tevColor = GX_COLORNULL;

	bool renderColors = (mColors.size() == mVertexSize);
	if (renderColors)
	{
 		GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGB8, 0);

		tevColor = GX_COLOR0A0;
	}
	else
	{
		GX_SetVtxDesc(GX_VA_CLR0, GX_NONE);
	}

	bool renderNormals = (mNormals.size() == mVertexSize);
	if (renderNormals)
	{
 		GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_POS_XYZ, GX_F32, 0);
	}

	bool renderTexCoords = (mTexCoords.size() == mVertexSize);
	if (renderTexCoords)
	{
 		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_TEX0MTXIDX, GX_TEXMTX0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

		if (mActiveMaterial)
		for (unsigned int i = 1; i < mActiveMaterial->getTextureUnits(); i++)
		{
 			GX_SetVtxDesc(GX_VA_TEX0 + i, GX_DIRECT);
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + i, GX_TEX_ST, GX_F32, 0);
		}

		texCoord = GX_TEXCOORD0;
		texMap = GX_TEXMAP0;
	}
	else
	{
		GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
	}

	GX_SetTevOrder(GX_TEVSTAGE0, texCoord, texMap, tevColor);
	if (mActiveMaterial)
	{
		for (unsigned int i = 1; i < mActiveMaterial->getTextureUnits(); i++)
			GX_SetTevOrder(GX_TEVSTAGE0 + i, GX_TEXCOORD0 + i, GX_TEXMAP0 + i, tevColor);
	}

	std::map<int, GXTexObj*>::const_iterator it;
	for (it = mActiveTextures.begin(); it != mActiveTextures.end(); it++)
	{
		kAssert(it->second);
		GX_LoadTexObj(it->second, GX_TEXMAP0 + it->first);
	}

	// ModelView
	GX_LoadPosMtxImm(mModelViewMatrix, GX_PNMTX0);
	GX_LoadNrmMtxImm(mInverseModelViewMatrix, GX_PNMTX0);
	GX_SetCurrentMtx(GX_PNMTX0);

	// Start the Vertex Descriptor on Flipper
	switch (mRenderingMode)
	{
		case VERTEXMODE_LINE:
			GX_Begin(GX_LINES, GX_VTXFMT0, mVertices.size());
			break;
		default:
		case VERTEXMODE_TRIANGLES:
			GX_Begin(GX_TRIANGLES, GX_VTXFMT0, mVertices.size());
			break;
		case VERTEXMODE_QUAD:
			GX_Begin(GX_QUADS, GX_VTXFMT0, mVertices.size());
			break;
	}

	// Draw Vertices
	unsigned int index = 0;
	std::list<vector3>::iterator vIt;
	for (vIt = mVertices.begin(); vIt != mVertices.end(); vIt++)
	{
		GX_Position3f32((*vIt).x, (*vIt).y, (*vIt).z);

		if (renderColors)
		{
			GX_Color3f32(mColors[index].x, mColors[index].y, mColors[index].z);
		}

		if (renderNormals)
		{
			GX_Normal3f32(mNormals[index].x, mNormals[index].y, mNormals[index].z);
		}

		if (renderTexCoords)
		{
			GX_TexCoord2f32(mTexCoords[index].x, mTexCoords[index].y);
			if (mActiveMaterial)
			{
				for (unsigned int i = 1; i < mActiveMaterial->getTextureUnits(); i++)
					GX_TexCoord2f32(mTexCoords[index].x, mTexCoords[index].y);
			}
		}

		index++;
	}

	GX_End();
}

void wiiRenderSystem::matAmbient(const vector3& color)
{
	//TODO
}

void wiiRenderSystem::matDiffuse(const vector3& color)
{
	//TODO
}

void wiiRenderSystem::matSpecular(const vector3& color)
{
	//TODO
}

void wiiRenderSystem::bindTexture(GXTexObj* tex, int chan)
{
	kAssert(chan < 8);
	mActiveTextures[chan] = tex;
}
			
void wiiRenderSystem::unBindTexture(int chan)
{
	kAssert(chan < 8);
	mActiveTextures[chan] = NULL;
}

void wiiRenderSystem::drawArrays()
{
	GX_ClearVtxDesc();

	GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

	if (!mVertexArray)
	{
		S_LOG_INFO("Missing vertex array");
		return;
	}

	// Define if theres a material and how many
	// texture units it has.
	unsigned short materialTextureUnits = 0;

	if (mActiveMaterial)
		materialTextureUnits = mActiveMaterial->getTextureUnits();

	DCFlushRange((void*)mVertexArray, mVertexCount * sizeof(vec_t) * 3);
	GX_SetArray(GX_VA_POS, (void*)mVertexArray, 3 * sizeof(vec_t));

	u8 tevStage = GX_TEVSTAGE0;
	u8 texCoord = GX_TEXCOORDNULL;
	u32 texMap = GX_TEXMAP_NULL;
	u8 tevColor = GX_COLORNULL;

	if (mTexCoordArray)
	{
 		GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);
		GX_SetVtxDesc(GX_VA_TEX0MTXIDX, GX_TEXMTX0);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

		DCFlushRange((void*)mTexCoordArray, mVertexCount * sizeof(vec_t) * 2);
		GX_SetArray(GX_VA_TEX0, (void*)mTexCoordArray, 2 * sizeof(vec_t));

		for (unsigned int i = 1; i < materialTextureUnits; i++)
		{
 			GX_SetVtxDesc(GX_VA_TEX0 + i, GX_INDEX16);
			GX_SetArray(GX_VA_TEX0 + i, (void*)mTexCoordArray, 2 * sizeof(vec_t));
			GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + i, GX_TEX_ST, GX_F32, 0);
		}

		texCoord = GX_TEXCOORD0;
		texMap = GX_TEXMAP0;
	}
	else
	{
		GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
	}

	if (mNormalArray)
	{
 		GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);

		DCFlushRange((void*)mNormalArray, mVertexCount * sizeof(vec_t) * 3);
		GX_SetArray(GX_VA_NRM, (void*)mNormalArray, 3 * sizeof(vec_t));
	}

	GX_SetTevOrder(GX_TEVSTAGE0, texCoord, texMap, tevColor);
	for (unsigned int i = 1; i < materialTextureUnits; i++)
		GX_SetTevOrder(GX_TEVSTAGE0 + i, GX_TEXCOORD0 + i, GX_TEXMAP0 + i, tevColor);

	// Bind the textures
	std::map<int, GXTexObj*>::const_iterator it;
	for (it = mActiveTextures.begin(); it != mActiveTextures.end(); it++)
	{
		kAssert(it->second);
		GX_LoadTexObj(it->second, GX_TEXMAP0 + it->first);
	}

	// ModelView
	GX_LoadPosMtxImm(mModelViewMatrix, GX_PNMTX0);
	GX_LoadNrmMtxImm(mInverseModelViewMatrix, GX_PNMTX0);
	GX_SetCurrentMtx(GX_PNMTX0);

	for (unsigned int i = 0; i < mIndexCount; i += 3)
	{
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);

		GX_Position1x16(mIndexArray[i]);
		if (mTexCoordArray)
		{
			GX_TexCoord1x16(mIndexArray[i]);
			for (unsigned int ki = 1; ki < materialTextureUnits; ki++)
				GX_TexCoord1x16(mIndexArray[i]);
		}

		if (mNormalArray)
			GX_Normal1x16(mIndexArray[i]);

		GX_Position1x16(mIndexArray[i+1]);
		if (mTexCoordArray)
		{
			GX_TexCoord1x16(mIndexArray[i+1]);
			for (unsigned int ki = 1; ki < materialTextureUnits; ki++)
				GX_TexCoord1x16(mIndexArray[i+1]);
		}

		if (mNormalArray)
			GX_Normal1x16(mIndexArray[i+1]);

		GX_Position1x16(mIndexArray[i+2]);
		if (mTexCoordArray)
		{
			GX_TexCoord1x16(mIndexArray[i+2]);
			for (unsigned int ki = 1; ki < materialTextureUnits; ki++)
				GX_TexCoord1x16(mIndexArray[i+2]);
		}

		if (mNormalArray)
			GX_Normal1x16(mIndexArray[i+2]);

		GX_End();
	}
}

void wiiRenderSystem::screenshot(const char* filename)
{
	kAssert(filename);

	FILE* texFile = fopen(filename, "wb");
	if (!texFile)
	{
		S_LOG_INFO("Failed to create screenshot file " + std::string(filename));
		return;
	}

	int w = mVideoMode->fbWidth;
	int h = mVideoMode->efbHeight;

	char* textureData = (char*) memalign(32, w * h * 4);
	if (!textureData)
	{
		S_LOG_INFO("Failed to allocate screenshot memory");
		fclose(texFile);

		return;
	}

	GX_SetTexCopySrc(0, 0, w, h);
	GX_SetTexCopyDst(w, h, GX_TF_RGBA8, false);
	GX_CopyTex(textureData, false);
	GX_PixModeSync();

	// Write to file
	char* rgbTex = (char*) malloc(w * h * 3);
	if (!rgbTex)
	{
		free(textureData);
		fclose(texFile);

		S_LOG_INFO("Failed to allocate screenshot memory");
		return;
	}

	// Copy true rgb data
	/*
	char* finalWii = textureData;
	for (int y = 0; y < h; y += 4)
	{
		char* line = &rgbTex[y * w * 3];
		for (int x = 0; x < w; x += 8)
		{
			char* color = line + (x * 3);
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 4; tx++)
				{
					*color = finalWii[1];
					*(color + 1) = finalWii[32];
					*(color + 2) = finalWii[33];

					finalWii += 2;
					color += 3;
				}
				color += w * 3 - 12;
			}
			finalWii += 32;
		}
	}
	*/

	int x0, y0, ix0, iy0;
	int pitch = w<<1;
	short *d = (short*)textureData;

	for (y0 = 0; y0 < h; y0 += 4) 
	{
		for (x0 = 0; x0 < w; x0 += 4) 
		{
			for (iy0 = 0; iy0 < 4; iy0++) 
			{
				for (ix0 = 0; ix0 < 4; ix0++) 
				{
					int i = (y0*pitch)+(iy0<<2)+(x0<<3)+ix0;
					u32 k = (d[i]<<16)|d[i+16];
		
					rgbTex[(y0+iy0)*(w*3)+((x0+ix0)*3) + 0] = (k >> 16) & 0xff;
					rgbTex[(y0+iy0)*(w*3)+((x0+ix0)*3) + 1] = (k >> 8) & 0xff;
					rgbTex[(y0+iy0)*(w*3)+((x0+ix0)*3) + 2] = k & 0xff;
				}
			}
		}
	}


	/*
	short* p = (short*)textureData;

	int pitch = w << 1;
	int pitch2 = w * 3;

	for (short y = 0; y < h; y += 4) 
	for (short x = 0; x < w; x += 4) 
	for (short iy = 0; iy < 4; iy++) 
	for (short ix = 0; ix < 4; ix++) 
	{
		u32 c = (p[(y*pitch)+(x<<2)+(iy<<2)+((ix/4)*32)+(ix%4)]<<16)|p[(y*pitch)+(x<<2)+(iy<<2)+((ix/4)*32)+(ix%4)+16];
		rgbTex[(y+iy)*(pitch2)+((x+ix)<<2)] = (c >> 16) & 0xff;
		rgbTex[(y+iy)*(pitch2)+((x+ix)<<2) + 1] = (c >> 8) & 0xff;
		rgbTex[(y+iy)*(pitch2)+((x+ix)<<2) + 2] = c & 0xff;
	}
	*/

	// Compress the texture
	struct jpeg_compress_struct jInfo;
	struct jpeg_error_mgr jError;

	jInfo.err = jpeg_std_error(&jError);
	jpeg_create_compress(&jInfo);
	jpeg_stdio_dest(&jInfo, texFile);

	jInfo.image_width = w;
	jInfo.image_height = h;
	jInfo.input_components = 3;
	jInfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&jInfo);
	jpeg_set_quality(&jInfo, 100, true);
	jpeg_start_compress(&jInfo, true);

	while (jInfo.next_scanline < jInfo.image_height)
	{
		char* data = &rgbTex[w * 3 * jInfo.next_scanline];
		jpeg_write_scanlines(&jInfo, (JSAMPLE**)&data, 1);
	}

	jpeg_finish_compress(&jInfo);
	fclose(texFile);

	// Free Texture
	jpeg_destroy_compress(&jInfo);
	free(rgbTex);
	free(textureData);
}

unsigned int wiiRenderSystem::getScreenWidth()
{
	return mVideoMode->fbWidth;
}

unsigned int wiiRenderSystem::getScreenHeight()
{
	return mVideoMode->xfbHeight;
}
			
void wiiRenderSystem::copyBufferToTexture()
{
	GX_SetTexCopySrc(0, 0, getScreenWidth(), getScreenHeight());
	GX_SetTexCopyDst(mRttDimensions[0], mRttDimensions[1], GX_TF_RGBA8, GX_FALSE);
	GX_CopyTex(mRttBuffer, GX_FALSE);
	GX_PixModeSync();

	GX_InitTexObj(mRttTarget, mRttBuffer, mRttDimensions[0], mRttDimensions[1], 
				GX_TF_RGBA8, GX_REPEAT, GX_REPEAT, GX_FALSE);

	GX_InitTexObjLOD(mRttTarget, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, 0, 0, GX_ANISO_1);
	GX_InvalidateTexAll();
}

void wiiRenderSystem::copyToTexture(kTexture* tex)
{
	tex = mRttTarget;
	copyBufferToTexture();
}
			
void wiiRenderSystem::setTexEnv(const std::string& baseEnv, int stage)
{
	unsigned int mod = GX_REPLACE;
	if (baseEnv == "modulate")
		mod = GX_MODULATE;
	else
	if (baseEnv == "decal")
		mod = GX_DECAL;
	else
	if (baseEnv == "blend")
		mod = GX_BLEND;

	GX_SetTevOp(GX_TEVSTAGE0 + stage, mod);
}
		
void wiiRenderSystem::setTextureUnits(int i)
{
	GX_SetNumTevStages(i);
}
			
void wiiRenderSystem::setTextureGenerations(int i)
{
	GX_SetNumTexGens(i);

	if (i == 0)
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
}

void wiiRenderSystem::setColorChannels(int i)
{
	GX_SetNumChans(i);
}
			
void wiiRenderSystem::genTexture(uint32_t w, uint32_t h, uint32_t bpp, kTexture* tex)
{
	// For now, nothing
}

}

#endif

