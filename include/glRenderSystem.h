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

#ifndef _GL_RENDERSYSTEM_
#ifndef __WII__
#define _GL_RENDERSYSTEM_

#include "rendersystem.h"

namespace k 
{
	class DLL_EXPORT glRenderSystem : public renderSystem
	{
		private:
			uint32_t mScreenSize[2];
			SDL_Surface* mScreen;

			/**
			 * Screenshot ;)
			 */
			GLuint mScreenshotTex;

		public:
			glRenderSystem();
			~glRenderSystem();

			void initialize();
			void deinitialize();
			void configure();

			void createWindow(const int w, const int h);
			void destroyWindow();
			void setWindowTitle(const std::string& title);

			void frameStart();
			void frameEnd();

			void setClearColor(const vector3& color);
			void setClearDepth(const vec_t amount);
			void setDepthTest(bool test);
			void setDepthMask(bool test);

			void setShadeModel(ShadeModel model);
			void setWireFrame(bool wire);

			void setMatrixMode(MatrixMode mode);
			void pushMatrix();
			void popMatrix();
			void identityMatrix();

			void setTexEnv(const std::string& baseEnv, int stage);
			void setTextureUnits(int i) {}
			void setTextureGenerations(int i) {}
			void setColorChannels(int i) {}

			void setInverseTransposeModelview(const matrix4& mat) {}

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
			void bindTexture(GLuint* tex, int chan);
			void unBindTexture(int chan);

			void copyToTexture(kTexture* tex);

			void setBlendMode(unsigned short src, unsigned short dst);
			void setBlend(bool state);
			void drawArrays();
			void screenshot(const char* filename);

			bool getVBOSupport(); 
			void genVBO(kVBO* target);
			void bindVBO(kVBO* target, VBOArrayType type);
			void setVBOData(VBOArrayType type, int size, void* data, VBOUsage use);
			void delVBO(kVBO* target);

			unsigned int getScreenWidth();
			unsigned int getScreenHeight();
	};
}

#endif
#endif

