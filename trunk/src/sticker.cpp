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

#include "sticker.h"
#include "root.h"
#include "materialManager.h"

namespace k {
			
sticker::sticker(const std::string& matName)
{
	materialManager* mm = root::getSingleton().getMaterialManager();
	mMaterial = mm->createMaterial(matName);

	assert(mMaterial != NULL);

	mZ = 0;
}

sticker::~sticker()
{
}

void sticker::setZ(vec_t z)
{
	mZ = z;
}

vec_t sticker::getZ()
{
	return mZ;
}

material* sticker::getMaterial()
{
	return mMaterial;
}

void sticker::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	vec_t realY = mPosition.y;

	#ifndef __WII__
	// The position for openGL should be switched in
	// Y axis because it starts from bottom while on
	// wii it starts on the top.
	realY = rs->getScreenHeight() - mPosition.y;
	#endif

	// Prepare the material
	mMaterial->prepare();

	rs->startVertices(VERTEXMODE_QUAD);

	rs->texCoord(vector2(0.0f, 0.0f));
	rs->vertex(vector3(mPosition.x, realY, mZ));

	rs->texCoord(vector2(1.0f, 0.0f));
	rs->vertex(vector3(mPosition.x + mScale.x, realY, mZ));

	rs->texCoord(vector2(1.0f, 1.0f));
	rs->vertex(vector3(mPosition.x + mScale.x, realY + mScale.y, mZ));

	rs->texCoord(vector2(0.0f, 1.0f));
	rs->vertex(vector3(mPosition.x, realY + mScale.y, mZ));

	rs->endVertices();
}

}
