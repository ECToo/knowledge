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

#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "prerequisites.h"
#include "drawable.h"
#include "singleton.h"
#include "rendersystem.h"
#include "camera.h"
#include "sprite.h"
#include "timer.h"
#include "particle.h"
#include "world.h"
#include "light.h"

namespace k
{
	/**
	 * \brief The global renderer, father of all objects.
	 * The renderer is responsible for drawing objects, the scenarios
	 * and Skyplanes, Background, Boxes.
	 */
	class DLL_EXPORT renderer : public singleton<renderer>
	{
		private:
			std::list<drawable3D*> m3DObjects;
			std::list<drawable2D*> m2DObjects;
			std::list<sprite*> mSprites;
			std::list<light::light*> mLights;

			camera* mActiveCamera;

			/**
			 * If this vecto changes, notify
			 * all sprites that their orientations
			 * are invalid.
			 */
			vector3 mLastCameraPos;
			quaternion mLastCameraOrientation;

			/**
			 * Keep a timer to calculate FPS
			 */
			timer mFpsTimer;
			bool mCalculateFps;
			unsigned int mFpsCount;
			unsigned int mLastFps;

			/**
			 * Time since frame start
			 */
			timer mFrameTime;

			/**
			 * Skybox
			 */
			material* mSkybox;
			void _drawSkybox();

			/**
			 * SkyPlane
			 */
			material* mSkyPlane;
			void _drawSkyPlane();

			/**
			 * Renderer time
			 */
			timer mRendererTimer;

			/**
			 * Render to texture
			 */
			platformTexturePointer* mTextureTarget;
			bool mRenderToTexture;
			unsigned int mRTTSize[2];

			/**
			 * World
			 */
			world* mActiveWorld;

		public:
			/**
			 * Constructor.
			 */
			renderer();

			/**
			 * Destructor. 
			 * Note: Doesnt free anything.
			 */
			~renderer();

			/**
			 * Returns the renderer singleton instance.
			 */
			static renderer& getSingleton();

			/**
			 * Push a 3D drawable into renderer list. Opaque objects will be drawn
			 * first and transparent objects will be drawn last.
			 */
			void push3D(drawable3D* object);

			/**
			 * Remove a 3D object from renderer list.
			 */
			void pop3D(drawable3D* object);

			/**
			 * Push a 2D drawable into renderer list. 2D Objects will be sorted by
			 * their Z factor (@see drawable2D)
			 */
			void push2D(drawable2D* object);

			/**
			 * Remove a 2D object from renderer list.
			 */
			void pop2D(drawable2D* object);

			/**
			 * Force sorting of 2D objects.
			 */
			void sort2D();

			/**
			 * Set renderer skybox by material name.
			 * @param matName The skybox material.
			 */
			void setSkyBox(const std::string& matName);

			/**
			 * Set renderer skybox by material pointer.
			 * @param mat A pointer to skybox material.
			 */
			void setSkyBox(material* mat);

			/**
			 * Set renderer skyplane by material name.
			 * @param matName The skyplane material.
			 */
			void setSkyPlane(const std::string& matName);

			/**
			 * Set renderer skyplane by material pointer.
			 * @param mat A pointer to skyplane material.
			 */
			void setSkyPlane(material* mat);

			/**
			 * Set a sub-world of the renderer. This might be interesting
			 * when you have octrees or any advanced scene rendering and want
			 * renderer to draw it before 3D and 2D objects.
			 *
			 * @param w A pointer to the world.
			 */
			void setWorld(world* w);

			/**
			 * Asks the renderer to draw the full scene.
			 */
			void draw();

			/**
			 * Create a point light.
			 */
			light::light* createPointLight();

			/**
			 * Render to texture.
			 *
			 * After calling this function, the first
			 * call to draw() on renderer will render
			 * to a texture specified by @tex. Every
			 * subsequent call to draw() will work
			 * normally.
			 */
			void prepareRTT(unsigned int w, unsigned h, platformTexturePointer* tex);	

			/**
			 * Create a sprite within this renderer
			 */
			sprite* createSprite(vec_t radi, material* mat);
			sprite* createSprite(const std::string& mat, vec_t radius);

			/**
			 * Remove the sprite from the renderer.
			 * Keep in mind that this wont clear
			 * the sprite pointer. If you want to do
			 * this use the fullRemoveSprite function.
			 */
			void removeSprite(sprite* spr);

			/**
			 * Remove the sprite from the renderer cleaning
			 * its pointer (freeing the memory)
			 */
			void fullRemoveSprite(sprite* spr);

			/**
			 * Return the renderer active camera
			 */
			camera* getCamera();

			/**
			 * Set renderer active camera.
			 * @param cam A pointer to the active camera.
			 */
			void setCamera(camera* cam);

			/**
			 * Get renderer time (in milliseconds)
			 */
			long getTimeNow();

			/**
			 * Set whenever renderer will count frames or not.
			 */
			void setFpsCounter(bool status);

			/**
			 * Get scene FPS
			 */
			unsigned int getFps();
			
			/**
			 * Get scene last FPS
			 */
			unsigned int getLastFps();
	};
}

#endif

