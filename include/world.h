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

#ifndef _WORLD_H_
#define _WORLD_H_

#include "prerequisites.h"
#include "camera.h"

namespace k 
{
	/**
	 * \brief A basic abstraction for a world handler.
	 * This class should be a basic abstraction for world
	 * handlers. You can for example create a world handler that uses
	 * octree and insert it into the renderer so it will cull and process
	 * based on camera position.
	 */
	class DLL_EXPORT world
	{
		public:
			virtual ~world() {}

			/**
			 * Draw the world from the cameras position.
			 * @param viewer The active camera on the renderer.
			 */
			virtual void draw(const camera* viewer) = 0;
	};
}

#endif

