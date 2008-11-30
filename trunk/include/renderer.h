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

namespace k
{
	class renderer : public singleton<renderer>
	{
		private:
			std::list<drawable3D*> m3DObjects;
			std::list<drawable2D*> m2DObjects;
			std::list<sprite*> mSprites;
			std::list<particleSystem*> mParticles;

			camera* mActiveCamera;

			/**
			 * If this vecto changes, notify
			 * all sprites that their orientations
			 * are invalid.
			 */
			vector3 mLastCameraPos;

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

		public:
			renderer();
			~renderer();

			static renderer& getSingleton();

			void push3D(drawable3D* object);
			void pop3D(drawable3D* object);

			void push2D(drawable2D* object);
			void pop2D(drawable2D* object);
			void sort2D();

			void pushParticle(particleSystem* p);
			void popParticle(particleSystem* p);

			void setSkyBox(const std::string& matName);
			void setSkyBox(material* mat);
			void setSkyPlane(const std::string& matName);
			void setSkyPlane(material* mat);

			void draw();

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

			// Fps Counter
			void setFpsCounter(bool status);
			unsigned int getFps();
			unsigned int getLastFps();
	};
}

#endif

