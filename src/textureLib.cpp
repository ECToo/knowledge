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

inline bool isPowerOfTwo(unsigned int n)
{
	return ((n & (n-1)) == 0);
}

texture* createRawTexture(const std::string& filename)
{
	texture* newTexture = new texture();
	if (newTexture)
	{
		textureLoader* loader = &textureLoader::getSingleton();
		kAssert(loader != NULL);

		unsigned short w, h;
		kTexture* newKTexture = NULL;

		newKTexture = loader->loadTexture(filename.c_str(), &w, &h);

		if (!isPowerOfTwo(w) || !isPowerOfTwo(h))
		{
			std::stringstream warn;
			warn << "WARNING! The texture " << filename << " dimensions(";
			warn << w << "," << h << ") are not power of 2";

			S_LOG_INFO(warn.str());
		}

		if (newKTexture)
		{
			newTexture->push(filename);
			newTexture->push(newKTexture, w, h);

			return newTexture;
		}
		else
		{
			delete newTexture;
		}
	}
		
	return NULL;
}

texture* createRawCubemap(const std::string& filename)
{
	textureLoader* texLoader = &textureLoader::getSingleton();
	if (!texLoader)
		return NULL;

	texture* newTexture = new texture;
	if (!newTexture)
	{
		S_LOG_INFO("Failed to allocate texture for cubemap.");
		return NULL;
	}

	// Get Temp name
	std::string newName = filename;
	std::string extension = getExtension(filename);

	newName.erase(newName.length() - 4, 4);

	// Now we are going to get the filename,
	// append the suffixes and the extensions to
	// retrieve the 6 textures
	std::string tempName;	

	// Raw Textures
	kTexture* tempTex = NULL;
		
	// Front
	tempName = newName + "_front" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), NULL, NULL);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex, 0, 0);

	// Back
	tempName = newName + "_back" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), NULL, NULL);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex, 0, 0);

	// Left 
	tempName = newName + "_left" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), NULL, NULL);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex, 0, 0);

	// Right
	tempName = newName + "_right" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), NULL, NULL);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex, 0, 0);

	// Up
	tempName = newName + "_up" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), NULL, NULL);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}
	newTexture->push(tempTex, 0, 0);

	// Final Size
	unsigned short width = 0;
	unsigned short height = 0;

	// Down 
	tempName = newName + "_down" + extension;	
	tempTex = texLoader->loadTexture((char*)tempName.c_str(), &width, &height);
	if (!tempTex)
	{
		S_LOG_INFO("Failed to read " + tempName + ", does it exist?");
		return NULL;
	}

	newTexture->push(tempTex, width, height);
	newTexture->push(filename);

	if (!isPowerOfTwo(width) || !isPowerOfTwo(height))
	{
		std::stringstream warn;
		warn << "WARNING! The texture " << filename << " dimensions(";
		warn << width << "," << height;
		warn << ") are not power of 2";

		S_LOG_INFO(warn.str());
	}

	return newTexture;
}

}

#endif

