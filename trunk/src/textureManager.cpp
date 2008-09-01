#include "textureManager.h"

namespace k {

template<> textureManager* singleton<textureManager>::singleton_instance = 0;

textureManager& textureManager::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}

textureManager::textureManager()
{
	#ifdef __WII__
	// Invalidate all texture units
	GX_InvalidateTexAll();
	#else
	ilInit();
	ilEnable(IL_CONV_PAL);
	iluInit();
	ilutInit();
	#endif

	mTextures.clear();
}

textureManager::~textureManager()
{
}

#ifndef __WII__
GLuint* textureManager::getTexture(const std::string& filename)
#else	
GXTexObj* textureManager::getTexture(const std::string& filename)
#endif
{
	#ifndef __WII__
	std::map<std::string, GLuint*>::iterator it = mTextures.find(filename);
	#else
	std::map<std::string, GXTexObj*>::iterator it = mTextures.find(filename);
	#endif

	if (it != mTextures.end())
	{
		return it->second;
	}
	
	return NULL;
}

#ifndef __WII__
texture* textureManager::createTexture(const std::string& filename)
{
	unsigned int width, height;

	if (ILuint* existingImage = getTexture(filename))
	{
		ilBindImage(*existingImage);

		width = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);

		return (new texture(existingImage, width, height));
	}
	else
	{
		ILuint* newImage = new ILuint;

		ilGenImages(1, newImage);
		ilBindImage(newImage[0]);
		ilLoadImage((char*)filename.c_str());

		if (ilGetError() == IL_NO_ERROR)
		{
			width = ilGetInteger(IL_IMAGE_WIDTH);
			height = ilGetInteger(IL_IMAGE_HEIGHT);

			texture* newTexture = new texture(newImage, width, height);
			if (newTexture)
			{
				mTextures[filename] = newImage;
			}

			return newTexture;
		}
		else
		{
			return NULL;
		}
	}
}
#else
#endif

}

