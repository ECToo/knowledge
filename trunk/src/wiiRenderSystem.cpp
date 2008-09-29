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

#include "wiiRenderSystem.h"
#include "root.h"
#include "logger.h"

namespace k {

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
		guMtxIdentity(stack[i]);
	}

	// Top of the stack
	mPosition = 16;
}

void matrix44Stack::push(Mtx44 matrix)
{
	if (mPosition <= 0)
		return;

	guMtxCopy(matrix, stack[--mPosition]);
}

void matrix44Stack::pop(Mtx44 destination)
{
	if (mPosition < 32)
	{
		guMtxCopy(stack[mPosition++], destination);
	}
}

void genericMesh::initialise(VertexMode vMode)
{
	mRenderingMode = vMode;

	mVertices.clear();
	mNormals.clear();
	mColors.clear();
	mTexCoords.clear();
}

void genericMesh::end(Mtx& mModelViewMatrix, GXTexObj* mActiveTexture)
{
	unsigned int mVertexSize = mVertices.size();

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);

	u8 tevStage = GX_TEVSTAGE0;
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
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

		texCoord = GX_TEXCOORD0;
		texMap = GX_TEXMAP0;
	}
	else
	{
		GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
	}

	GX_SetNumChans(1);

	if (mActiveTexture)
	{
		GX_SetNumTevStages(1);
		GX_SetNumTexGens(1);
		GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	}
	else
	{
		GX_SetNumTevStages(1);
		GX_SetNumTexGens(0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	}

	GX_SetTevOrder(tevStage, texCoord, texMap, tevColor);

	// Bind the texture
	if (mActiveTexture);
	{
		GX_LoadTexObj(mActiveTexture, GX_TEXMAP0);
	}

	// ModelView
	GX_LoadPosMtxImm(mModelViewMatrix, GX_PNMTX0);

	// Start the Vertex Descriptor on Flipper
	switch (mRenderingMode)
	{
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
		}

		// Increment index
		index++;
	}

	GX_End();
}

void genericMesh::pushVertex(const vector3& vec)
{
	mVertices.push_back(vec);
}

void genericMesh::pushNormal(const vector3& vec)
{
	mNormals.push_back(vec);
}

void genericMesh::pushColor(const vector3& vec)
{
	mColors.push_back(vec);
}

void genericMesh::pushTexCoord(const vector2& vec)
{
	mTexCoords.push_back(vec);
}
			
wiiRenderSystem::wiiRenderSystem()
{
}

wiiRenderSystem::~wiiRenderSystem()
{
	deinitialise();
}

void wiiRenderSystem::initialise()
{
	VIDEO_Init();
}

void wiiRenderSystem::deinitialise()
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
	VIDEO_Flush();
	VIDEO_WaitVSync();

	if (mVideoMode->viTVMode & VI_NON_INTERLACE) 
		VIDEO_WaitVSync();

	// Allocate the FIFO
	mFifo = memalign(32, DEFAULT_FIFO_SIZE);
	memset(mFifo, 0, DEFAULT_FIFO_SIZE);

	// Initialise GX
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

	setPerspective(90, (float)getScreenWidth()/getScreenHeight(), 0.1f, 5000.0f);
}

void wiiRenderSystem::createWindow(const int w, const int h)
{
	// Doesnt make sense on wii ;)
}
			
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

void wiiRenderSystem::destroyWindow()
{
	// Doesnt make sense on wii ;)
}

void wiiRenderSystem::frameStart()
{
	GX_SetViewport(0, 0, mVideoMode->fbWidth, mVideoMode->efbHeight, 0, 1);
	GX_InvVtxCache();
	GX_InvalidateTexAll();
}

void wiiRenderSystem::frameEnd()
{
	GX_DrawDone();
		
	// Flip Buffers
	mBufferIndex ^= 1;
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(mFrameBuffers[mBufferIndex], GX_TRUE);

	VIDEO_SetNextFramebuffer(mFrameBuffers[mBufferIndex]);
 	VIDEO_Flush();
	VIDEO_WaitVSync();
}

void wiiRenderSystem::setClearColor(const vector3& color)
{
	mClearColor.r = color.x*255;
	mClearColor.g = color.y*255;
	mClearColor.b = color.z*255;
	mClearColor.a = 255;
}

void wiiRenderSystem::setClearDepth(const vec_t amount)
{
	mClearDepth = amount;
}

