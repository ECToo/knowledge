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
	mTanFov = tan(mFov * 0.5f);
	mAspectRatio = 1.333f;
	mNearPlane = 0.1;
	mFarPlane = 1000;
}
			
void camera::setFov(unsigned int fov)
{
	if (!fov)
		S_LOG_INFO("Warning: Setting camera fov to 0!");

	mFov = fov;
	mTanFov = tan(mFov * 0.5f);
}

void camera::setAspectRatio(vec_t ar)
{
	mAspectRatio = ar;
}

void camera::setPlanes(vec_t nearP, vec_t farP)
{
	mNearPlane = nearP;
	mFarPlane = farP;
}

void camera::setPerspective(unsigned int fov, vec_t ar, vec_t nearP, vec_t farP)
{
	if (!fov)
		S_LOG_INFO("Warning, setting camera fov to 0!");

	mFov = fov;
	mTanFov = tan(mFov * 0.5f);
	mAspectRatio = ar;
	mNearPlane = nearP;
	mFarPlane = farP;
}

void camera::setPerspective()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	kAssert(rs != NULL);

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
	vector3 dirZ = pos - mPosition;
	dirZ.normalise();

	// Direction of "right" can be determined by 
	// the up vector cross the forward direction
	vector3 up = vector3::unit_y;
	vector3 dirX = dirZ.crossProduct(up);
	dirX.normalise();

	// Same goes to the up direction
	vector3 dirY = dirX.crossProduct(dirZ);
	dirY.normalise();

	matrix3 mat(dirX, dirY, dirZ.negateItself());
	mOrientation = quaternion(mat);
	mOrientation.normalise();

	setView();
}

static inline vector3 getPlaneNormal(const vector3& a, const vector3& b, const vector3& c)
{
	vector3 dir1 = b - a;
	vector3 dir2 = c - a;

	vector3 result = dir1.crossProduct(dir2);
	result.normalise();

	return result;
}

void camera::setView()
{
	matrix4 mRotation = mOrientation.toMatrix();
	mFinal = mRotation;

	vector3 look(mFinal.m[0][2], mFinal.m[1][2], mFinal.m[2][2]);
	vector3 up(mFinal.m[0][1], mFinal.m[1][1], mFinal.m[2][1]);
	vector3 right(mFinal.m[0][0], mFinal.m[1][0], mFinal.m[2][0]);

	mFinal.m[3][0] = -right.dotProduct(mPosition);
	mFinal.m[3][1] = -up.dotProduct(mPosition);
	mFinal.m[3][2] = -look.dotProduct(mPosition);
	mFinal.m[3][3] = 1.0f;

	// We need the transpose
	mFinalInverse = mFinal.transpose().inverse();

	// View Frustum
	vec_t tanFov = 2 * mTanFov;
	vec_t hFar = tanFov * mFarPlane;
	vec_t wFar = hFar * mAspectRatio;

	vec_t hNear = tanFov * mNearPlane;
	vec_t wNear = hNear * mAspectRatio;

	// Far Plane
	vector3 farCenter = mPosition + getDirection() * mFarPlane;

	vector3 farTopBase = (getUp() * hFar/2);
	vector3 farRightBase = (getRight() * wFar/2);

	vector3 farTopLeft = farCenter + farTopBase - farRightBase;
	vector3 farTopRight = farCenter + farTopBase + farRightBase;
	vector3 farBottomRight = farCenter - farTopBase + farRightBase;

	// Near Plane
	vector3 nearCenter = mPosition + getDirection() * mNearPlane;

	vector3 nearTopBase = (getUp() * hNear/2);
	vector3 nearRightBase = (getRight() * wNear/2);

	vector3 nearTopLeft = nearCenter + nearTopBase - nearRightBase;
	vector3 nearTopRight = nearCenter + nearTopBase + nearRightBase;

	vector3 nearBottomLeft = nearCenter - nearTopBase - nearRightBase;
	vector3 nearBottomRight = nearCenter - nearTopBase + nearRightBase;

	// Ok lets define the frustum planes
	mFrustumPlanes[PLANE_NEAR] = getDirection();
	mFrustumDs[PLANE_NEAR] = -getDirection().dotProduct(nearCenter);

	mFrustumPlanes[PLANE_FAR] = getDirection().negate();
	mFrustumDs[PLANE_FAR] = getDirection().dotProduct(farCenter);

	mFrustumPlanes[PLANE_RIGHT] = getPlaneNormal(nearTopRight, farTopRight, nearBottomRight); 
	mFrustumDs[PLANE_RIGHT] = -mFrustumPlanes[PLANE_RIGHT].dotProduct(farTopRight);

	mFrustumPlanes[PLANE_LEFT] = getPlaneNormal(nearTopLeft, nearBottomLeft, farTopLeft);
	mFrustumDs[PLANE_LEFT] = -mFrustumPlanes[PLANE_LEFT].dotProduct(farTopLeft);

	mFrustumPlanes[PLANE_TOP] = getPlaneNormal(farTopRight, nearTopRight, farTopLeft);
	mFrustumDs[PLANE_TOP] = -mFrustumPlanes[PLANE_TOP].dotProduct(farTopRight);

	mFrustumPlanes[PLANE_BOTTOM] = getPlaneNormal(nearTopLeft, nearTopRight, farTopLeft);
	mFrustumDs[PLANE_BOTTOM] = -mFrustumPlanes[PLANE_BOTTOM].dotProduct(farBottomRight);
}
			
/**
 * This function is based on the article
 * "Projecting a Ray from 2D Screen Coordinates"
 * by Robert Dunlop - http://www.mvps.org/directx
 *
 * coords are in local screen coordinates from (0.0 to 1.0f)
 */
vector3 camera::projectRayFrom2D(const vector2& coords)
{
	vec_t dx = mTanFov * ((coords.x * 2.0f - 1.0f) / mAspectRatio);
	vec_t dy = mTanFov * ((coords.y * 2.0f - 1.0f) / -mAspectRatio);

	vector3 farP = vector3(dx * mFarPlane, dy * mFarPlane, -mFarPlane);
	vector3 nearP = vector3(dx * mNearPlane, dy * mNearPlane, -mNearPlane);

	vector3 result = mFinalInverse * (farP - nearP);
	result.normalise();

	return result;
}

void camera::copyView()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	kAssert(rs != NULL);

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->copyMatrix(mFinal);
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

