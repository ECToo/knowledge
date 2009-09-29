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

#ifndef _WII_RENDERSYSTEM_
#define _WII_RENDERSYSTEM_
#ifdef __WII__

#include "rendersystem.h"

#define DEFAULT_FIFO_SIZE (512 * 1024)
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
			std::vector<color> mColors;
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

			/**
			 * The 8 supported lights.
			 */
			GXLightObj mLights[8];
			bool mEnabledLights;

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
			void showCursor(bool show) {}

			void frameStart();
			void frameEnd();
			void setWireFrame(bool wire);

			void setClearColor(const color& clr);
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
			void vcolor(const color& col);
			void texCoord(const vector2& coord);
			void endVertices();

			void matAmbient(const color& color);
			void matDiffuse(const color& color);
			void matSpecular(const color& color);

			void genTexture(uint32_t w, uint32_t h, uint32_t bpp, platformTexturePointer* tex);
			void bindTexture(platformTexturePointer* tex, int chan);
			void unBindTexture(int chan);

			/**
			 * Internal, copy to temp texture
			 */
			void copyBufferToTexture();
			void copyToTexture(platformTexturePointer* tex);

			void setBlendMode(unsigned short src, unsigned short dst);
			void setBlend(bool state);
			void drawArrays();
			void screenshot(const char* filename);

			bool isLightOn();
			void setLighting(bool status);
			void setLight(unsigned int i, bool status);
			void setLightPosition(unsigned int i, const vector3& p, bool directional);
			void setLightAmbient(unsigned int i, const color& a);
			void setLightSpecular(unsigned int i, const color& s);
			void setLightDiffuse(unsigned int i, const color& d);
			void setLightAttenuation(unsigned int i, const vector3& att);

			bool getPointSpriteSupport();
			float getPointSpriteMaxSize();
			void setPointSprite(bool enabled);
			void setPointSpriteSize(vec_t size);
			void drawPointSprites(const vec_t* positions, unsigned int numPositions);
			void setPointSpriteAttenuation(vec_t* att);

			bool getVBOSupport() { return false; }
			void genVBO(platformVBO* target) {}
			void bindVBO(platformVBO* target, VBOArrayType type = VBO_ARRAY) {}
			void setVBOData(VBOArrayType type, int size, void* data, VBOUsage use) {}
			void delVBO(platformVBO* target) {}

			unsigned int getScreenWidth();
			unsigned int getScreenHeight();
	};
}

#endif
#endif

