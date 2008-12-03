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

#ifndef __WII__
#include "textureLib.h"
#include "logger.h"
#include "root.h"

namespace k {

template<> textureLoader* singleton<textureLoader>::singleton_instance = 0;

textureLoader& textureLoader::getSingleton()
{  
	if (!singleton_instance)
		new textureLoader();

	assert(singleton_instance);
	return (*singleton_instance);  
}

void textureLoader::unLoadTexture(kTexture* tex)
{
	glDeleteTextures(1, tex);
}

bool textureLoader::loadTexture(char* file, kTexture* tex,
					unsigned short* w, unsigned short* h)
{
	ILuint* newImage = new ILuint;
	if (!newImage)
		return false;

	ilGenImages(1, newImage);
	ilBindImage(newImage[0]);
	ilLoadImage(file);

	if (ilGetError() == IL_NO_ERROR)
	{
		if (w && h)
		{
			*w = ilGetInteger(IL_IMAGE_WIDTH);
			*h = ilGetInteger(IL_IMAGE_HEIGHT);
		}

		ilBindImage(newImage[0]);
		ilutRenderer(ILUT_OPENGL);
	
		tex = new kTexture;
		if (tex)
		{
			*tex = ilutGLBindTexImage();
			ilDeleteImages(1, &newImage[0]);

			delete newImage;
			return true;
		}
		else
		{
			ilDeleteImages(1, &newImage[0]);
			delete newImage;
		}
	}

	delete newImage;
	return false;
}

}

#endif

