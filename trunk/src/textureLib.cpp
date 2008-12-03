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

#ifndef _TEXTURELIB_H_
#define _TEXTURELIB_H_

#include "textureLib.h"
#include "root.h"
#include "logger.h"

namespace k {

texture* createRawTexture(const std::string& filename)
{
	texture* newTexture = new texture;
	if (newTexture)
	{
		textureLoader* loader = &textureLoader::getSingleton();
		assert(loader != NULL);

		newTexture->mImagesCount = 1;

		kTexture* newKTexture = NULL;
		newKTexture = loader->loadTexture(filename.c_str(), &newTexture->mWidth, &newTexture->mHeight);

		if (newKTexture)
		{
			newTexture->mId.push_back(newKTexture);
			return newTexture;
		}
	}
		
	return NULL;
}

texture* createRawCubemap(const std::string& filename)
{
	textureLoader* texLoader = &textureLoader::getSingleton();
	if (!texLoader)
		return NULL;

	// Get Temp name
	std::string newName = filename;
	std::string extension = getExtension(filename);

	newName.erase(newName.length() - 4, 4);

	// Now we are going to get the filename,
	// append the suffixes and the extensions to
	// retrieve the 6 textures
	std::string tempName;	

	// Opengl Textures
	std::vector<kTexture*> cubeTex;
	kTexture* tempTex = NULL;
		
	// Front
	tempName = newName + "_front" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), NULL, NULL);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	cubeTex[CUBE_FRONT] = tempTex;

	// Up
	tempName = newName + "_up" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), NULL, NULL);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	cubeTex[CUBE_UP] = tempTex;

	// Down 
	tempName = newName + "_down" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), NULL, NULL);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	cubeTex[CUBE_DOWN] = tempTex;

	// left
	tempName = newName + "_left" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), NULL, NULL);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	cubeTex[CUBE_LEFT] = tempTex;

	// right
	tempName = newName + "_right" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), NULL, NULL);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	cubeTex[CUBE_RIGHT] = tempTex;

	// back 
	unsigned short width, height;
	tempName = newName + "_back" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), NULL, NULL);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	cubeTex[CUBE_BACK] = tempTex;

	// Ok we can setup everything
	texture* newTexture = new texture;
	if (newTexture)
	{
		newTexture->mWidth = width;
		newTexture->mHeight = height;
		newTexture->mImagesCount = 6;
		newTexture->mId = cubeTex;

		return newTexture;
	}
	else
	{
		for (unsigned int i = 0; i < 6; i++)
		{
			texLoader->unLoadTexture(cubeTex[i]);
		}
	}

	return NULL;
}

}

#endif

