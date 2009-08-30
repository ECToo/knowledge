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

#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"
#include "quaternion.h"

namespace k 
{
	/**
	 * A virtual representation of a drawable 2D entity.
	 */
	class DLL_EXPORT drawable2D
	{
		protected:
			vector2 mPosition;
			vec_t mZ;

			/**
			 * Width x Height
			 */
			vector2 mScale;

			/**
			 * Rotation in degrees
			 */
			vec_t mRotation;

			// Another Drawable this one
			// is attached to
			const drawable2D* mDrawableAttach;

			// Is this drawable ignored by pipeline?
			bool mDrawableVisible;

		public:
			drawable2D();
			virtual ~drawable2D();

			/**
			 * Set drawable position.
			 *
			 * @param pos The new position.
			 */
			void setPosition(const vector2& pos);

			/**
			 * Set drawable rotation.
			 *
			 * @param rot The new drawable angle (in degrees).
			 */
			void setRotation(const vec_t rot);

			/**
			 * Set drawable scale.
			 *
			 * @param size The new drawable dimensions in global coordinates (0, ScreenWidth/Height)
			 */
			void setScale(const vector2& size);

			/**
			 * Set drawable Z ordering factor.
			 * 2D are automatically sorted on the renderer by their Z ordering attribute. 
			 * Drawables with a higher Z factor are drawn lastly.
			 */
			void setZ(vec_t z);

			/**
			 * Set the visibility of the drawable.
			 *
			 * @param vis The visibility as boolean value.
			 */
			void setVisible(bool vis)
			{
				mDrawableVisible = vis;
			}

			/**
			 * Returns true if the drawable is visible, false otherwise.
			 */
			bool isVisible() const
			{
				return mDrawableVisible;
			}

			/**
			 * Attach a child 2D drawable to this one. 
			 */
			void attach(const drawable2D* target);

			/**
			 * If this drawable is attached to another one, will return
			 * its root/parent.
			 */
			const drawable2D* getRoot() const;

			/**
			 * Return this drawable position indepent of its parents.
			 */
			const vector2& getRelativePosition() const;

			/**
			 * Return this drawable orientation indepent of its parents.
			 */
			const vec_t getRelativeRotation() const;

			/**
			 * Return this drawable position and if it has a parent, return
			 * the final position.
			 */
			vector2 getAbsolutePosition() const;

			/**
			 * Return this drawable orientation and if it has a parent, return
			 * the final orientation.
			 */
			vec_t getAbsoluteRotation() const;

			/**
			 * Return the drawable dimensions (scale).
			 */
			const vector2& getDimension() const;

			/**
			 * Return the drawable Z factor.
			 */
			const vec_t getZ() const;

			virtual void draw() = 0;
	};

	class DLL_EXPORT boundingBox
	{
		private:
			vector3 mMins;
			vector3 mMaxs;

		public:
			boundingBox();
			boundingBox(const vector3& min, const vector3& max);

			boundingBox operator + (const boundingBox& b);
			boundingBox& operator += (const boundingBox& b);

			boundingBox operator - (const boundingBox& b);
			boundingBox& operator -= (const boundingBox& b);

			void setTestMins(const vector3& min);
			void setTestMaxs(const vector3& max);
			void setTest(const vector3& dist);

			void setMins(const vector3& min);
			void setMaxs(const vector3& max);

			const vector3& getMins() const;
			const vector3& getMaxs() const;

			void draw() const;
	};

	/**
	 * A virtual representation of a drawable 3D entity.
	 */
	class DLL_EXPORT drawable3D
	{
		protected:
			vector3 mScale;
			vector3 mPosition;
			quaternion mOrientation;

			// Draws bounding box? Default to false
			bool mDrawAABB;

			// Another Drawable this one
			// is attached to
			const drawable3D* mDrawableAttach;

			// Is this drawable ignored by pipeline?
			bool mDrawableVisible;

		public:
			drawable3D();
			virtual ~drawable3D();

			/**
			 * Set drawable orientation.
			 *
			 * @param orientation The new drawable orientation.
			 */
			void setOrientation(const quaternion& orientation);

			/**
			 * Set drawable scale.
			 *
			 * @param scale The new drawable dimensions in global coordinates (0, ScreenWidth/Height)
			 */
			void setScale(const vector3& scale);

			/**
			 * Set drawable scale.
			 *
			 * @param scale The new drawable dimension (will be multiplied on all axis) in global coordinates (0, ScreenWidth/Height)
			 */
			void setScale(const vec_t scale);

			/**
			 * Set drawable position.
			 *
			 * @param pos The new position.
			 */
			void setPosition(const vector3& pos);

			/**
			 * Set the visibility of the drawable.
			 *
			 * @param vis The visibility as boolean value.
			 */
			void setVisible(bool vis)
			{
				mDrawableVisible = vis;
			}

			/**
			 * Returns true if the drawable is visible, false otherwise.
			 */
			bool isVisible() const
			{
				return mDrawableVisible;
			}

			/**
			 * Attach a child 3D drawable to this one. 
			 */
			void attach(const drawable3D* target);

			/**
			 * If this drawable is attached to another one, will return
			 * its root/parent.
			 */
			const drawable3D* getRoot() const;

			/**
			 * Return the drawable scale.
			 */
			const vector3& getScale() const;

			/**
			 * Return this drawable position and if it has a parent, return
			 * the final position.
			 */
			vector3 getAbsolutePosition() const;

			/**
			 * Return this drawable orientation and if it has a parent, return
			 * the final orientation.
			 */
			quaternion getAbsoluteOrientation() const;

			/**
			 * Return this drawable position indepent of its parents.
			 */
			const vector3& getRelativePosition() const;

			/**
			 * Return this drawable orientation indepent of its parents.
			 */
			const quaternion& getRelativeOrientation() const;

			/**
			 * Set drawable bounding box drawing.
			 *
			 * @param option A boolean with the new option to draw drawable bounding box.
			 */
			void setDrawBoundingBox(bool option);

			/**
			 * Returns true if we are drawing the bounding box, false otherwise.
			 */
			bool getDrawBoundingBox() const;

			virtual void draw() = 0;

			/**
			 * Return true if the drawable is opaque (its material doesnt have any transparency).
			 */
			virtual bool isOpaque() const = 0;

			/**
			 * Return drawable Axis Aligned bounding box.
			 */
			virtual boundingBox getAABoundingBox() const = 0;

			/**
			 * Return drawable bounding box.
			 */
			virtual boundingBox getBoundingBox() const = 0;
	};
}

#endif

