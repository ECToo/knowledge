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

#include "drawable.h"
#include "renderer.h"

namespace k {

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
	renderer* mRenderer = &renderer::getSingleton();
	if (mRenderer)
		mRenderer->sort2D();
}

const vec_t drawable2D::getZ()
{
	return mZ;
}

void drawable2D::setScale(const vector2& size)
{
	mScale = size;
}

const vector2& drawable2D::getPosition()
{
	return mPosition;
}

const vec_t drawable2D::getRotation()
{
	return mRotation;
}

const vector2& drawable2D::getDimension()
{
	return mScale;
}

drawable2D::~drawable2D()
{
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

const vector3& drawable3D::getScale()
{
	return mScale;
}

const vector3& drawable3D::getPosition()
{
	return mScale;
}
		
const quaternion& drawable3D::getOrientation()
{
	return mOrientation;
}

drawable3D::~drawable3D()
{
}

}

