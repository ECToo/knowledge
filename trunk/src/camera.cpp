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

void camera::lookAt(vector3 pos)
{
	// Find the Direction
	vector3 dirZ = mPosition - pos;
	dirZ.normalise();

	// Direction of "right" can be determined by 
	// the up vector cross the forward direction
	vector3 dirX = vector3(0, 1, 0).crossProduct(dirZ);
	dirX.normalise();

	// Same goes to the up direction
	vector3 dirY = dirZ.crossProduct(dirX);
	dirY.normalise();

	memset(mMatrix, 0, sizeof(vec_t) * 16);

	mMatrix[0] = dirX.x;
	mMatrix[4] = dirX.y;
	mMatrix[8] = dirX.z;

	mMatrix[1] = dirY.x;
	mMatrix[5] = dirY.y;
	mMatrix[9] = dirY.z;

	mMatrix[2] = dirZ.x;
	mMatrix[6] = dirZ.y;
	mMatrix[10] = dirZ.z;

	mMatrix[15] = 1.0f;
}

void camera::setView()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	// mOrientation.toMatrix(mMatrix);

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->copyMatrix(mMatrix);
	rs->translateScene(-mPosition.x, -mPosition.y, -mPosition.z);
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

