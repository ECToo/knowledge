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

#include "texture.h"
#include "root.h"
#include "logger.h"

namespace k {

textureStage::textureStage(unsigned int width, unsigned int height, unsigned short index)
{
	mIndex = index;
	mAngle = 0;
	mWidth = width;
	mHeight = height;
	mBlendSrc = 0;
	mBlendDst = 0;
	mRotate = 0;
	mScroll.x = 0;
	mScroll.y = 0;
}
			
void textureStage::setBlendMode(unsigned short src, unsigned short dst)
{
	mBlendSrc = src;
	mBlendDst = dst;
}

unsigned int textureStage::getWidth()
{
	return mWidth;
}

void textureStage::setProgram(const std::string& name)
{
	mProgram = name;	
}

unsigned int textureStage::getHeight()
{
	return mHeight;
}
			
void textureStage::setTexCoordType(texCoordType type)
{
	mTexCoordType = type;
}

void textureStage::setScroll(vector2 scroll)
{
	mScroll = scroll;
}

void textureStage::setRotate(vec_t angle)
{
	mRotate = angle;
}
			
texCoordType textureStage::getTexCoordType()
{
	return mTexCoordType;
}

}

