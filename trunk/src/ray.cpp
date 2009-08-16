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

#include "ray.h"

namespace k {
			
/**
 * This one is based on Moller and Trumbore
 */
bool ray::intersect(const vector3& v1, const vector3& v2, const vector3& v3)
{
	vector3 newDirection = mDirection;
	vector3 newOrigin = mOrigin;

	// Change coordinates of the ray
	if (mOrientation != quaternion::identity)
	{
		newDirection = mOrientation.rotateVector(mDirection);
		newOrigin = mOrientation.rotateVector(mOrigin);
	}

	// reset fraction
	mFraction = 0;

	// Make plane from triangle vertices
	vector3 edge1 = v2 - v1;
	vector3 edge2 = v3 - v1;

	// Start calculating the determinant, needed for U
	vector3 pvec = newDirection.crossProduct(edge2);
	vec_t det = edge1.dotProduct(pvec);

	if (det > -0.00001 && det < 0.00001)
		return false;

	vec_t inv_det = 1.0f / det;
	
	// Distance from v1 to ray origin
	vector3 tvec = newOrigin - v1;
	vec_t u = inv_det * tvec.dotProduct(pvec);

	if (u < 0.0 || u > 1.0)
		return false;

	vector3 qvec = tvec.crossProduct(edge1);
	vec_t v = inv_det * newDirection.dotProduct(qvec);
	if (v < 0.0 || (u + v) > 1.0)
		return false;

	vec_t distance = inv_det * edge2.dotProduct(qvec);
	if (distance > 0.00001)
	{
		mFraction = distance;
		return true;
	}
		
	return false;
}

bool ray::intersect(const boundingBox& bb)
{
	//TODO
	return false;
}

}

