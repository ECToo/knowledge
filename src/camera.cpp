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
	mTanFov = tan(DEG_TO_RAD(mFov * 0.5f));
	mAspectRatio = 1.3333f;
	mNearPlane = 0.1;
	mFarPlane = 5000;
}
			
void camera::setFov(unsigned int fov)
{
	if (!fov)
		S_LOG_INFO("Warning: Setting camera fov to 0!");

	mFov = fov;
	mTanFov = tan(DEG_TO_RAD(mFov * 0.5f));
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

	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	rs->identityMatrix();
	rs->setPerspective(mFov, mAspectRatio, mNearPlane, mFarPlane);
}
			
bool camera::isSphereInsideFrustum(const vector3& center, vec_t radius) const
{
	for (unsigned short i = 0; i < 6; i++)
	{
		if ((mFrustumPlanes[i].dotProduct(center) + mFrustumDs[i]) < -radius)
			return false;
	}

	return true;
}

bool camera::isPointInsideFrustum(const vector3& point) const
{
	for (unsigned short i = 0; i < 6; i++)
	{
		if ((mFrustumPlanes[i].dotProduct(point) + mFrustumDs[i]) < 0)
			return false;
	}

	return true;
}
			
bool camera::isBoxInsideFrustum(const boundingBox& AABB) const
{
	const vector3 mMins = AABB.getMins();
	const vector3 mMaxs = AABB.getMaxs();
	const vector3 boxPoints[8] = { mMins, vector3(mMaxs.x, mMins.y, mMins.z),
		vector3(mMaxs.x, mMins.y, mMaxs.z), vector3(mMins.x, mMins.y, mMaxs.z),
		vector3(mMins.x, mMaxs.y, mMins.z), vector3(mMaxs.x, mMaxs.y, mMins.z),
		mMaxs, vector3(mMins.x, mMaxs.y, mMaxs.z) };

	for (unsigned int i = 0; i < 8; i++)
	{
		if (isPointInsideFrustum(boxPoints[i]))
			return true;
	}

	return false;
}

void camera::lookAt(vector3 pos)
{
	// Find the Direction
	vector3 dirZ = pos - mPosition;
	dirZ.normalize();

	// Direction of "right" can be determined by 
	// the up vector cross the forward direction
	vector3 up = vector3::unit_y;
	vector3 dirX = dirZ.crossProduct(up);
	dirX.normalize();

	// Same goes to the up direction
	vector3 dirY = dirX.crossProduct(dirZ);
	dirY.normalize();

	matrix3 mat(dirX, dirY, dirZ.negateItself());
	mOrientation = quaternion(mat);
	mOrientation.normalize();

	setView();
}

static inline vector3 getPlaneNormal(const vector3& a, const vector3& b, const vector3& c)
{
	const vector3 dir1 = b - a;
	const vector3 dir2 = c - a;

	vector3 result = dir1.crossProduct(dir2);
	result.normalize();

	return result;
}

void camera::setView()
{
	mFinal = mOrientation.toMatrix();
	mOrientationInverse = mFinal.transpose().inverse();

	mFinal.m[3][0] = -getRight().dotProduct(mPosition);
	mFinal.m[3][1] = -getUp().dotProduct(mPosition);
	mFinal.m[3][2] = getDirection().dotProduct(mPosition);
	mFinal.m[3][3] = 1.0f;

	// We need the transpose
	mFinalInverse = mFinal.transpose().inverse();

	// View Frustum
	const vec_t tanFov = 2 * mTanFov;
	const vec_t hFar = tanFov * mFarPlane;
	const vec_t wFar = hFar * mAspectRatio;

	const vec_t hNear = tanFov * mNearPlane;
	const vec_t wNear = hNear * mAspectRatio;

	// Far Plane
	const vector3 farCenter = mPosition + getDirection() * mFarPlane;

	const vector3 farTopBase = (getUp() * hFar/2);
	const vector3 farRightBase = (getRight() * wFar/2);

	const vector3 farTopLeft = farCenter + farTopBase - farRightBase;
	const vector3 farTopRight = farCenter + farTopBase + farRightBase;
	const vector3 farBottomRight = farCenter - farTopBase + farRightBase;

	// Near Plane
	const vector3 nearCenter = mPosition + getDirection() * mNearPlane;

	const vector3 nearTopBase = (getUp() * hNear/2);
	const vector3 nearRightBase = (getRight() * wNear/2);

	const vector3 nearTopLeft = nearCenter + nearTopBase - nearRightBase;
	const vector3 nearTopRight = nearCenter + nearTopBase + nearRightBase;

	const vector3 nearBottomLeft = nearCenter - nearTopBase - nearRightBase;
	const vector3 nearBottomRight = nearCenter - nearTopBase + nearRightBase;

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
const ray camera::projectRayFrom2D(const vector2& coords) const
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	vector2 halfScreenSize(rs->getScreenWidth(), rs->getScreenHeight());
	halfScreenSize *= 0.5f;

	// mTanFov is the tangent of half of the fov.
	// Real coordinates [-1,1] - Notice that the article
	// is wrong and you must divide dy by aspect ratio, not X.
	// Otherwise Height will have the same dimension as width.
	vec_t dx = mTanFov * ((coords.x / halfScreenSize.x) - 1.0f);
	vec_t dy = mTanFov * (1.0f - (coords.y / halfScreenSize.y)) / mAspectRatio;

	vec_t farNearDiff = mFarPlane - mNearPlane;
	vector3 direction = mFinalInverse * vector3(dx * farNearDiff, dy * farNearDiff, -farNearDiff);
	direction.normalize();

	return ray(mPosition, direction);
}

void camera::copyView() const
{
	renderSystem* rs = root::getSingleton().getRenderSystem();

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->copyMatrix(mFinal);
	rs->setInverseTransposeModelview(mFinalInverse);
}
			
const matrix4& camera::getInverseTranspose() const
{
	return mFinalInverse;
}
			
const matrix4& camera::getRotInverseTranspose() const
{
	return mOrientationInverse;
}

void camera::setPosition(const vector3& pos)
{
	mPosition = pos;
	setView();
}

const vector3& camera::getPosition() const
{
	return mPosition;
}

// Orientation
void camera::setOrientation(const quaternion& ori)
{
	mOrientation = ori;
	setView();
}

const quaternion& camera::getOrientation() const
{
	return mOrientation;
}
			
const vector3 camera::getDirection() const
{
	vector3 direction(-mFinal.m[0][2], -mFinal.m[1][2], -mFinal.m[2][2]);
	direction.normalize();

	return direction;
}
			
const vector3 camera::getUp() const
{
	vector3 direction(mFinal.m[0][1], mFinal.m[1][1], mFinal.m[2][1]);
	direction.normalize();

	return direction;
}

const vector3 camera::getRight() const
{
	vector3 direction(mFinal.m[0][0], mFinal.m[1][0], mFinal.m[2][0]);
	direction.normalize();

	return direction;
}

}

