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

#ifndef _RAY_H_
#define _RAY_H_

#include "prerequisites.h"
#include "vector3.h"
#include "quaternion.h"
#include "drawable.h"

namespace k
{
	class ray
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
			quaternion mOrientation;

		public:
			ray() {}

			ray(vector3 origin, vector3 dir)
			{
				mOrigin = origin;
				mDirection = dir;
				mFraction = 0;
			}

			ray(const ray& oldRay)
			{
				mOrigin = oldRay.getOrigin();
				mDirection = oldRay.getDirection();
				mFraction = oldRay.getFraction();
				mOrientation = oldRay.getOrientation();
			}

			void setOrientation(const quaternion& quat)
			{
				mOrientation = quat;
			}

			void resetOrientation()
			{
				mOrientation.w = 1;
				mOrientation.x = mOrientation.y = mOrientation.z = 0;
			}

			const vector3& getOrigin() const
			{
				return mOrigin;
			}

			const vector3& getDirection() const
			{
				return mDirection;
			}

			const quaternion& getOrientation() const
			{
				return mOrientation;
			}

			const vec_t getFraction() const
			{
				return mFraction;
			}

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

