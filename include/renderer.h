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
#include "timer.h"

namespace k
{
	class renderer : public singleton<renderer>
	{
		private:
			std::list<drawable3D*> m3DObjects;
			std::list<drawable2D*> m2DObjects;
			camera* mActiveCamera;

			/**
			 * Keep a timer to calculate FPS
			 */
			timer mFpsTimer;
			bool mCalculateFps;
			unsigned int mFpsCount;
			unsigned int mLastFps;

		public:
			renderer();
			~renderer();

			static renderer& getSingleton();

			void push3D(drawable3D* object);
			void pop3D(drawable3D* object);

			void push2D(drawable2D* object);
			void pop2D(drawable2D* object);

			void draw();

			void setCamera(camera* cam);

			// Fps Counter
			void setFpsCounter(bool status);
			unsigned int getFps();
			unsigned int getLastFps();
	};
}

#endif

