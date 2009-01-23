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
#include "quaternion.h"
#include "material.h"
#include "logger.h"

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

	enum VertexMode
	{
		VERTEXMODE_LINE,
		VERTEXMODE_TRIANGLES,
		VERTEXMODE_TRI_STRIP,
		VERTEXMODE_QUAD
	};

	class renderSystem
	{
		protected:
			/**
			 * For array drawing
			 */
			unsigned int mVertexCount;
			vec_t* mVertexArray;

			vec_t* mTexCoordArray;
			vec_t* mNormalArray;

			unsigned int mIndexCount;
			index_t* mIndexArray;

			material* mActiveMaterial;

			// Only Flush
			bool mOnlyFlush;

		public:
			
			/**
			 * Memory allocation, configurations set
			 */
			virtual void initialise() = 0;
			virtual void deinitialise() = 0;
			virtual void configure() = 0;

			virtual void createWindow(const int w, const int h) = 0;
			virtual void destroyWindow() = 0;
			virtual void setWindowTitle(const std::string& title) = 0;

			virtual void frameStart() = 0;
			virtual void frameEnd() = 0;
			virtual void setWireFrame(bool wire) = 0;

			virtual void setOnlyFlush(bool flush)
			{
				mOnlyFlush = flush;
			}

			virtual void setClearColor(const vector3& color) = 0;
			virtual void setClearDepth(const vec_t amount) = 0;
			virtual void setDepthTest(bool test) = 0;
			virtual void setDepthMask(bool mask) = 0;

			virtual void setShadeModel(ShadeModel model) = 0;

			virtual void setMatrixMode(MatrixMode mode) = 0;
			virtual void pushMatrix() = 0;
			virtual void popMatrix() = 0;
			virtual void identityMatrix() = 0;

			#ifndef __WII__
			virtual void copyMatrix(vec_t* matrix) = 0;
			virtual void multMatrix(vec_t* matrix) = 0;
			virtual vec_t* getModelView() = 0;
			#else
			virtual void copyMatrix(Mtx44 matrix) = 0;
			virtual void multMatrix(Mtx44 matrix) = 0;
			virtual void getModelView(Mtx matrix) = 0;
			#endif

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

			void bindMaterial(material* mat)
			{
				mActiveMaterial = mat;
			}

			virtual void genTexture(unsigned int w, unsigned int h, unsigned int bpp, kTexture* tex) = 0;
			virtual void bindTexture(kTexture* tex, int chan) = 0;
			virtual void unBindTexture(int chan) = 0;

			virtual void setBlendMode(unsigned short src, unsigned short dst) = 0;
			virtual void setBlend(bool state) = 0;

			virtual void copyToTexture(unsigned int w, unsigned int h, kTexture* tex) = 0;

			virtual void clearArrayDesc()
			{
				mVertexCount = 0;
				mVertexArray = NULL;

				mTexCoordArray = NULL;
				mNormalArray = NULL;

				mIndexCount = 0;
				mIndexArray = NULL;
			}

			virtual void setVertexArray(vec_t* vertices)
			{
				assert(vertices != NULL);
				mVertexArray = vertices;
			}

			virtual void setVertexCount(unsigned int count)
			{
				assert(count != 0);
				mVertexCount = count;
			}

			virtual void setTexCoordArray(vec_t* coords)
			{
				assert(coords != NULL);
				mTexCoordArray = coords;
			}

			virtual void setNormalArray(vec_t* normals)
			{
				assert(normals != NULL);
				mNormalArray = normals;
			}

			virtual void setVertexIndex(index_t* indexes)
			{
				assert(indexes != NULL);
				mIndexArray = indexes;
			}

			virtual void setIndexCount(unsigned int count)
			{
				assert(count != 0);
				mIndexCount = count;
			}

			virtual void drawArrays() = 0;
			virtual void screenshot(const char* filename) = 0;

			virtual unsigned int getScreenWidth() = 0;
			virtual unsigned int getScreenHeight() = 0;
	};
}

#endif

