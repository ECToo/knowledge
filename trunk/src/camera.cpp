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
#include "rendersystem.h"
#include "matrix3.h"

namespace k {

void camera::lookAt(vector3 pos)
{
	// Find the Direction
	vector3 dirZ = mPosition - pos;
	dirZ.normalise();

	// Direction of "right" can be determined by 
	// the up vector cross the forward direction
	vector3 up = vector3::unit_y;
	vector3 dirX = up.crossProduct(dirZ);
	dirX.normalise();

	// Same goes to the up direction
	vector3 dirY = dirZ.crossProduct(dirX);
	dirY.normalise();

	matrix3 mat(dirX, dirY, dirZ);
	mOrientation = quaternion(mat);

	setView();
}

void camera::setView()
{
	matrix4 mRotation = mOrientation.toMatrix();
	matrix4 mTranslation;
	mTranslation.m[3][0] = -mPosition.x;
	mTranslation.m[3][1] = -mPosition.y;
	mTranslation.m[3][2] = -mPosition.z;

	mFinal = mTranslation * mRotation;
	copyView();
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

}

