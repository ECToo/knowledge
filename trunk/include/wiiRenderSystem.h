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

#define DEFAULT_FIFO_SIZE (1024*1024)

namespace k 
{
	class matrixStack
	{
		private:
			Mtx stack[32];
			unsigned int mPosition;

		public:
			matrixStack();

			void push(Mtx& matrix);
			void pop(Mtx& destination);
	};

	class matrix44Stack
	{
		private:
			Mtx44 stack[16];
			unsigned int mPosition;

		public:
			matrix44Stack();

			void push(Mtx44 matrix);
			void pop(Mtx44 destination);
	};

	class genericMesh
	{
		private:
			std::list<vector3> mVertices;
			std::vector<vector3> mNormals;
			std::vector<vector3> mColors;
			std::vector<vector2> mTexCoords;

			VertexMode mRenderingMode;

		public:
			void initialise(VertexMode vMode);

			/**
			 * Note that this function doesnt
			 * support multi textures neither
			 * multi pass for rendering yet.
			 */
			void end(Mtx& mModelViewMatrix, std::map<int, GXTexObj*>* mActiveTextures,
					material* mat);

			void pushVertex(const vector3& vec);
			void pushNormal(const vector3& vec);
			void pushColor(const vector3& vec);
			void pushTexCoord(const vector2& vec);
	};

	class wiiRenderSystem : public renderSystem
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
			genericMesh mGenericMesh;

			/**
			 * Active texture unit, if any
			 */
			std::map<int, GXTexObj*> mActiveTextures;

		public:
			wiiRenderSystem();
			~wiiRenderSystem();

			void initialise();
			void deinitialise();
			void configure();

			void createWindow(const int w, const int h);
			void destroyWindow();
			void setWindowTitle(const std::string& title) = 0;

			void frameStart();
			void frameEnd();

			void setClearColor(const vector3& color);
			void setClearDepth(const vec_t amount);
			void setDepthTest(bool test);
			void setDepthMask(bool test);

			void setShadeModel(ShadeModel model);

			void setMatrixMode(MatrixMode mode);
			void pushMatrix();
			void popMatrix();
			void identityMatrix();
			void copyMatrix(f32 matrix[][4]);
			void multMatrix(f32 matrix[][4]);
			void getModelView(Mtx matrix);

			void translateScene(vec_t x, vec_t y, vec_t z);
			void rotateScene(vec_t angle, vec_t x, vec_t y, vec_t z);
			void scaleScene(vec_t x, vec_t y, vec_t z);

			void setViewPort(int x, int y, int w, int h);
			void setPerspective(vec_t fov, vec_t aspect, vec_t near, vec_t far);
			void setOrthographic(vec_t left, vec_t right, vec_t bottom, vec_t top, vec_t near, vec_t far);

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

			void bindTexture(GXTexObj* tex, int chan);

			void setBlendMode(unsigned short src, unsigned short dst);
			void setBlend(bool state);
			void drawArrays();

			unsigned int getScreenWidth();
			unsigned int getScreenHeight();
	};
}

#endif
#endif

