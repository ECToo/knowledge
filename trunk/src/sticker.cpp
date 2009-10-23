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

	// Allocate memory for drawing data - wii needs memaligned memory
	mUvs = (vec_t*) memalign(32, sizeof(vec_t) * 8);
	if (!mUvs)
	{
		S_LOG_INFO("Failed to allocate memory for sticker uvw arrays.");
		return;
	}

	mVertices = (vec_t*) memalign(32, sizeof(vec_t) * 12);
	if (!mVertices)
	{
		S_LOG_INFO("Failed to allocate memory for sticker vertex arrays.");
		return;
	}

	mVertices[2] = mVertices[5] = mVertices[8] = mVertices[11] = -0.5;
}

sticker::~sticker()
{
	if (mUvs)
		free(mUvs);

	if (mVertices)
		free(mVertices);
}

material* sticker::getMaterial()
{
	return mMaterial;
}

void sticker::draw()
{
	if (!mVertices || !mUvs)
	{
		S_LOG_INFO("Invalid pointer to vertex or uv array.");
		return;
	}

	const vector2 mScale = mRectangle.getDimension();
	const vector2 mPosition = mRectangle.getPosition();

	if (mScale.x == 0 || mScale.y == 0)
		return;

	// Final position for vertices
	vector2 finalPos;
	if (mPosition.x + mDrawRegionPos.x + mDrawRegionSize.x > mPosition.x + mScale.x)
		finalPos.x = mPosition.x + mScale.x;
	else
		finalPos.x = mPosition.x + mDrawRegionPos.x + mDrawRegionSize.x;

	if (mPosition.y + mDrawRegionPos.y + mDrawRegionSize.y > mPosition.y + mScale.y)
		finalPos.y = mPosition.y + mScale.y;
	else
		finalPos.y = mPosition.y + mDrawRegionPos.y + mDrawRegionSize.y;

	// Setup draw arrays
	mUvs[0] = mDrawRegionPos.x / mScale.x; 
	mUvs[1] = mDrawRegionPos.y / mScale.y;

	mUvs[2] = (mDrawRegionPos.x + mDrawRegionSize.x) / mScale.x;
	mUvs[3] = mDrawRegionPos.y / mScale.y;

	mUvs[4] = (mDrawRegionPos.x + mDrawRegionSize.x) / mScale.x;
	mUvs[5] = (mDrawRegionPos.y + mDrawRegionSize.y) / mScale.y;

	mUvs[6] = mDrawRegionPos.x / mScale.x;
	mUvs[7] = (mDrawRegionPos.y + mDrawRegionSize.y) / mScale.y;

	mVertices[0] = mPosition.x + mDrawRegionPos.x; 
	mVertices[1] = mPosition.y + mDrawRegionPos.y;

	mVertices[3] = finalPos.x; 
	mVertices[4] = mPosition.y + mDrawRegionPos.y;
		
	mVertices[6] = finalPos.x;
	mVertices[7] = finalPos.y;
		
	mVertices[9] = mPosition.x + mDrawRegionPos.x;
	mVertices[10] = finalPos.y;

	// Get rendersystem
	renderSystem* rs = root::getSingleton().getRenderSystem();

	// Prepare the material and draw
	mMaterial->start();
	rs->setCulling(CULLMODE_NONE);
	rs->setDepthMask(false);

	rs->clearArrayDesc(VERTEXMODE_QUAD);
	rs->setVertexArray(mVertices);
	rs->setVertexCount(4);

	rs->setTexCoordArray(mUvs);

	rs->drawArrays(true);

	// Finish
	rs->setDepthMask(true);
	mMaterial->finish();
}

}

