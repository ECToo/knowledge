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

#include "sprite.h"
#include "logger.h"
#include "root.h"
#include "camera.h"

namespace k {

sprite::sprite()
{
	mRadius = 0;
	mMaterial = NULL;	
	mInvalidTransPos = false;
	mSpriteVisible = true;
}

sprite::sprite(material* mat, vec_t radi)
{
	kAssert(mat);

	renderSystem* rs = root::getSingleton().getRenderSystem();
	if (rs->getPointSpriteSupport())
	{
		mRadius = radi;
	}
	else
	{
		mRadius = radi * (radi / rs->getScreenWidth());
	}

	mMaterial = mat;	
	mInvalidTransPos = false;
	mSpriteVisible = true;

	// Generate mTransPos
	calculateTransPos();
}

sprite::sprite(const std::string& mat, vec_t radi)
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	if (rs->getPointSpriteSupport())
	{
		mRadius = radi;
	}
	else
	{
		mRadius = radi * (radi / rs->getScreenWidth());
	}

	mMaterial = materialManager::getSingleton().getMaterial(mat);	
	mInvalidTransPos = false;
	mSpriteVisible = true;

	// Generate mTransPos
	calculateTransPos();
}

sprite::~sprite()
{
}

void sprite::calculateTransPos()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	if (rs->getPointSpriteSupport())
		return;

	// Render must be valid
	camera* mCam = root::getSingleton().getRenderer()->getCamera();
	if (!mCam)
	{
		// Camera is not valid - yet
		invalidate();
		return;
	}

	vector3 sprZ = mCam->getPosition() - mPosition;
	sprZ.normalize();

	k::vector3 tempUp = k::vector3::unit_y;
	vector3 sprX = sprZ.crossProduct(tempUp);
	sprX.normalize();

	vector3 sprY = sprX.crossProduct(sprZ);
	sprY.normalize();

	mTransPos.m[0][0] = sprX.x;
	mTransPos.m[0][1] = sprX.y;
	mTransPos.m[0][2] = sprX.z;
	
	mTransPos.m[1][0] = sprY.x;
	mTransPos.m[1][1] = sprY.y;
	mTransPos.m[1][2] = sprY.z;
	
	mTransPos.m[2][0] = sprZ.x;
	mTransPos.m[2][1] = sprZ.y;
	mTransPos.m[2][2] = sprZ.z;

	mTransPos.m[3][0] = mPosition.x;
	mTransPos.m[3][1] = mPosition.y;
	mTransPos.m[3][2] = mPosition.z;

	// TransPos is now valid =]
	mInvalidTransPos = false;
}
			
void sprite::setPosition(const vector3& pos)
{
	mPosition = pos;
	invalidate();
}

const vector3& sprite::getPosition() const
{
	return mPosition;
}

material* sprite::getMaterial() const
{
	return mMaterial;
}

const vec_t sprite::getRadius() const
{
	return mRadius;
}

void sprite::setMaterial(material* mat)
{
	kAssert(mat);
	mMaterial = mat;
}

void sprite::setMaterial(const std::string& mat)
{
	mMaterial = materialManager::getSingleton().getMaterial(mat);
}

void sprite::setRadius(vec_t rad)
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	if (rs->getPointSpriteSupport())
	{
		if (rad > rs->getPointSpriteMaxSize())
			S_LOG_INFO("Radius is greater than max supported sprite point size.");

		mRadius = rad;
	}
	else
	{
		// We need to convert screen coordinates to ranged sprite coordinates.
		mRadius = rad * (rad / rs->getScreenWidth());
	}
}

void sprite::invalidate()
{
	mInvalidTransPos = true;
}

void sprite::draw()
{
	if (!mRadius)
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();

	camera* mActiveCamera = root::getSingleton().getRenderer()->getCamera();
	if (!mActiveCamera)
	{
		rs->setMatrixMode(MATRIXMODE_MODELVIEW);
		rs->identityMatrix();
	}
	else
	{
		mActiveCamera->copyView();
	}

	if (rs->getPointSpriteSupport())
	{
		mMaterial->start();
		rs->setCulling(CULLMODE_NONE);

		rs->setDepthMask(false);
		rs->setPointSprite(true);
		rs->setPointSpriteSize(mRadius);

		rs->clearArrayDesc(VERTEXMODE_POINTS);
		rs->setVertexArray(mPosition.vec);
		rs->setVertexCount(1);
		rs->drawArrays(true);

		rs->setPointSprite(false);

		mMaterial->finish();
		rs->setDepthMask(true);

		return;
	}

	if (mInvalidTransPos)
		calculateTransPos();

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->multMatrix(mTransPos);
	rs->scaleScene(mRadius, mRadius, mRadius);

	mMaterial->start();

	// Culled sprites might not show, prevent it.
	rs->setCulling(CULLMODE_NONE);

	// Force depth test and disable depth mask.
	rs->setDepthTest(true);
	rs->setDepthMask(false);

	const vec_t uv[] ATTRIBUTE_ALIGN(32) = {
		0, 1, 
		0, 0, 
		1, 0, 
		1, 1 };

	const vec_t vertex[12] ATTRIBUTE_ALIGN(32) = {
		1.0f, -1.0f, 0,
		1.0f, 1.0f, 0,
		-1.0f, 1.0f, 0,
		-1.0f, -1.0f, 0 };

	rs->clearArrayDesc(VERTEXMODE_QUAD);
	rs->setVertexArray(vertex);
	rs->setVertexCount(4);

	rs->setTexCoordArray(uv);
	rs->drawArrays(true);

	rs->setDepthMask(true);
	mMaterial->finish();
}
			
void sprite::rawDraw()
{
	if (!mRadius)
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();
	if (rs->getPointSpriteSupport())
	{
		rs->setPointSprite(true);
		rs->setPointSpriteSize(mRadius);

		rs->clearArrayDesc(VERTEXMODE_POINTS);
		rs->setVertexArray(mPosition.vec);
		rs->setVertexCount(1);
		rs->drawArrays(true);

		rs->setPointSprite(false);

		return;
	}

	if (mInvalidTransPos)
		calculateTransPos();

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->multMatrix(mTransPos);

	const vec_t uv[] ATTRIBUTE_ALIGN(32) = {0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0};
	const vec_t vertex[] ATTRIBUTE_ALIGN(32) = {-mRadius, -mRadius, 0, mRadius, -mRadius, 0,
		mRadius, mRadius, 0, mRadius, mRadius, 0, -mRadius, mRadius, 0,
		-mRadius, -mRadius, 0 };

	rs->clearArrayDesc();
	rs->setVertexArray(vertex);
	rs->setVertexCount(6);

	rs->setTexCoordArray(uv);
	rs->drawArrays(true);
}

}

