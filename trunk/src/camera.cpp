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

namespace k {

void camera::setView()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	mOrientation.toMatrix(mMatrix);

	#ifndef __WII__
	mMatrix[12] = mPosition.x;
	mMatrix[13] = mPosition.y;
	mMatrix[14] = mPosition.z;
	#else
	mMatrix[0][3] = mPosition.x;
	mMatrix[1][3] = mPosition.y;
	mMatrix[2][3] = mPosition.z;
	#endif

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->identityMatrix();
	rs->copyMatrix(mMatrix);
}

void camera::setPosition(vector3 pos)
{
	mPosition = pos;
}

vector3& camera::getPosition()
{
	return mPosition;
}

// Orientation
void camera::setOrientation(quaternion ori)
{
	mOrientation = ori;
}

quaternion& camera::getOrientation()
{
	return mOrientation;
}

}

