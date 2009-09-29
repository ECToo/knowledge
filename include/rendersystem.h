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

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"
#include "color.h"
#include "quaternion.h"
#include "material.h"
#include "logger.h"

namespace k 
{
	/**
	 * Shade models
	 */
	enum ShadeModel
	{
		SHADEMODEL_FLAT,
		SHADEMODEL_SMOOTH
	};

	/**
	 * Matrix modes
	 */
	enum MatrixMode
	{
		MATRIXMODE_PROJECTION,
		MATRIXMODE_MODELVIEW
	};

	/**
	 * Vertex modes
	 */
	enum VertexMode
	{
		VERTEXMODE_LINE,
		VERTEXMODE_POINTS,
		VERTEXMODE_TRIANGLES,
		VERTEXMODE_TRI_STRIP,
		VERTEXMODE_QUAD
	};

	/**
	 * Gamma modes
	 */
	enum GammaModes
	{
		GAMMA_1_0,
		GAMMA_1_7,
		GAMMA_2_2
	};

	/**
	 * Types of VBO Arrays
	 */
	enum VBOArrayType
	{
		VBO_ARRAY,
		VBO_ELEMENT_ARRAY
	};

	/**
	 * Types of VBO Usages
	 */
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

	/**
	 * Texture environment modes
	 */
	enum texEnvMode
	{
		TEX_ENV_MODULATE,
		TEX_ENV_REPLACE,
		TEX_ENV_DECAL,
		TEX_ENV_BLEND,
		TEX_ENV_ADD
	};

	#define MAX_TEXCOORD 8

	/**
	 * \brief renderSystem is responsible for low level interaction with OS/Hardware.
	 * The renderSystem is the part of the engine that communicates and operate
	 * the hardware or the OS functions to display and collect visual data. It has a number
	 * of functions to make this task easy and totally cross platform. Each OS or Hardware should
	 * implement their own renderSystem 's.
	 */
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
			platformTexturePointer* mRttTarget;

			// Vertex Buffer Object
			bool mUsingVBO;
			int mVertexOffset;
			int mNormalOffset;
			int mIndexOffset;
			int mTexCoordOffset[MAX_TEXCOORD];

			// Last Light Index selected
			unsigned int mLastLightIndex;

		public:

			virtual ~renderSystem() {}
			
			/**
			 * Initialize the render system, allocating necessary memory and initializing hardware.
			 */
			virtual void initialize() = 0;

			/**
			 * Deinitialize hardware and free allocate data.
			 */
			virtual void deinitialize() = 0;

			/**
			 * Ask the render system to configure itself.
			 */
			virtual void configure() = 0;

			/**
			 * Create (if possible) a render window and drawing context.
			 */
			virtual void createWindow(const int w, const int h) = 0;
			
			/**
			 * Destroy (if created) render window.
			 */
			virtual void destroyWindow() = 0;

			/**
			 * Set the created window title. If window doesnt exists, this function will do nothing.
			 *
			 * @param title The new window title.
			 */
			virtual void setWindowTitle(const std::string& title) = 0;

			/**
			 * Make the OS show or not its cursor.
			 */
			virtual void showCursor(bool show) = 0;

			/**
			 * This function needs to be called before drawing each frame.
			 */
			virtual void frameStart() = 0;

			/**
			 * This function needs to be called after drawing each frame.
			 */
			virtual void frameEnd() = 0;

			/**
			 * Set if this rendersystem is outputing in wireframe or not.
			 */
			virtual void setWireFrame(bool wire) = 0;

			/**
			 * Set Render to Texture Target
			 * @param tex A valid pointer to the texture target.
			 */
			virtual void setRttTarget(platformTexturePointer* tex)
			{
				kAssert(tex);
				mRttTarget = tex;
			}
			
			/**
			 * Set render to texture output dimensions.
			 * @param width The RTT width.
			 * @param height The RTT height.
			 */
			virtual void setRttSize(unsigned int width, unsigned int height)
			{
				mRttDimensions[0] = width;
				mRttDimensions[1] = height;
			}

			/**
			 * Set if next frame is going to be drawn to the target texture (RTT)
			 */
			virtual void setRenderToTexture(bool rtt)
			{
				mRenderToTexture = rtt;
			}

			/**
			 * Set clear color.
			 *
			 * @param cl The new clear color
			 */
			virtual void setClearColor(const color& cl) = 0;

