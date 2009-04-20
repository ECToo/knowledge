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
	mMaterial = root::getSingleton().getMaterialManager()->createMaterial(matName);
	if (!mMaterial)
		S_LOG_INFO("Failed to create sticker material.");
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

	// Prepare the material
	mMaterial->prepare();
	rs->setCulling(CULLMODE_NONE);

	rs->setDepthMask(false);
	rs->startVertices(VERTEXMODE_QUAD);

	rs->texCoord(vector2(0.0f, 0.0f));
	rs->vertex(vector3(mPosition.x, mPosition.y, -0.5));

	rs->texCoord(vector2(1.0f, 0.0f));
	rs->vertex(vector3(mPosition.x + mScale.x, mPosition.y, -0.5));

	rs->texCoord(vector2(1.0f, 1.0f));
	rs->vertex(vector3(mPosition.x + mScale.x, mPosition.y + mScale.y, -0.5));

	rs->texCoord(vector2(0.0f, 1.0f));
	rs->vertex(vector3(mPosition.x, mPosition.y + mScale.y, -0.5));

	rs->endVertices();
	rs->setDepthMask(true);

	// Finish
	mMaterial->finish();
}

}

