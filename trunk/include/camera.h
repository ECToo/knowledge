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

#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"
#include "quaternion.h"

namespace k
{
	class camera
	{
		private:
			// Matrix to keep camera
			// orientation + translation
			matrix4 mFinal;

			vector3 mPosition;
			quaternion mOrientation;

		public:
			// Copy the camera matrix to the current
			// modelview matrix. Needs to be called
			// before drawing each object.
			void copyView();

			// Apply the modelview matrix to the scene
			void setView();

			// Look at the vector dest
			void lookAt(vector3 dest);
			vector3 getDirection();
			vector3 getUp();
			vector3 getRight();

			// Translations
			void setPosition(vector3 pos);
			vector3& getPosition();

			// Orientation
			void setOrientation(quaternion ori);
			quaternion& getOrientation();
	};
}

#endif

