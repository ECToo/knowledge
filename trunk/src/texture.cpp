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

namespace k {

#ifndef __WII__
texture::texture(ILuint* src, unsigned int width, unsigned int height)
#else
texture::texture(GXTexObj* src, unsigned int width, unsigned int height)
#endif
{
	assert(src != NULL);

	#ifndef __WII__
	mDevilTextureId = src;
	ilBindImage(*mDevilTextureId);
	ilutRenderer(ILUT_OPENGL);
	mTextureId = ilutGLBindTexImage();
	#else
	mTextureId = src;
	#endif

	mWidth = width;
	mHeight = height;
}

#ifndef __WII__
GLuint& texture::getTextureId()
#else
GXTexObj* texture::getTextureId()
#endif
{
	return mTextureId;
}
			
unsigned int texture::getWidth()
{
	return mWidth;
}

unsigned int texture::getHeight()
{
	return mHeight;
}

void texture::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	rs->bindTexture(mTextureId);
}

}

