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

#include "drawable.h"
#include "renderer.h"
#include "root.h"
#include "materialManager.h"

namespace k {

drawable2D::drawable2D()
{
	mDrawableAttach = NULL;
	mDrawableVisible = true;
}
drawable2D::~drawable2D() {}

void drawable2D::setPosition(const vector2& pos)
{
	mRectangle.setPosition(pos);
}

void drawable2D::setRotation(const vec_t rot)
{
	mRotation = rot;
}
			
void drawable2D::setZ(vec_t z)
{
	mZ = z;

	// Resort 2D objects =]
	renderer::getSingleton().sort2D();
}

void drawable2D::setScale(const vector2& size)
{
	mRectangle.setDimension(size);
	mDrawRegionSize = size;
	mDrawRegionPos = vector2(0,0);
}
			
void drawable2D::setVisibleArea(const vector2& position, const vector2& size)
{
	mDrawRegionPos = position;
	mDrawRegionSize = size;
}

const vec_t drawable2D::getZ() const
{
	return mZ;
}
			
void drawable2D::attach(const drawable2D* target)
{
	kAssert(target);
	mDrawableAttach = target;
}

const drawable2D* drawable2D::getRoot() const
{
	return mDrawableAttach;
}

vector2 drawable2D::getAbsolutePosition() const
{
	if (mDrawableAttach)
		return mDrawableAttach->getAbsolutePosition() + mRectangle.getPosition();
	else
		return mRectangle.getPosition();
}

vec_t drawable2D::getAbsoluteRotation() const
{
	if (mDrawableAttach)
		return mDrawableAttach->getAbsoluteRotation() + mRotation;
	else
		return mRotation;
}

const vector2& drawable2D::getRelativePosition() const
{
	return mRectangle.getPosition();
}

const vec_t drawable2D::getRelativeRotation() const
{
	return mRotation;
}

const vector2& drawable2D::getDimension() const
{
	return mRectangle.getDimension();
}

boundingBox::boundingBox()
{}
			
boundingBox::boundingBox(const vector3& min, const vector3& max)
{
	mMins = min;
	mMaxs = max;
}
			
boundingBox boundingBox::operator + (const boundingBox& b)
{
	boundingBox newBox(mMins, mMaxs);
	newBox += b;

	return newBox;
}

boundingBox& boundingBox::operator += (const boundingBox& b)
{
	setTestMins(b.getMins());
	setTestMaxs(b.getMaxs());

	return *this;
}

boundingBox boundingBox::operator - (const boundingBox& b)
{
	boundingBox newBox(mMins, mMaxs);
	newBox -= b;

	return newBox;
}

boundingBox& boundingBox::operator -= (const boundingBox& b)
{
	mMins -= b.getMins();
	mMaxs -= b.getMaxs();

	return *this;
}
			
void boundingBox::setTest(const vector3& dist)
{
	setTestMins(dist);
	setTestMaxs(dist);
}

void boundingBox::setTestMins(const vector3& min)
{
	if (min.x < mMins.x)
		mMins.x = min.x;

	if (min.y < mMins.y)
		mMins.y = min.y;

	if (min.z < mMins.z)
		mMins.z = min.z;
}

void boundingBox::setTestMaxs(const vector3& max)
{
	if (max.x > mMaxs.x)
		mMaxs.x = max.x;

	if (max.y > mMaxs.y)
		mMaxs.y = max.y;

	if (max.z > mMaxs.z)
		mMaxs.z = max.z;
}

void boundingBox::setMins(const vector3& min)
{
	mMins = min;
}

void boundingBox::setMaxs(const vector3& max)
{
	mMaxs = max;
}

const vector3& boundingBox::getMins() const
{
	return mMins;
}

const vector3& boundingBox::getMaxs() const
{
	return mMaxs;
}
			
void boundingBox::draw()
{
	const vector3 v1 = mMins;
	const vector3 v2 = vector3(mMaxs.x, mMins.y, mMins.z);
	const vector3 v3 = vector3(mMaxs.x, mMins.y, mMaxs.z);
	const vector3 v4 = vector3(mMins.x, mMins.y, mMaxs.z);

	const vector3 v5 = vector3(mMins.x, mMaxs.y, mMins.z);
	const vector3 v6 = vector3(mMaxs.x, mMaxs.y, mMins.z);
	const vector3 v7 = mMaxs;
	const vector3 v8 = vector3(mMins.x, mMaxs.y, mMaxs.z);

	// Please romulo, do something better than that!
	mBoundingLines[0] = v1;
	mBoundingLines[1] = v2;
	mBoundingLines[2] = v2;
	mBoundingLines[3] = v3;
	mBoundingLines[4] = v3;
	mBoundingLines[5] = v4;
	mBoundingLines[6] = v4;
	mBoundingLines[7] = v1;
	mBoundingLines[8] = v5;
	mBoundingLines[9] = v6;
	mBoundingLines[10] = v6;
	mBoundingLines[11] = v7;
	mBoundingLines[12] = v7;
	mBoundingLines[13] = v8;
	mBoundingLines[14] = v8;
	mBoundingLines[15] = v5;
	mBoundingLines[16] = v1;
	mBoundingLines[17] = v5;
	mBoundingLines[18] = v2;
	mBoundingLines[19] = v6;
	mBoundingLines[20] = v3;
	mBoundingLines[21] = v7;
	mBoundingLines[22] = v4;
	mBoundingLines[23] = v8;

	material* boundMaterial = materialManager::getSingleton().getMaterial("k_base_white");
	if (boundMaterial) boundMaterial->start();

	renderSystem* rs = root::getSingleton().getRenderSystem();
	rs->translateScene(0, mMaxs.y, 0);

	rs->clearArrayDesc(VERTEXMODE_LINE);
	rs->setVertexArray(mBoundingLines[0].vec);
	rs->setVertexCount(24);
	rs->drawArrays(true);

	if (boundMaterial) boundMaterial->finish();
}

void drawable3D::setOrientation(const quaternion& orientation)
{
	mOrientation = orientation;
}

void drawable3D::setScale(const vector3& scale)
{
	mScale = scale;
}

void drawable3D::setScale(const vec_t scale)
{
	mScale = vector3(scale, scale, scale);
}

void drawable3D::setPosition(const vector3& pos)
{
	mPosition = pos;
}

const vector3& drawable3D::getScale() const
{
	return mScale;
}

vector3 drawable3D::getAbsolutePosition() const
{
	if (mDrawableAttach)
		return mDrawableAttach->getAbsolutePosition() + mPosition;
	else
		return mPosition;
}
		
quaternion drawable3D::getAbsoluteOrientation() const
{
	if (mDrawableAttach)
		return mDrawableAttach->getAbsoluteOrientation() * mOrientation;
	else
		return mOrientation;
}

const vector3& drawable3D::getRelativePosition() const
{
	return mPosition;
}
		
const quaternion& drawable3D::getRelativeOrientation() const
{
	return mOrientation;
}
			
drawable3D::drawable3D()
{
	mDrawAABB = false;
	mDrawableAttach = NULL;
	mScale = vector3(1, 1, 1);
	mDrawableVisible = true;
}

drawable3D::~drawable3D()
{
}
			
void drawable3D::setDrawBoundingBox(bool option)
{
	mDrawAABB = option;
}

bool drawable3D::getDrawBoundingBox() const
{
	return mDrawAABB;
}

void drawable3D::attach(const drawable3D* target)
{
	mDrawableAttach = target;
}

const drawable3D* drawable3D::getRoot() const
{
	return mDrawableAttach;
}

}