			/**
			 * Set clear depth.
			 *
			 * @param amount The new clear depth. 
			 */
			virtual void setClearDepth(const vec_t amount) = 0;

			/**
			 * Set if we are doing depth test or not.
			 */
			virtual void setDepthTest(bool test) = 0;
			
			/** 
			 * Set if we are doing depth mask (writing to the depth buffer) or not.
			 */
			virtual void setDepthMask(bool mask) = 0;

			/**
			 * Set the renderSystem shade model. @see ShadeModel
			 *
			 * @param model The new shade model.
			 */
			virtual void setShadeModel(ShadeModel model) = 0;

			/**
			 * Set the renderSystem matrix mode. @see MatrixMode
			 *
			 * @param mode The new matrix mode.
			 */
			virtual void setMatrixMode(MatrixMode mode) = 0;

			/**
			 * Push current matrix to the stack.
			 */
			virtual void pushMatrix() = 0;

			/**
			 * Pop current matrix from the stack.
			 */
			virtual void popMatrix() = 0;

			/**
			 * Copy the identity matrix over the current matrix. @see MatrixMode
			 */
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

			/**
			 * Sets the inverse transpose modelview matrix. 
			 */
			virtual void setInverseTransposeModelview(const matrix4& mat) = 0;

			/**
			 * Copy the source matrix over the current matrix.
			 *
			 * @param mat The source matrix.
			 */
			virtual void copyMatrix(const matrix4& mat) = 0;

			/**
			 * Multiply the source matrix by the current matrix.
			 *
			 * @param mat The source matrix.
			 */
			virtual void multMatrix(const matrix4& mat) = 0;

			/**
			 * Returns the modelview matrix.
			 */
			virtual matrix4 getModelView() = 0;

			/**
			 * Returns the modelview matrix to an array of 16 floats.
			 *
			 * @param mat The array of 16 floats.
			 */
			virtual void getModelView(float mat[][4]) = 0;

			/**
			 * Translate the current matrix by x, y, z.
			 */
			virtual void translateScene(vec_t x, vec_t y, vec_t z) = 0;

			/**
			 * Rotate the current matrix by angle on the axis x, y, z
			 */
			virtual void rotateScene(vec_t angle, vec_t x, vec_t y, vec_t z) = 0;

			/**
			 * Scales the current matrix by x, y, z
			 */
			virtual void scaleScene(vec_t x, vec_t y, vec_t z) = 0;

			/**
			 * Set viewport rectangle.
			 *
			 * @param x The viewport X
			 * @param y The viewport Y
			 * @param w The viewport width
			 * @param h The viewport height
			 */
			virtual void setViewPort(int x, int y, int w, int h) = 0;

			/**
			 * Set the perspective matrix on the renderSystem.
			 * @param fov The new Field of View.
			 * @param aspect The new aspect ratio.
			 * @param nearP The new near plane distance.
			 * @param farP The new far plane distance.
			 */
			virtual void setPerspective(vec_t fov, vec_t aspect, vec_t nearP, vec_t farP) = 0;
			
			/**
			 * Set the orthographic matrix on the renderSystem.
			 * @param left The leftmost edge of the projection.
			 * @param right The rightmost edge of the projection.
			 * @param top The top of the projection.
			 * @param bottom The bottom of the projection.
			 * @param nearP The near plane distance.
			 * @param farP The far plane distance.
			 */
			virtual void setOrthographic(vec_t left, vec_t right, vec_t bottom, vec_t top, vec_t nearP, vec_t farP) = 0;

			/**
			 * Set culling mode. @see CullMode
			 * @param culling The new culling mode.
			 */
			virtual void setCulling(CullMode culling) = 0;

			/**
			 * Start drawing vertices. @see VertexMode
			 * @param mode The vertex drawing mode.
			 */
			virtual void startVertices(VertexMode mode) = 0;

			/**
			 * Draw a vertex, must be called between startVertices() and endVertices() calls.
			 * @param vert The vertex to be drawn.
			 */
			virtual void vertex(const vector3& vert) = 0;

			/**
			 * Draw a normal, must be called between startVertices() and endVertices() calls.
			 * @param norm The normal to be drawn.
			 */
			virtual void normal(const vector3& norm) = 0;

