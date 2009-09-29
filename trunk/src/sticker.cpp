/*
Copyright (c) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
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

