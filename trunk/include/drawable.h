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

#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

#include "prerequisites.h"
#include "vector2.h"
#include "vector3.h"
#include "quaternion.h"

namespace k 
{
	/**
	 * \brief A 2D Rectangle.
	 */
	class DLL_EXPORT rectangle
	{
		protected:
			vector2 mPosition;
			vector2 mDimension;

		public:
			/**
			 * Create an empty dimensionless rectangle.
			 */
			rectangle() {}

			/**
			 * Create a rectangle.
			 * @pos The rectangle position.
			 * @size The rectangle dimensions.
			 */
			rectangle(const vector2& pos, const vector2& size)
			{
				mPosition = pos;
				mDimension = size;
			}

			/**
			 * Create a rectangle.
			 */
			rectangle(vec_t x, vec_t y, vec_t width, vec_t height)
			{
				mPosition.x = x;
				mPosition.y = y;
				mDimension.x = width;
				mDimension.y = height;
			}

			/**
			 * Set rectangle dimension.
			 */
			void setDimension(const vector2& dimension)
			{
				mDimension = dimension;
			}

			/**
			 * Set rectangle position.
			 */
			void setPosition(const vector2& pos)
			{
				mPosition = pos;
			}

			/**
			 * Returns rectangle position
			 */
			const vector2& getPosition() const
			{
				return mPosition;
			}
			
			/**
			 * Returns rectangle dimension
			 */
			const vector2& getDimension() const
			{
				return mDimension;
			}

			/**
			 * Return rectangle position in x axis.
			 */
			vec_t x() const
			{
				return mPosition.x;
			}

			/**
			 * Return rectangle position in y axis.
			 */
			vec_t y() const
			{
				return mPosition.y;
			}

			/**
			 * Return rectangle width
			 */
			vec_t w() const
			{
				return mDimension.x;
			}

			/**
			 * Return rectangle height
			 */
			vec_t h() const
			{
				return mDimension.y;
			}

			/**
			 * Print rectangle information.
			 */
			void cout() const
			{
				printf("x: %f, y: %f\n", mPosition.x, mPosition.y);
				printf("w: %f, h: %f\n", mDimension.x, mDimension.y);
			}
	};

	/**
	 * \brief A virtual representation of a drawable 2D entity.
	 */
	class DLL_EXPORT drawable2D
	{
		protected:
			rectangle mRectangle;
			vec_t mZ;

			/**
			 * Region to be drawn
			 */
			vector2 mDrawRegionPos;
			vector2 mDrawRegionSize;

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
			/**
			 * Constructor
			 */
			drawable2D();

			/**
			 * Destructor
			 */
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
			 * Set drawable scale. Keep in mind that this will reset the draw
			 * region to cover the full drawable area.
			 *
			 * @param size The new drawable dimensions in global coordinates (0, ScreenWidth/Height)
			 */
			void setScale(const vector2& size);

			/**
			 * Set the drawable draw area. The drawable material will not
			 * be stretched to fit the area, instead it will be cut if the size
			 * is smaller than the scale region.
			 */
			void setVisibleArea(const vector2& position, const vector2& size);

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

			/**
			 * Draw
			 */
			virtual void draw() = 0;
	};

	/**
	 * \brief Defines a rectangular region.
	 * The bounding box defines a rectangular region in the space.
	 */
	class DLL_EXPORT boundingBox
	{
		private:
			vector3 mMins;
			vector3 mMaxs;

		public:
			/**
			 * Creates a boundingBox with edges on zero.
			 */
			boundingBox();

			/**
			 * Creates a boundingBox with the edges set.
			 * @param min The minimum point.
			 * @param max The maximum point.
			 */
			boundingBox(const vector3& min, const vector3& max);

			/**
			 * Sum this bounding box with another one
			 */
			boundingBox operator + (const boundingBox& b);

			/**
			 * Sum this bounding box with another one
			 */
			boundingBox& operator += (const boundingBox& b);

			/**
			 * Subtract another bounding box from this one
			 */
			boundingBox operator - (const boundingBox& b);

			/**
			 * Subtract another bounding box from this one
			 */
			boundingBox& operator -= (const boundingBox& b);

			/**
			 * Test if min is lower than the box minimum and set it if true.
			 */
			void setTestMins(const vector3& min);

			/**
			 * Test if max is greater than the box minimum and set it if true.
			 */
			void setTestMaxs(const vector3& max);

			/**
			 * Test if dist is lower than minimum and set minimum if true or if its
			 * greater than maximum and set maximum if true.
			 */
			void setTest(const vector3& dist);

			/**
			 * Set box minimum.
			 */
			void setMins(const vector3& min);

			/**
			 * Set box maximum.
			 */
			void setMaxs(const vector3& max);

			/**
			 * Returns minimum
			 */
			const vector3& getMins() const;

			/**
			 * Returns maximum
			 */
			const vector3& getMaxs() const;

			/**
			 * Draw the bounding box edges.
			 */
			void draw() const;
	};

	/**
	 * \brief A virtual representation of a drawable 3D entity.
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
			/**
			 * Constructor
			 */
			drawable3D();

			/**
			 * Destructor
			 */
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

