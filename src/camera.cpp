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

#include "camera.h"
#include "root.h"
#include "logger.h"
#include "rendersystem.h"
#include "matrix3.h"

namespace k {

camera::camera()
{
	// Set the default rendering options
	// like fov, aspect ratio.
	mFov = 90;
	mAspectRatio = 1.33;
	mNearPlane = 0.1;
	mFarPlane = 1000;
}
			
void camera::setFov(unsigned int fov)
{
	if (!fov)
		S_LOG_INFO("Warning: Setting camera fov to 0!");

	mFov = fov;
}

void camera::setAspectRatio(vec_t ar)
{
	mAspectRatio = ar;
}

void camera::setPlanes(vec_t near, vec_t far)
{
	mNearPlane = near;
	mFarPlane = far;
}

void camera::setPerspective(unsigned int fov, vec_t ar, vec_t near, vec_t far)
{
	if (!fov)
		S_LOG_INFO("Warning, setting camera fov to 0!");

	mFov = fov;
	mAspectRatio = ar;
	mNearPlane = near;
	mFarPlane = far;
}

void camera::setPerspective()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	rs->identityMatrix();
	rs->setPerspective(mFov, mAspectRatio, mNearPlane, mFarPlane);
}
			
bool camera::isSphereInsideFrustum(const vector3& center, vec_t radius)
{
	for (unsigned short i = 0; i < 6; i++)
	{
		if ((mFrustumPlanes[i].dotProduct(center) + mFrustumDs[i]) < -radius)
			return false;
	}

	return true;
}

bool camera::isPointInsideFrustum(const vector3& point)
{
	for (unsigned short i = 0; i < 6; i++)
	{
		if ((mFrustumPlanes[i].dotProduct(point) + mFrustumDs[i]) < 0)
			return false;
	}

	return true;
}

void camera::lookAt(vector3 pos)
{
	// Find the Direction
	vector3 dirZ = mPosition - pos;
	dirZ.normalise();

	// Direction of "right" can be determined by 
	// the up vector cross the forward direction
	vector3 up = vector3::unit_y;
	vector3 dirX = up.crossProduct(dirZ);

	// Same goes to the up direction
	vector3 dirY = dirZ.crossProduct(dirX);

	matrix3 mat(dirX, dirY, dirZ);
	mOrientation = quaternion(mat);

	// 
	std::stringstream vec;
	vec << dirZ.x << ", " << dirZ.y << ", " << dirZ.z;
	S_LOG_INFO(vec.str());

	setView();
}

void camera::setView()
{
	matrix4 mRotation = mOrientation.toMatrix();

	#ifndef __WII__

	matrix4 mTranslation;
	mTranslation.m[3][0] = -mPosition.x;
	mTranslation.m[3][1] = -mPosition.y;
	mTranslation.m[3][2] = -mPosition.z;
	mFinal = mTranslation * mRotation;

	#else

	vector3 look(-mRotation.m[0][2], -mRotation.m[1][2], -mRotation.m[2][2]);
	vector3 up(mRotation.m[0][1], mRotation.m[1][1], mRotation.m[2][1]);
	vector3 right(mRotation.m[0][0], mRotation.m[1][0], mRotation.m[2][0]);

	// 
	std::stringstream vec;
	vec << look.x << ", " << look.y << ", " << look.z;
	S_LOG_INFO(vec.str());

	mRotation.m[0][0] = right.x;
	mRotation.m[0][1] = right.y;
	mRotation.m[0][2] = right.z;

	mRotation.m[1][0] = up.x;
	mRotation.m[1][1] = up.y;
	mRotation.m[1][2] = up.z;

	mRotation.m[2][0] = look.x;
	mRotation.m[2][1] = look.y;
	mRotation.m[2][2] = look.z;

	mRotation.m[0][3] = -right.dotProduct(mPosition);
	mRotation.m[1][3] = -up.dotProduct(mPosition);
	mRotation.m[2][3] = -look.dotProduct(mPosition);

	mFinal = mRotation;
	#endif

	// View Frustum
	vec_t hFar = 2 * tan(mFov / 2) * mFarPlane;
	vec_t wFar = hFar * mAspectRatio;

	// Far Plane
	vector3 farCenter = mPosition + getDirection() * mFarPlane;

	vector3 farTopBase = (getUp() * hFar/2);
	vector3 farRightBase = (getRight() * wFar/2);

	vector3 farTopLeft = farCenter + farTopBase - farRightBase;
	vector3 farTopRight = farCenter + farTopBase + farRightBase;
	vector3 farBottomRight = farCenter - farTopBase + farRightBase;

	// Near Plane
	vector3 nearCenter = mPosition + getDirection() * mNearPlane;

	// Ok lets define the frustum planes
	mFrustumPlanes[PLANE_NEAR] = getDirection();
	mFrustumDs[PLANE_NEAR] = -getDirection().dotProduct(nearCenter);

	mFrustumPlanes[PLANE_FAR] = getDirection().negate();
	mFrustumDs[PLANE_FAR] = getDirection().dotProduct(nearCenter);

	mFrustumPlanes[PLANE_RIGHT] = getRight().negate();
	mFrustumDs[PLANE_RIGHT] = getRight().dotProduct(farTopRight);

	mFrustumPlanes[PLANE_LEFT] = getRight();
	mFrustumDs[PLANE_LEFT] = -getRight().dotProduct(farTopLeft);

	mFrustumPlanes[PLANE_TOP] = getUp().negate();
	mFrustumDs[PLANE_TOP] = getUp().dotProduct(farTopRight);

	mFrustumPlanes[PLANE_BOTTOM] = getUp();
	mFrustumDs[PLANE_BOTTOM] = -getUp().dotProduct(farBottomRight);
}

void camera::copyView()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);

	#ifdef __WII__
	rs->copyMatrix(mFinal.m);
	#else
	rs->copyMatrix(mFinal.m[0]);
	#endif
}

void camera::setPosition(vector3 pos)
{
	mPosition = pos;
	setView();
}

vector3& camera::getPosition()
{
	return mPosition;
}

// Orientation
void camera::setOrientation(quaternion ori)
{
	mOrientation = ori;
	setView();
}

quaternion& camera::getOrientation()
{
	return mOrientation;
}
			
vector3 camera::getDirection()
{
	return vector3(-mFinal.m[0][2], -mFinal.m[1][2], -mFinal.m[2][2]);
}
			
vector3 camera::getUp()
{
	return vector3(mFinal.m[0][1], mFinal.m[1][1], mFinal.m[2][1]);
}

vector3 camera::getRight()
{
	return vector3(mFinal.m[0][0], mFinal.m[1][0], mFinal.m[2][0]);
}

}