void wiiRenderSystem::setDepthTest(bool test)
{
	// TODO
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
			guMtxIdentity(mProjectionMatrix);
			break;
		case MATRIXMODE_MODELVIEW:
			guMtxIdentity(mModelViewMatrix);
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::copyMatrix(Mtx44 matrix)
{
	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			guMtxCopy(matrix, mProjectionMatrix);
			break;
		case MATRIXMODE_MODELVIEW:
			guMtxCopy(matrix, mModelViewMatrix);
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::translateScene(vec_t x, vec_t y, vec_t z)
{
	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			{
				Mtx44 temp;

				guMtxIdentity(temp);
				guMtxTrans(temp, x, y, z);
				guMtxConcat(mProjectionMatrix, temp, mProjectionMatrix);
			}
			break;
		case MATRIXMODE_MODELVIEW:
			{
				Mtx temp;

				guMtxIdentity(temp);
				guMtxTrans(temp, x, y, z);
				guMtxConcat(mModelViewMatrix, temp, mModelViewMatrix);
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
	
	axis.x = x;
	axis.y = y;
	axis.z = z;

	guMtxIdentity(temp);
	guMtxRotAxisDeg(temp, &axis, angle);

	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			// guMtxConcat(mProjectionMatrix, temp, mProjectionMatrix);
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
}

void wiiRenderSystem::setPerspective(vec_t fov, vec_t aspect, vec_t near, vec_t far)
{
	Mtx44 perspective;

	guMtxIdentity(perspective);
	guPerspective(perspective, fov, aspect, near, far);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);

	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			guMtxConcat(mProjectionMatrix, perspective, mProjectionMatrix);
			break;
		default:
			S_LOG_INFO("Invalid matrix mode");
			break;
	}
}

void wiiRenderSystem::setOrthographic(vec_t left, vec_t right, vec_t bottom, vec_t top, vec_t near, vec_t far)
{
	Mtx44 ortho;

	guMtxIdentity(ortho);
	guOrtho(ortho, left, right, bottom, top, near, far);
	GX_LoadProjectionMtx(ortho, GX_ORTHOGRAPHIC);

	switch (mActiveMatrix)
	{
		case MATRIXMODE_PROJECTION:
			guMtxConcat(mProjectionMatrix, ortho, mProjectionMatrix);
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
			GX_SetCullMode(GX_CULL_BACK);
			break;
		case CULLMODE_FRONT:
			GX_SetCullMode(GX_CULL_FRONT);
			break;
		case CULLMODE_BOTH:
			GX_SetCullMode(GX_CULL_ALL);
			break;
	};
}

void wiiRenderSystem::startVertices(VertexMode mode)
{
	mGenericMesh.initialise(mode);
}

void wiiRenderSystem::vertex(const vector3& vert)
{
	mGenericMesh.pushVertex(vert);
}

void wiiRenderSystem::normal(const vector3& norm)
{
	mGenericMesh.pushNormal(norm);
}

void wiiRenderSystem::color(const vector3& col)
{
	mGenericMesh.pushColor(col);
}

void wiiRenderSystem::texCoord(const vector2& coord)
{
	mGenericMesh.pushTexCoord(coord);
}

void wiiRenderSystem::endVertices()
{
	mGenericMesh.end(mModelViewMatrix, mActiveTexture);
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

void wiiRenderSystem::bindTexture(GXTexObj* tex)
{
	// TODO: Multipass
	mActiveTexture = tex;
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

	DCFlushRange(mVertexArray, mVertexCount * sizeof(vec_t) * 3);
	GX_SetArray(GX_VA_POS, mVertexArray, 3 * sizeof(vec_t));

	u8 tevStage = GX_TEVSTAGE0;
	u8 texCoord = GX_TEXCOORDNULL;
	u32 texMap = GX_TEXMAP_NULL;
	u8 tevColor = GX_COLORNULL;

	if (mTexCoordArray)
	{
 		GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);
		GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

		DCFlushRange(mTexCoordArray, mVertexCount * sizeof(vec_t) * 2);
		GX_SetArray(GX_VA_TEX0, mTexCoordArray, 2 * sizeof(vec_t));

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

		DCFlushRange(mNormalArray, mVertexCount * sizeof(vec_t) * 3);
		GX_SetArray(GX_VA_NRM, mNormalArray, 3 * sizeof(vec_t));
	}

	GX_SetNumChans(1);

	if (mActiveTexture)
	{
		GX_SetNumTevStages(1);
		GX_SetNumTexGens(1);
		GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	}
	else
	{
		GX_SetNumTevStages(1);
		GX_SetNumTexGens(0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	}

	GX_SetTevOrder(tevStage, texCoord, texMap, tevColor);

	// Bind the texture
	if (mActiveTexture);
	{
		GX_LoadTexObj(mActiveTexture, GX_TEXMAP0);
	}

	// ModelView
	GX_LoadPosMtxImm(mModelViewMatrix, GX_PNMTX0);

	for (unsigned int i = 0; i < mIndexCount; i += 3)
	{
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);

		GX_Position1x16(mIndexArray[i]);
		if (mTexCoordArray)
			GX_TexCoord1x16(mIndexArray[i]);
		if (mNormalArray)
			GX_Normal1x16(mIndexArray[i]);

		GX_Position1x16(mIndexArray[i+1]);
		if (mTexCoordArray)
			GX_TexCoord1x16(mIndexArray[i+1]);
		if (mNormalArray)
			GX_Normal1x16(mIndexArray[i+1]);

		GX_Position1x16(mIndexArray[i+2]);
		if (mTexCoordArray)
			GX_TexCoord1x16(mIndexArray[i+2]);
		if (mNormalArray)
			GX_Normal1x16(mIndexArray[i+2]);

		GX_End();
	}
}

unsigned int wiiRenderSystem::getScreenWidth()
{
	// return mVideoMode->viWidth;
	return mVideoMode->fbWidth;
}

unsigned int wiiRenderSystem::getScreenHeight()
{
	// return mVideoMode->viHeight;
	return mVideoMode->efbHeight;
}

camera::camera()
{
	guMtxIdentity(mMatrix);
}

void camera::setView()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->identityMatrix();
	rs->copyMatrix(mMatrix);
}

void camera::setPosition(vector3 pos)
{
	mMatrix[3][0] = pos.x;
	mMatrix[3][1] = pos.y;
	mMatrix[3][2] = pos.z;
}

vector3 camera::getPosition()
{
	return vector3(mMatrix[3][0], mMatrix[3][1], mMatrix[3][2]);
}

}
