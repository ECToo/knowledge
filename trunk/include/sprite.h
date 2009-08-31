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
	 * \brief Automatic oriented (always facing camera) finite planes with textures.
	 *
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

			/**
			 * Is this sprite visible?
			 */
			bool mSpriteVisible;

		public:
			/**
			 * Constructor, empty sprite.
			 */
			sprite();

			/**
			 * Constructor, creates a sprite from a material and radius.
			 *
			 * @param[in] mat A valid pointer to sprite material.
			 * @param[in] radi The sprite radius.
			 */
			sprite(material* mat, vec_t radi);

			/**
			 * Constructor, creates a sprite from a material and radius.
			 *
			 * @param[in] mat Sprite material name.
			 * @param[in] radi The sprite radius.
			 */
			sprite(const std::string& mat, vec_t radi);

			/**
			 * Destructor.
			 */
			~sprite();

			/**
			 * Returns the sprite material.
			 */
			material* getMaterial() const;

			/**
			 * Returns the sprite radius.
			 */
			const vec_t getRadius() const;

			/**
			 * Set if the sprite is visible or not.
			 */
			void setVisible(bool vis)
			{
				mSpriteVisible = vis;
			}

			/**
			 * Returns true if sprite is visible, false otherwise.
			 */
			bool isVisible() const
			{
				return mSpriteVisible;
			}

			/**
			 * Set sprite material.
			 *
			 * @param[in] mat A valid pointer to the new sprite material.
			 */
			void setMaterial(material* mat);

			/**
			 * Set sprite material.
			 *
			 * @param[in] mat The new sprite material name.
			 */
			void setMaterial(const std::string& mat);

			/**
			 * Set sprite radius.
			 *
			 * @param[in] rad The new sprite radius.
			 */
			void setRadius(vec_t rad);

			/**
			 * Set sprite position.
			 *
			 * @param[in] pos The new position.
			 */
			void setPosition(const vector3& pos);

			/**
			 * Returns sprite position in world space.
			 */
			const vector3& getPosition() const;

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

			/**
			 * Draws the sprite ignoring material
			 * settings.
			 */
			void rawDraw();
	};
}

#endif

