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

#ifndef _WII_RENDERSYSTEM_
#define _WII_RENDERSYSTEM_
#ifdef __WII__

#include "rendersystem.h"

#define DEFAULT_FIFO_SIZE (4 * 1024*1024)
#define MAX_WII_TEXTURES 8

namespace k 
{
	class DLL_EXPORT matrixStack
	{
		private:
			Mtx stack[32];
			unsigned int mPosition;

		public:
			matrixStack();

			void push(Mtx& matrix);
			void pop(Mtx& destination);
	};

	class DLL_EXPORT matrix44Stack
	{
		private:
			Mtx44 stack[16];
			unsigned int mPosition;

		public:
			matrix44Stack();

			void push(Mtx44 matrix);
			void pop(Mtx44 destination);
	};

	class DLL_EXPORT wiiRenderSystem : public renderSystem
	{
		private:
			/**
			 * Front and Back render buffers
			 */
			void* mFrameBuffers[2];
			
			/**
			 * Actual buffer index we are rendering to
			 */
			unsigned int mBufferIndex;

			/**
			 * The FIFO area
			 */
			void* mFifo;

			/**
			 * mode used to drive resolutions and configurations
			 * about the video
			 */
			GXRModeObj* mVideoMode;

			/**
			 * Clear Screen Color
			 */
			GXColor mClearColor;
			u32 mClearDepth;
			
			/**
			 * Depth testing
			 */
			bool mDepthTest;

			/**
			 * Depth masking 
			 */
			bool mDepthMask;

			/**
			 * Since the wii doesn't have a matrix stack by itself
			 * we are going to build one for each type, MODELVIEW, PROJECTION and TEXTURE
			 * The size of the stacks are defined following openGL api
			 */
			matrixStack mModelViewStack;
			matrix44Stack mProjectionStack;
			matrixStack mTextureStack;
			
			/**
			 * Default matrices
			 */
			Mtx mModelViewMatrix;
			Mtx mInverseModelViewMatrix;
			Mtx44 mProjectionMatrix;

			/**
			 * Active matrix mode
			 */
			MatrixMode mActiveMatrix;

			/**
			 * This concat and transform the matrices
			 * to make the final product =]
			 */
			void prepareFinalMatrix();

			/**
			 * Since we have to count the vertices
			 * and adjust everything, better to use
			 * a generic mesh to do the job for us
			 */
			std::list<vector3> mVertices;
			std::vector<vector3> mNormals;
			std::vector<vector3> mColors;
			std::vector<vector2> mTexCoords;
			VertexMode mRenderingMode;

			/**
			 * Active texture unit, if any
			 */
			GXTexObj* mActiveTextures[MAX_WII_TEXTURES];

			/**
			 * Render To Texture buffer
			 */
			char* mRttBuffer;

			void _cleanTextures();

		public:
			wiiRenderSystem();
			~wiiRenderSystem();

			void initialize();
			void deinitialize();
			void configure();

			void createWindow(const int w, const int h);
			void destroyWindow();
			void setWindowTitle(const std::string& title);

			void frameStart();
			void frameEnd();
			void setWireFrame(bool wire);

			void setClearColor(const vector3& color);
			void setClearDepth(const vec_t amount);
			void setDepthTest(bool test);
			void setDepthMask(bool test);

			void setShadeModel(ShadeModel model);

			void setMatrixMode(MatrixMode mode);
			void pushMatrix();
			void popMatrix();
			void identityMatrix();

			void setTexEnv(const std::string& baseEnv, int stage);
			void setTexEnv(texEnvMode mode, int stage);
			void setTextureUnits(int i);
			void setTextureGenerations(int i);
			void setColorChannels(int i);
			void setInverseTransposeModelview(const matrix4& mat);

			void copyMatrix(const matrix4& mat);
			void multMatrix(const matrix4& mat);

			matrix4 getModelView();
			void getModelView(float mat[][4]);

			void translateScene(vec_t x, vec_t y, vec_t z);
			void rotateScene(vec_t angle, vec_t x, vec_t y, vec_t z);
			void scaleScene(vec_t x, vec_t y, vec_t z);

			void setViewPort(int x, int y, int w, int h);
			void setPerspective(vec_t fov, vec_t aspect, vec_t nearP, vec_t farP);
			void setOrthographic(vec_t left, vec_t right, vec_t bottom, vec_t top, vec_t nearP, vec_t farP);

			void setCulling(CullMode culling);

			void startVertices(VertexMode mode);
			void vertex(const vector3& vert);
			void normal(const vector3& norm);
			void color(const vector3& col);
			void texCoord(const vector2& coord);
			void endVertices();

			void matAmbient(const vector3& color);
			void matDiffuse(const vector3& color);
			void matSpecular(const vector3& color);

			void genTexture(uint32_t w, uint32_t h, uint32_t bpp, kTexture* tex);
			void bindTexture(kTexture* tex, int chan);
			void unBindTexture(int chan);

			/**
			 * Internal, copy to temp texture
			 */
			void copyBufferToTexture();
			void copyToTexture(kTexture* tex);

			void setBlendMode(unsigned short src, unsigned short dst);
			void setBlend(bool state);
			void drawArrays();
			void screenshot(const char* filename);

			bool getVBOSupport() { return false; }
			void genVBO(kVBO* target) {}
			void bindVBO(kVBO* target, VBOArrayType type = VBO_ARRAY) {}
			void setVBOData(VBOArrayType type, int size, void* data, VBOUsage use) {}
			void delVBO(kVBO* target) {}

			unsigned int getScreenWidth();
			unsigned int getScreenHeight();
	};
}

#endif
#endif

