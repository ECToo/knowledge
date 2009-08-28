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

	mRadius = radi;
	mMaterial = mat;	
	mInvalidTransPos = false;
	mSpriteVisible = true;

	// Generate mTransPos
	calculateTransPos();
}

sprite::sprite(const std::string& mat, vec_t radi)
{
	mRadius = radi;
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

	vector3 sprX = mCam->getRight().negate();
	vector3 sprY = mCam->getUp();

	mTransPos.m[0][0] = sprX.x;
	mTransPos.m[1][0] = sprX.y;
	mTransPos.m[2][0] = sprX.z;
	
	mTransPos.m[0][1] = sprY.x;
	mTransPos.m[1][1] = sprY.y;
	mTransPos.m[2][1] = sprY.z;
	
	mTransPos.m[0][2] = sprZ.x;
	mTransPos.m[1][2] = sprZ.y;
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

	}

	mRadius = rad;
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

		rs->startVertices(VERTEXMODE_POINTS);
		rs->vertex(mPosition);
		rs->endVertices();

		rs->setPointSprite(false);

		mMaterial->finish();
		rs->setDepthMask(true);

		return;
	}

	if (mInvalidTransPos)
		calculateTransPos();

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->multMatrix(mTransPos);

	// We dont need depth masking on this case
	rs->setDepthMask(false);

	mMaterial->start();
	rs->setCulling(CULLMODE_NONE);

	const vec_t uv[] ATTRIBUTE_ALIGN(32) = {0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0};
	const vec_t vertex[] ATTRIBUTE_ALIGN(32) = {-mRadius, -mRadius, 0, mRadius, -mRadius, 0,
		mRadius, mRadius, 0, mRadius, mRadius, 0, -mRadius, mRadius, 0,
		-mRadius, -mRadius, 0 };

	const index_t index[] ATTRIBUTE_ALIGN(32) = {0, 1, 2, 3, 4, 5}; 

	rs->clearArrayDesc();
	rs->setVertexArray(vertex);
	rs->setVertexCount(6);

	rs->setTexCoordArray(uv);

	rs->setVertexIndex(index);
	rs->setIndexCount(6);

	rs->drawArrays();

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

		rs->startVertices(VERTEXMODE_POINTS);
		rs->vertex(mPosition);
		rs->endVertices();

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

	const index_t index[] ATTRIBUTE_ALIGN(32) = {0, 1, 2, 3, 4, 5}; 

	rs->clearArrayDesc();
	rs->setVertexArray(vertex);
	rs->setVertexCount(6);

	rs->setTexCoordArray(uv);

	rs->setVertexIndex(index);
	rs->setIndexCount(6);

	rs->drawArrays();
}

}

