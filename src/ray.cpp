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
	if (!mInverseTransformation.isIdentity())
	{
		newDirection = mInverseTransformation * mDirection;
		newOrigin = mInverseTransformation * mOrigin;
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