			/**
			 * Draw a vertex color, must be called between startVertices() and endVertices() calls.
			 * @param col The color to be drawn.
			 */
			virtual void vcolor(const color& col) = 0;

			/**
			 * Draw a texture coordinate, must be called between startVertices() and endVertices() calls.
			 * @param coord The texture coordinate to be drawn.
			 */
			virtual void texCoord(const vector2& coord) = 0;

			/**
			 * End drawing vertices started with startVertices().
			 */
			virtual void endVertices() = 0;

			/**
			 * Set current object material ambient component.
			 * @param ambient Ambient component.
			 */
			virtual void matAmbient(const color& ambient) = 0;

			/**
			 * Set current object material diffuse component.
			 * @param diffuse Diffuse component.
			 */
			virtual void matDiffuse(const color& diffuse) = 0;

			/**
			 * Set current object material specular component.
			 * @param specular Specular component.
			 */
			virtual void matSpecular(const color& specular) = 0;

			/**
			 * Bind a material to the drawing object.
			 * @param mat A valid pointer to the object material.
			 */
			void bindMaterial(material* mat)
			{
				mActiveMaterial = mat;
			}

			/**
			 * Create and allocate a texture.
			 * @param w Texture width.
			 * @param h Texture height.
			 * @param bpp Texture Bytes Per Pixel.
			 * @param[out] tex The returning created texture.
			 */
			virtual void genTexture(unsigned int w, unsigned int h, unsigned int bpp, platformTexturePointer* tex) = 0;

			/**
			 * Bind a texture to a channel (max 8 texture channels).
			 * @param tex A valid texture pointer.
			 * @param chan A valid channel [0,7]
			 */
			virtual void bindTexture(platformTexturePointer* tex, int chan) = 0;

			/**
			 * Unbind texture from a channel. If theres no texture binded, nothing happens.
			 * @param chan A valid channel [0,7]
			 */
			virtual void unBindTexture(int chan) = 0;

			/**
			 * Set objects blend mode. See the material documentation to know more about
			 * blending modes.
			 */
			virtual void setBlendMode(unsigned short src, unsigned short dst) = 0;

			/**
			 * Set if blending is enabled or not.
			 */
			virtual void setBlend(bool state) = 0;

			/**
			 * Copy the current rendersystem buffer to destination texture.
			 * @param[out] tex The destination texture.
			 */
			virtual void copyToTexture(platformTexturePointer* tex) = 0;

			/**
			 * Clear array descriptions, must be called before each array drawing.
			 * @see VertexMode
			 */
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

			/**
			 * Set if we are drawing with vertex buffer objects or not.
			 * In case VBOS arent supported, this function does nothing.
			 */
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

			/**
			 * Set the vertex array and its stride.
			 * @param vertices A valid pointer to vertex array.
			 * @param stride The vertex stride (Distance in bytes between vertexes)
			 */
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

			/**
			 * Set the vertex array and its stride. Use this function if you are drawing with VBOS
			 * @param vertices A valid index to vertex array.
			 * @param stride The vertex stride (Distance in bytes between vertexes)
			 */
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

