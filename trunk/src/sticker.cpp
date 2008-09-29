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
}

sticker::~sticker()
{
}

material* sticker::getMaterial()
{
	return mMaterial;
}

void sticker::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	// Prepare the material
	mMaterial->prepare();

	rs->startVertices(VERTEXMODE_QUAD);

	#ifdef __WII__
	rs->texCoord(vector2(0.0f, 0.0f));
	rs->vertex(vector3(mPosition.x, mPosition.y, 0));

	rs->texCoord(vector2(1.0f, 0.0f));
	rs->vertex(vector3(mPosition.x + mScale.x, mPosition.y, 0));

	rs->texCoord(vector2(1.0f, 1.0f));
	rs->vertex(vector3(mPosition.x + mScale.x, mPosition.y + mScale.y, 0));

	rs->texCoord(vector2(0.0f, 1.0f));
	rs->vertex(vector3(mPosition.x, mPosition.y + mScale.y, 0));
	#else
	rs->texCoord(vector2(0.0f, 1.0f));
	rs->vertex(vector3(mPosition.x, mPosition.y, 0));

	rs->texCoord(vector2(1.0f, 1.0f));
	rs->vertex(vector3(mPosition.x + mScale.x, mPosition.y, 0));

	rs->texCoord(vector2(1.0f, 0.0f));
	rs->vertex(vector3(mPosition.x + mScale.x, mPosition.y + mScale.y, 0));

	rs->texCoord(vector2(0.0f, 0.0f));
	rs->vertex(vector3(mPosition.x, mPosition.y + mScale.y, 0));
	#endif

	rs->endVertices();
}

}

