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

#include "drawable.h"
#include "renderer.h"
#include "root.h"
#include "materialManager.h"

namespace k {

drawable2D::drawable2D()
{
	mAttach = NULL;
}
drawable2D::~drawable2D() {}

void drawable2D::setPosition(const vector2& pos)
{
	mPosition = pos;
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
	mScale = size;
}

const vec_t drawable2D::getZ() const
{
	return mZ;
}
			
void drawable2D::attach(const drawable2D* target)
{
	kAssert(target);
	mAttach = target;
}

const drawable2D* drawable2D::getRoot() const
{
	return mAttach;
}

vector2 drawable2D::getAbsolutePosition() const
{
	if (mAttach)
		return mAttach->getAbsolutePosition() + mPosition;
	else
		return mPosition;
}

vec_t drawable2D::getAbsoluteRotation() const
{
	if (mAttach)
		return mAttach->getAbsoluteRotation() + mRotation;
	else
		return mRotation;
}

const vector2& drawable2D::getRelativePosition() const
{
	return mPosition;
}

const vec_t drawable2D::getRelativeRotation() const
{
	return mRotation;
}

const vector2& drawable2D::getDimension() const
{
	return mScale;
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
	boundingBox newBox(mMins + b.getMins(), mMaxs + b.getMaxs());
	return newBox;
}

boundingBox& boundingBox::operator += (const boundingBox& b)
{
	mMins += b.getMins();
	mMaxs += b.getMaxs();

	return *this;
}

boundingBox boundingBox::operator - (const boundingBox& b)
{
	boundingBox newBox(mMins - b.getMins(), mMaxs - b.getMaxs());
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
			
void boundingBox::draw() const
{
	const vector3 v1 = mMins;
	const vector3 v2 = vector3(mMaxs.x, mMins.y, mMins.z);
	const vector3 v3 = vector3(mMaxs.x, mMins.y, mMaxs.z);
	const vector3 v4 = vector3(mMins.x, mMins.y, mMaxs.z);

	const vector3 v5 = vector3(mMins.x, mMaxs.y, mMins.z);
	const vector3 v6 = vector3(mMaxs.x, mMaxs.y, mMins.z);
	const vector3 v7 = mMaxs;
	const vector3 v8 = vector3(mMins.x, mMaxs.y, mMaxs.z);

	material* boundMaterial = materialManager::getSingleton().getMaterial("k_base_white");
	if (boundMaterial) boundMaterial->start();

	renderSystem* rs = root::getSingleton().getRenderSystem();
	rs->translateScene(0, mMaxs.y, 0);
	rs->startVertices(VERTEXMODE_LINE);
		// Base
		rs->vertex(v1);
		rs->vertex(v2);

		rs->vertex(v2);
		rs->vertex(v3);

		rs->vertex(v3);
		rs->vertex(v4);

		rs->vertex(v4);
		rs->vertex(v1);

		// Top
		rs->vertex(v5);
		rs->vertex(v6);

		rs->vertex(v6);
		rs->vertex(v7);

		rs->vertex(v7);
		rs->vertex(v8);

		rs->vertex(v8);
		rs->vertex(v5);

		// Vertical Edges
		rs->vertex(v1);
		rs->vertex(v5);

		rs->vertex(v2);
		rs->vertex(v6);

		rs->vertex(v3);
		rs->vertex(v7);

		rs->vertex(v4);
		rs->vertex(v8);
	rs->endVertices();

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
	if (mAttach)
		return mAttach->getAbsolutePosition() + mPosition;
	else
		return mPosition;
}
		
quaternion drawable3D::getAbsoluteOrientation() const
{
	if (mAttach)
		return mAttach->getAbsoluteOrientation() * mOrientation;
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
	mAttach = NULL;
	mScale = vector3(1, 1, 1);
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
	mAttach = target;
}

const drawable3D* drawable3D::getRoot() const
{
	return mAttach;
}

}

