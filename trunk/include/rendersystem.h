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

	enum GammaModes
	{
		GAMMA_1_0,
		GAMMA_1_7,
		GAMMA_2_2
	};

	enum VBOArrayType
	{
		VBO_ARRAY,
		VBO_ELEMENT_ARRAY
	};

	enum VBOUsage
	{
		VBO_STATIC_DRAW,
		VBO_STATIC_READ,
		VBO_STATIC_COPY,
		VBO_DYNAMIC_DRAW,
		VBO_DYNAMIC_READ,
		VBO_DYNAMIC_COPY,
		VBO_STREAM_DRAW,
		VBO_STREAM_READ,
		VBO_STREAM_COPY
	};

	enum texEnvMode
	{
		TEX_ENV_MODULATE,
		TEX_ENV_REPLACE,
		TEX_ENV_DECAL,
		TEX_ENV_BLEND,
		TEX_ENV_ADD
	};

	#define MAX_TEXCOORD 8
	class DLL_EXPORT renderSystem
	{
		protected:
			/**
			 * For array drawing
			 */
			unsigned int mVertexCount;
			const vec_t* mVertexArray;

			const vec_t* mTexCoordArray[MAX_TEXCOORD];
			const vec_t* mNormalArray;

			VertexMode mIndexDrawMode;
			unsigned int mIndexCount;
			const index_t* mIndexArray;

			unsigned int mVertexStride;
			unsigned int mTexCoordStride[MAX_TEXCOORD];
			unsigned int mNormalStride;

			material* mActiveMaterial;

			// Render To Texture
			bool mRenderToTexture;
			unsigned int mRttDimensions[2];
			kTexture* mRttTarget;

			// Vertex Buffer Object
			bool mUsingVBO;
			int mVertexOffset;
			int mNormalOffset;
			int mIndexOffset;
			int mTexCoordOffset[MAX_TEXCOORD];

		public:
			
			/**
			 * Memory allocation, configurations set
			 */
			virtual void initialize() = 0;
			virtual void deinitialize() = 0;
			virtual void configure() = 0;

			virtual void createWindow(const int w, const int h) = 0;
			virtual void destroyWindow() = 0;
			virtual void setWindowTitle(const std::string& title) = 0;

			virtual void frameStart() = 0;
			virtual void frameEnd() = 0;
			virtual void setWireFrame(bool wire) = 0;

			virtual void setRttTarget(kTexture* tex)
			{
				kAssert(tex);
				mRttTarget = tex;
			}
			
			virtual void setRttSize(unsigned int width, unsigned int height)
			{
				mRttDimensions[0] = width;
				mRttDimensions[1] = height;
			}

			virtual void setRenderToTexture(bool rtt)
			{
				mRenderToTexture = rtt;
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

			/**
			 * Set Texture environment (base one like replace, modulate..)
			 * on the following texture unit/tev stage
			 */
			virtual void setTexEnv(const std::string& baseEnv, int stage) = 0;

			/**
			 * Set base texture environment modes.
			 */
			virtual void setTexEnv(texEnvMode mode, int stage) = 0;
			
			/**
			 * Only required on wii, to set the number of tev stages.
			 */
			virtual void setTextureUnits(int i) = 0;

			/**
			 * Only required on wii, to set the number of texgens.
			 */
			virtual void setTextureGenerations(int i) = 0;
			
			/**
			 * Only required on wii, to set the number of color channels (output).
			 */
			virtual void setColorChannels(int i) = 0;

			// Should be valid on wii and another platforms
			// wich needs a inverse transposed modelview
			virtual void setInverseTransposeModelview(const matrix4& mat) = 0;

			virtual void copyMatrix(const matrix4& mat) = 0;
			virtual void multMatrix(const matrix4& mat) = 0;

			virtual matrix4 getModelView() = 0;
			virtual void getModelView(float mat[][4]) = 0;

			virtual void translateScene(vec_t x, vec_t y, vec_t z) = 0;
			virtual void rotateScene(vec_t angle, vec_t x, vec_t y, vec_t z) = 0;
			virtual void scaleScene(vec_t x, vec_t y, vec_t z) = 0;

			virtual void setViewPort(int x, int y, int w, int h) = 0;
			virtual void setPerspective(vec_t fov, vec_t aspect, vec_t nearP, vec_t farP) = 0;
			virtual void setOrthographic(vec_t left, vec_t right, vec_t bottom, vec_t top, vec_t nearP, vec_t farP) = 0;

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

			virtual void copyToTexture(kTexture* tex) = 0;

			virtual void clearArrayDesc(VertexMode drawMode = VERTEXMODE_TRIANGLES)
			{
				mVertexCount = 0;
				mVertexArray = NULL;

				mVertexStride = 0;
				mNormalStride = 0;

				mIndexDrawMode = drawMode;

				mNormalArray = NULL;

				for (int i = 0; i < MAX_TEXCOORD; i++)
				{
					mTexCoordArray[i] = NULL;
					mTexCoordStride[i] = 0;
				}

				mIndexCount = 0;
				mIndexArray = NULL;
				mUsingVBO = false;
			}

			virtual bool setVBO(bool enabled)
			{
				mUsingVBO = enabled;
				if (enabled)
				{
					if (getVBOSupport())
					{
						mVertexOffset = -1;
						mNormalOffset = -1;
						mIndexOffset = -1;

						for (int i = 0; i < MAX_TEXCOORD; i++)
							mTexCoordOffset[i] = -1;
					}

					return getVBOSupport();
				}
				else
					return false;
			}

			virtual void setVertexArray(const vec_t* vertices, unsigned int stride = 0)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				#ifdef _FORCE_32_ALIGNMENT_
				kAssert(!(reinterpret_cast<uintptr_t>(vertices) & 0x1F));
				#endif

				kAssert(vertices);

				mVertexArray = vertices;
				mVertexStride = stride;
			}

			virtual void setVertexArray(const unsigned int vertices, unsigned int stride = 0)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				if (mUsingVBO)
				{
					mVertexStride = stride;
					mVertexOffset = vertices;
				}
			}

			virtual void setVertexCount(unsigned int count)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				kAssert(count);
				mVertexCount = count;
			}

			virtual void setTexCoordArray(const vec_t* coords, unsigned int stride = 0, int slot = 0)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				#ifdef _FORCE_32_ALIGNMENT_
				kAssert(!(reinterpret_cast<uintptr_t>(coords) & 0x1F));
				#endif

				kAssert(coords);

				if (slot > MAX_TEXCOORD)
				{
					S_LOG_INFO("Texture coordinate array slot can't be greater than 8, fallbacking to 0.");
					slot = 0;
				}

				mTexCoordArray[slot] = coords;
				mTexCoordStride[slot] = stride;
			}

			virtual void setTexCoordArray(const unsigned int coords, unsigned int stride = 0, int slot = 0)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				// coords is an offset in VBO
				if (slot > MAX_TEXCOORD)
				{
					S_LOG_INFO("Texture coordinate array slot can't be greater than 8, fallbacking to 0.");
					slot = 0;
				}

				if (mUsingVBO)
				{
					mTexCoordOffset[slot] = coords;
					mTexCoordStride[slot] = stride;
				}
			}

			virtual void setNormalArray(const vec_t* normals, unsigned int stride = 0)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				#ifdef _FORCE_32_ALIGNMENT_
				kAssert(!(reinterpret_cast<uintptr_t>(normals) & 0x1F));
				#endif

				kAssert(normals);

				mNormalArray = normals;
				mNormalStride = stride;
			}

			virtual void setNormalArray(const unsigned int normals, unsigned int stride = 0)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				if (mUsingVBO)
				{
					mNormalOffset = normals;
					mNormalStride = stride;
				}
			}

			virtual void setVertexIndex(const index_t* indexes)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				#ifdef _FORCE_32_ALIGNMENT_
				kAssert(!(reinterpret_cast<uintptr_t>(indexes) & 0x1F));
				#endif

				kAssert(indexes);

				mIndexArray = indexes;
			}

			virtual void setVertexIndex(const unsigned int indexes)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				if (mUsingVBO)
					mIndexOffset = indexes;
			}

			virtual void setIndexCount(unsigned int count)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				assert(count != 0);
				mIndexCount = count;
			}

			virtual void draw3DLine(const vector3& start, const vector3& end)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				startVertices(VERTEXMODE_LINE);
					vertex(start);
					vertex(end);
				endVertices();
			}

			virtual void drawLineBox(const vector3& min, const vector3& max)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				// Base
				startVertices(VERTEXMODE_LINE);
					vertex(min);
					vertex(vector3(min.x, min.y, max.z));
					vertex(vector3(max.x, min.y, max.z));
					vertex(vector3(max.x, min.y, min.z));
				endVertices();

				// Top
				startVertices(VERTEXMODE_LINE);
					vertex(vector3(min.x, max.y, min.z));
					vertex(vector3(min.x, max.y, max.z));
					vertex(vector3(max.x, max.y, max.z));
					vertex(vector3(max.x, max.y, min.z));
				endVertices();

				// Edges
				startVertices(VERTEXMODE_LINE);
					vertex(min);
					vertex(vector3(min.x, max.y, min.z));
				endVertices();

				startVertices(VERTEXMODE_LINE);
					vertex(max);
					vertex(vector3(max.x, min.y, max.z));
				endVertices();

				startVertices(VERTEXMODE_LINE);
					vertex(vector3(max.x, min.y, min.z));
					vertex(vector3(max.x, max.y, min.z));
				endVertices();

				startVertices(VERTEXMODE_LINE);
					vertex(vector3(min.x, min.y, max.z));
					vertex(vector3(min.x, max.y, max.z));
				endVertices();
			}

			virtual void drawArrays() = 0;
			virtual void screenshot(const char* filename) = 0;

			/**
			 * See if the rendersystem supports
			 * vertex buffer objects.
			 */
			virtual bool getVBOSupport() = 0; 

			/**
			 * Generate a VBO
			 */
			virtual void genVBO(kVBO* target) = 0;

			/**
			 * Bind a VBO to the array type
			 */
			virtual void bindVBO(kVBO* target, VBOArrayType type = VBO_ARRAY) = 0;

			/**
			 * Set memory area for VBO
			 */
			virtual void setVBOData(VBOArrayType type, int size, void* data, VBOUsage use) = 0;

			/**
			 * Remove VBO
			 */
			virtual void delVBO(kVBO* target) = 0;

			virtual unsigned int getScreenWidth() = 0;
			virtual unsigned int getScreenHeight() = 0;
	};
}

#endif

