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

