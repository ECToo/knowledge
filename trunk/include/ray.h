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

#ifndef _RAY_H_
#define _RAY_H_

#include "prerequisites.h"
#include "vector3.h"
#include "matrix4.h"
#include "drawable.h"

namespace k
{
	/**
	 * \brief Casting rays for collision tests.
	 * Rays are used to execute collision tests against boxes, spheres, triangles, etc.
	 */
	class DLL_EXPORT ray
	{
		protected:
			vector3 mOrigin;
			vector3 mDirection;

			/**
			 * If the ray collided with something, mFraction
			 * will be written and will be the distance
			 * from the mOrigin to Collision point.
			 *
			 * P = mOrigin + mDirection * mFraction;
			 */
			vec_t mFraction;

			/**
			 * Temporary orientation, in case something is 
			 * another way oriented, you must give the ray the same
			 * orientation (put them in the same space)
			 */
			matrix4 mInverseTransformation;

		public:
			ray() {}

			/**
			 * Create a ray from a position and direction.
			 * @param origin The ray origin.
			 * @param dir The ray direction.
			 */
			ray(vector3 origin, vector3 dir)
			{
				mOrigin = origin;
				mDirection = dir;
				mFraction = 0;
			}

			/**
			 * Create a ray from another ray (copying).
			 */
			ray(const ray& oldRay)
			{
				mOrigin = oldRay.getOrigin();
				mDirection = oldRay.getDirection();
				mFraction = oldRay.getFraction();
				mInverseTransformation = oldRay.getTransformation();
			}

			/**
			 * Set ray origin.
			 * @param origin The new ray origin.
			 */
			void setOrigin(const vector3& origin)
			{
				mOrigin = origin;
			}

			/**
			 * Set ray direction.
			 * @param dir The new ray direction.
			 */
			void setDirection(const vector3& dir)
			{
				mDirection = dir;
			}

			/**
			 * Set the new ray space, multiplying its coords
			 * by the transformation matrix.
			 */
			void setTransformation(const matrix4& mat)
			{
				mInverseTransformation = mat;
			}

			/**
			 * Reset ray orientation to world space.
			 */
			void resetOrientation()
			{
				mInverseTransformation.setIdentity();
			}

			/**
			 * Returns ray origin.
			 */
			const vector3& getOrigin() const
			{
				return mOrigin;
			}

			/**
			 * Returns ray Direction.
			 */
			const vector3& getDirection() const
			{
				return mDirection;
			}

			/**
			 * Returns ray transformation (to change ray coord space into another one).
			 */
			const matrix4& getTransformation() const
			{
				return mInverseTransformation;
			}

			/**
			 * Returns how far the ray collided from getOrigin() in getDirection()
			 * In case the ray collided with something, getFraction()
			 * will return a value != 0.
			 */
			const vec_t getFraction() const
			{
				return mFraction;
			}

			/**
			 * Returns the contact point in case of ray collision.
			 */
			const vector3 getContactPoint() const
			{
				return mOrigin + mDirection * mFraction;
			}

			/**
			 * Test against triangle vertices.
			 */
			bool intersect(const vector3& v1, const vector3& v2, const vector3& v3);

			/**
			 * Test against bounding box
			 */
			bool intersect(const boundingBox& bb);
	};
}

#endif

