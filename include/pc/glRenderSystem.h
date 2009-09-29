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

#ifndef _GL_RENDERSYSTEM_
#ifndef __WII__
#define _GL_RENDERSYSTEM_

#include "rendersystem.h"
	
// SDL
#include "SDL.h"

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
			void showCursor(bool show);

			void frameStart();
			void frameEnd();

			void setClearColor(const color& clr);
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
			void setTexEnv(texEnvMode mode, int stage);
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
			void vcolor(const color& col);
			void texCoord(const vector2& coord);
			void endVertices();

			void matAmbient(const color& color);
			void matDiffuse(const color& color);
			void matSpecular(const color& color);

			void genTexture(uint32_t w, uint32_t h, uint32_t bpp, platformTexturePointer* tex);
			void bindTexture(GLuint* tex, int chan);
			void unBindTexture(int chan);

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

			bool getVBOSupport(); 
			void genVBO(platformVBO* target);
			void bindVBO(platformVBO* target, VBOArrayType type);
			void setVBOData(VBOArrayType type, int size, void* data, VBOUsage use);
			void delVBO(platformVBO* target);

			unsigned int getScreenWidth();
			unsigned int getScreenHeight();
	};
}

#endif
#endif

