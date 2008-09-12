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

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"

namespace k 
{
	enum ShadeModel
	{
		SHADEMODEL_FLAT,
		SHADEMODEL_SMOOTH
	};

	enum MatrixMode
	{
		MATRIXMODE_PROJECTION,
		MATRIXMODE_MODELVIEW
	};

	enum CullMode
	{
		CULLMODE_NONE,
		CULLMODE_BACK,
		CULLMODE_FRONT,
		CULLMODE_BOTH
	};

	enum VertexMode
	{
		VERTEXMODE_TRIANGLES,
		VERTEXMODE_QUAD
	};

	class renderSystem
	{
		protected:
			/**
			 * For array drawing
			 */
			unsigned int mVertexArraySize;
			unsigned int mVertexStride;
			vec_t* mVertexArray;

			unsigned int mTexCoordStride;
			vec_t* mTexCoordArray;

			vec_t* mNormalArray;

			unsigned int mIndexArraySize;
			index_t* mIndexArray;

		public:
			
			/**
			 * Memory allocation, configurations set
			 */
			virtual void initialise() = 0;
			virtual void deinitialise() = 0;
			virtual void configure() = 0;

			virtual void createWindow(const int w, const int h) = 0;
			virtual void destroyWindow() = 0;

			virtual void frameStart() = 0;
			virtual void frameEnd() = 0;

			virtual void setClearColor(const vector3& color) = 0;
			virtual void setClearDepth(const vec_t amount) = 0;
			virtual void setDepthTest(bool test) = 0;

			virtual void setShadeModel(ShadeModel model) = 0;

			virtual void setMatrixMode(MatrixMode mode) = 0;
			virtual void pushMatrix() = 0;
			virtual void popMatrix() = 0;
			virtual void identityMatrix() = 0;

			virtual void translateScene(vec_t x, vec_t y, vec_t z) = 0;
			virtual void rotateScene(vec_t angle, vec_t x, vec_t y, vec_t z) = 0;
			virtual void scaleScene(vec_t x, vec_t y, vec_t z) = 0;

			virtual void setViewPort(int x, int y, int w, int h) = 0;
			virtual void setPerspective(vec_t fov, vec_t aspect, vec_t near, vec_t far) = 0;
			virtual void setOrthographic(vec_t left, vec_t right, vec_t bottom, vec_t top, vec_t near, vec_t far) = 0;

			virtual void setCulling(CullMode culling) = 0;

			virtual void startVertices(VertexMode mode) = 0;
			virtual void vertex(const vector3& vert) = 0;
			virtual void normal(const vector3& norm) = 0;
			virtual void color(const vector3& col) = 0;
			virtual void texCoord(const vector2& coord) = 0;
			virtual void endVertices() = 0;

			virtual void matAmbient(const vector3& ambient) = 0;
			virtual void matDiffuse(const vector3& diffuse) = 0;
			virtual void matSpecular(const vector3& specular) = 0;

			#ifndef __WII__
			virtual void bindTexture(GLuint tex) = 0;
			#else
			virtual void bindTexture(GXTexObj* tex) = 0;
			#endif

			virtual void setVertexArray(vec_t* vertices, unsigned int stride, unsigned int vertNum)
			{
				assert(vertices != NULL);

				mVertexArraySize = vertNum;
				mVertexArray = vertices;
				mVertexStride = stride;
			}

			virtual void setTexCoordArray(vec_t* coords, unsigned int stride)
			{
				assert(coords != NULL);

				mTexCoordArray = coords;
				mTexCoordStride = stride;
			}

			virtual void setNormalArray(vec_t* normals)
			{
				assert(normals != NULL);
				mNormalArray = normals;
			}

			virtual void setVertexIndex(index_t* indexes, unsigned int size)
			{
				assert(indexes != NULL);

				mIndexArray = indexes;
				mIndexArraySize = size;
			}

			virtual void drawArrays() = 0;

			virtual unsigned int getScreenWidth() = 0;
			virtual unsigned int getScreenHeight() = 0;
	};
}

#endif

