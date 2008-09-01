#ifndef _GL_RENDERSYSTEM_
#ifndef __WII__
#define _GL_RENDERSYSTEM_

#include "rendersystem.h"

namespace k 
{
	class glRenderSystem : public renderSystem
	{
		private:
			vector2 mScreenSize;
			SDL_Surface* mScreen;

		public:
			glRenderSystem();
			~glRenderSystem();

			void initialise();
			void deinitialise();
			void configure();

			void createWindow(const int w, const int h);
			void destroyWindow();

			void frameStart();
			void frameEnd();

			void setClearColor(const vector3& color);
			void setClearDepth(const vec_t amount);
			void setDepthTest(bool test);

			void setShadeModel(ShadeModel model);

			void setMatrixMode(MatrixMode mode);
			void pushMatrix();
			void popMatrix();
			void identityMatrix();

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

			void bindTexture(GLuint tex);

			unsigned int getScreenWidth();
			unsigned int getScreenHeight();
	};
}

#endif
#endif

