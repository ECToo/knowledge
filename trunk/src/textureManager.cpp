#include "textureManager.h"
#include "logger.h"

#ifdef __WII__
	#include "pngu.h"
#endif

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
	// TODO: Free all textures =]
}

#ifndef __WII__
GLuint* textureManager::getTexture(const std::string& filename)
#else	
wiiTexture* textureManager::getTexture(const std::string& filename)
#endif
{
	#ifndef __WII__
	std::map<std::string, GLuint*>::iterator it = mTextures.find(filename);
	#else
	std::map<std::string, wiiTexture*>::iterator it = mTextures.find(filename);
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
texture* textureManager::createTexturePNG(const std::string& filename)
{
	IMGCTX textureCtx = PNGU_SelectImageFromDevice(filename.c_str());
	if (textureCtx)
	{
		PNGUPROP imgProperties;
		PNGU_GetImageProperties(textureCtx, &imgProperties);

		char* textureData = (char*) memalign(32, imgProperties.imgWidth * imgProperties.imgHeight * 4);
		if (textureData)
		{
			GXTexObj* newGXTex = (GXTexObj*) memalign(32, sizeof(GXTexObj));	
			if (newGXTex)
			{	
				PNGU_DecodeTo4x4RGBA8(textureCtx, imgProperties.imgWidth, imgProperties.imgHeight, textureData, 0xff);
				PNGU_ReleaseImageContext(textureCtx);

				DCFlushRange(textureData, imgProperties.imgWidth * imgProperties.imgHeight * 4);

				GX_InitTexObj(newGXTex, textureData, imgProperties.imgWidth,
						imgProperties.imgHeight, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);

				wiiTexture* newListedTexture = new wiiTexture;
				if (newListedTexture)
				{
					newListedTexture->mWidth = imgProperties.imgWidth;
					newListedTexture->mHeight = imgProperties.imgHeight;
					newListedTexture->mData = newGXTex;

					mTextures[filename] = newListedTexture;
				}

				texture* newWiiTexture = new texture(newGXTex, imgProperties.imgWidth, imgProperties.imgHeight);
				return newWiiTexture;
			}
			else
			{
				delete [] textureData;
				return NULL;
			}
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

texture* textureManager::createTextureJPG(const std::string& filename)
{
	// TODO
	return NULL;
}

texture* textureManager::createTexture(const std::string& filename)
{
	// Try to find the texture first
	if (wiiTexture* nTexture = getTexture(filename))
	{
		texture* newTexture = new texture(nTexture->mData, nTexture->mWidth, nTexture->mHeight);
		return newTexture;
	}

	// Discover texture "format" (by extension)
	if (filename.length() > 4)
	{
		const std::string extension = filename.substr(filename.length()-3, 3);
		if (extension == "PNG" || extension == "png")
			return createTexturePNG(filename);
		else
		if (extension == "JPG" || extension == "jpg")
			return createTextureJPG(filename);
	}

	return NULL;
}
#endif

}

