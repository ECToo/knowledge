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

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "prerequisites.h"
#include "vector3.h"
#include "quaternion.h"
#include "material.h"
#include "matrix4.h"

namespace k 
{
	/**
	 * The sprite is a special class because it is
	 * directly connected to the scene camera to archieve
	 * its orientation. Keep in mind that sprites
	 * shouldnt be created by yourself but instead by
	 * the renderer class, wich will keep them.
	 */
	class DLL_EXPORT sprite
	{
		private:	
			/*
			 * Half the width and height
			 * of the sprite quad.
			 */
			vec_t	mRadius;

			material* mMaterial;
			vector3 mPosition;

			/**
			 * This matrix will be the result of the
			 * sprite orientation + position, to be
			 * applied each frame. 
			 */
			matrix4 mTransPos;

			/**
			 * The sprite mTransPos is invalid
			 * and must be re-calculated.
			 */
			bool mInvalidTransPos;

			/**
			 * Calculate the mTransPos.
			 * Only happens when mInvalidTransPos is true.
			 */
			void calculateTransPos();

		public:
			sprite(material* mat, vec_t radi);
			sprite(const std::string& mat, vec_t radi);
			~sprite();

			material* getMaterial();
			vec_t getRadius();

			void setMaterial(material* mat);
			void setMaterial(const std::string& mat);
			void setRadius(vec_t rad);

			void setPosition(vector3& pos);
			const vector3& getPosition();

			/**
			 * Invalidate sprite orientation.
			 * This can happen when the camera position
			 * changes or the sprite does.
			 */
			void invalidate();

			/**
			 * Draw the sprite.
			 */
			void draw();
	};
}

#endif

