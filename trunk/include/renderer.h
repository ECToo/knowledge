/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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

namespace k
{
	class DLL_EXPORT renderer : public singleton<renderer>
	{
		private:
			std::list<drawable3D*> m3DObjects;
			std::list<drawable2D*> m2DObjects;
			std::list<sprite*> mSprites;

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
			renderer();
			~renderer();

			static renderer& getSingleton();

			void push3D(drawable3D* object);
			void pop3D(drawable3D* object);

			void push2D(drawable2D* object);
			void pop2D(drawable2D* object);
			void sort2D();

			void setSkyBox(const std::string& matName);
			void setSkyBox(material* mat);
			void setSkyPlane(const std::string& matName);
			void setSkyPlane(material* mat);
			void setWorld(world* w);

			void draw();

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

			camera* getCamera();
			void setCamera(camera* cam);

			// Get renderer time
			long getTimeNow();

			// Fps Counter
			void setFpsCounter(bool status);
			unsigned int getFps();
			unsigned int getLastFps();
	};
}

#endif