			/**
			 * Set array vertex count.
			 * @param count The vertex array vertex count.
			 */
			virtual void setVertexCount(unsigned int count)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				kAssert(count);
				mVertexCount = count;
			}

			/**
			 * Set texture coordinate array and its stride.
			 * @param coords A valid pointer to texture coordinate array.
			 * @param stride The array stride.
			 * @param slot The texture coordinate slot [0,7] for the array.
			 */
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

			/**
			 * Set texture coordinate array and its stride. Use this if you are drawing with VBOS
			 * @param coords A valid index to texture coordinate array.
			 * @param stride The array stride.
			 * @param slot The texture coordinate slot [0,7] for the array.
			 */
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

			/**
			 * Set normal array and its stride. 
			 * @param normals A valid pointer to normal array.
			 * @param stride The array stride.
			 */
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

			/**
			 * Set normal array and its stride. Use this if you are drawing with VBOS
			 * @param normals A valid index to normal array.
			 * @param stride The array stride.
			 */
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

			/**
			 * Set the indexes array.
			 * @param indexes A valid pointer to the indexes array.
			 */
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

			/**
			 * Set the indexes array. Use this if you are drawing with VBOS
			 * @param indexes A valid index to the indexes array.
			 */
			virtual void setVertexIndex(const unsigned int indexes)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				if (mUsingVBO)
					mIndexOffset = indexes;
			}

			/**
			 * Set the number of indices on the array.
			 */
			virtual void setIndexCount(unsigned int count)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				assert(count != 0);
				mIndexCount = count;
			}

			/**
			 * Draw a 3D line between two points.
			 * @param start The starting point.
			 * @param end The ending point.
			 */
			virtual void draw3DLine(const vector3& start, const vector3& end)
			{
				if (mActiveMaterial && mActiveMaterial->getNoDraw())
					return;

				startVertices(VERTEXMODE_LINE);
					vertex(start);
					vertex(end);
				endVertices();
			}

			/**
			 * Draw the edges of a box.
			 * @param min The mins vector.
			 * @param max The maxs vector.
			 */
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

			/**
			 * Draw previously set arrays.
			 * @see clearArrayDesc 
			 * @see setVertexArray
			 * @see setVertexCount
			 * @see setVertexIndex
			 * @see setNormalArray
			 * @see setTexCoordArray
			 */
			virtual void drawArrays() = 0;

			/**
			 * Takes a screenshot and saves on a file.
			 * @param filename The destination file for the screenshot.
			 */
			virtual void screenshot(const char* filename) = 0;

			/**
			 * Get the last light index we enabled.
			 */
			unsigned int getEnabledLightCount() const
			{
				return mLastLightIndex;
			}

			/**
			 * If we are rendering with lights.
			 */
			virtual bool isLightOn() = 0;

			/**
			 * Set lighting
			 * @status If lighting is on or off.
			 */
			virtual void setLighting(bool status) = 0;

			/**
			 * Set light status
			 * @status If light is on or off.
			 */
			virtual void setLight(unsigned int i, bool status) = 0;

			/**
			 * Set light position
			 * @i Light index
			 * @p New position
			 */
			virtual void setLightPosition(unsigned int i, const vector3& p, bool directional) = 0;

			/**
			 * Set light ambient
			 * @i Light index
			 * @a Ambient
			 */
			virtual void setLightAmbient(unsigned int i, const color& a) = 0;

			/**
			 * Set light specular
			 * @i Light index
			 * @s Specular 
			 */
			virtual void setLightSpecular(unsigned int i, const color& s) = 0;

			/**
			 * Set light diffuse
			 * @i Light index
			 * @d Diffuse
			 */
			virtual void setLightDiffuse(unsigned int i, const color& d) = 0;

			/**
			 * Set light attenuation
			 * @i Light index
			 * @att Light attenuation vector [constant, linear, quadratic]
			 */
			virtual void setLightAttenuation(unsigned int i, const vector3& att) = 0;

			/**
			 * Check if rendersystem supports point sprite.
			 */
			virtual bool getPointSpriteSupport() = 0;

			/**
			 * Get max size for point sprites
			 */
			virtual float getPointSpriteMaxSize() = 0;

			/**
			 * Set draw commands using point sprite
			 */
			virtual void setPointSprite(bool enabled) = 0;

			/**
			 * Set Point sprite size.
			 */
			virtual void setPointSpriteSize(vec_t size) = 0;

			/**
			 * Draw an array of sprite positions
			 */
			virtual void drawPointSprites(const vec_t* positions, unsigned int numPositions) = 0;

			/**
			 * Set point sprite attenuation
			 */
			virtual void setPointSpriteAttenuation(vec_t* att) = 0;

			/**
			 * See if the rendersystem supports
			 * vertex buffer objects.
			 */
			virtual bool getVBOSupport() = 0; 

			/**
			 * Generate a VBO
			 */
			virtual void genVBO(platformVBO* target) = 0;

			/**
			 * Bind a VBO to the array type
			 */
			virtual void bindVBO(platformVBO* target, VBOArrayType type = VBO_ARRAY) = 0;

			/**
			 * Set memory area for VBO
			 */
			virtual void setVBOData(VBOArrayType type, int size, void* data, VBOUsage use) = 0;

			/**
			 * Remove VBO
			 */
			virtual void delVBO(platformVBO* target) = 0;

			/**
			 * Get the screen width.
			 */
			virtual unsigned int getScreenWidth() = 0;

			/**
			 * Get the screen height.
			 */
			virtual unsigned int getScreenHeight() = 0;
	};
}

#endif

