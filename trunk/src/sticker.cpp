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

#include "sticker.h"
#include "root.h"
#include "materialManager.h"

namespace k {
			
sticker::sticker(const std::string& matName)
{
	mMaterial = root::getSingleton().getMaterialManager()->createMaterial(matName);
	if (!mMaterial)
		S_LOG_INFO("Failed to create sticker material.");

	mZ = 0;
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
	const vector2 mScale = mRectangle.getDimension();
	const vector2 mPosition = mRectangle.getPosition();

	if (mScale.x == 0 || mScale.y == 0)
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();

	vector2 finalPos;
	if (mPosition.x + mDrawRegionPos.x + mDrawRegionSize.x > mPosition.x + mScale.x)
		finalPos.x = mPosition.x + mScale.x;
	else
		finalPos.x = mPosition.x + mDrawRegionPos.x + mDrawRegionSize.x;

	if (mPosition.y + mDrawRegionPos.y + mDrawRegionSize.y > mPosition.y + mScale.y)
		finalPos.y = mPosition.y + mScale.y;
	else
		finalPos.y = mPosition.y + mDrawRegionPos.y + mDrawRegionSize.y;

	// Prepare the material
	mMaterial->start();
	rs->setCulling(CULLMODE_NONE);

	rs->setDepthMask(false);
	rs->startVertices(VERTEXMODE_QUAD);

	rs->texCoord(vector2(mDrawRegionPos.x / mScale.x, mDrawRegionPos.y / mScale.y));
	rs->vertex(vector3(mPosition.x + mDrawRegionPos.x, mPosition.y + mDrawRegionPos.y, -0.5));

	rs->texCoord(vector2((mDrawRegionPos.x + mDrawRegionSize.x) / mScale.x, mDrawRegionPos.y / mScale.y));
	rs->vertex(vector3(finalPos.x, mPosition.y + mDrawRegionPos.y, -0.5));

	rs->texCoord(vector2((mDrawRegionPos.x + mDrawRegionSize.x) / mScale.x, (mDrawRegionPos.y + mDrawRegionSize.y) / mScale.y));
	rs->vertex(vector3(finalPos.x, finalPos.y, -0.5));

	rs->texCoord(vector2(mDrawRegionPos.x / mScale.x, (mDrawRegionPos.y + mDrawRegionSize.y) / mScale.y));
	rs->vertex(vector3(mPosition.x + mDrawRegionPos.x, finalPos.y, -0.5));

	rs->endVertices();
	rs->setDepthMask(true);

	// Finish
	mMaterial->finish();
}

}

