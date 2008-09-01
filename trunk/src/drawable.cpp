#include "drawable.h"

namespace k {

void drawable2D::setPosition(const vector2& pos)
{
	mPosition = pos;
}

void drawable2D::setRotation(const vec_t rot)
{
	mRotation = rot;
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

