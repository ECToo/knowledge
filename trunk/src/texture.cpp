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

texture::texture()
{
	mWidth = mHeight = 0;

	mId.clear();
	mFilenames.clear();
	mTextureData.clear();
}

texture::~texture()
{
	std::vector<char*>::iterator it;
	for (it = mTextureData.begin(); it != mTextureData.end(); it++)
	{
		char* temp = *it;
		if (temp)
		{
			// free texture data if it was saved.
			// we used free here because of memalign
			free(temp);
		}
	}
	mTextureData.clear();

	std::vector<kTexture*>::iterator kit;
	for (kit = mId.begin(); kit != mId.end(); kit++)
		unloadTexture(*kit);

	mId.clear();
}
			
void texture::push(kTexture* tex, unsigned short w, unsigned short h)
{
	kAssert(tex);

	mWidth = w;
	mHeight = h;
	mId.push_back(tex);
}

void texture::push(const std::string& filename)
{
	mFilenames.push_back(filename);
}

void texture::push(char* data)
{
	kAssert(data);
	mTextureData.push_back(data);
}

unsigned short texture::getSize()
{
	return mId.size();
}
			
unsigned short texture::getWidth()
{
	return mWidth;
}

unsigned short texture::getHeight()
{
	return mHeight;
}

char* texture::getData(unsigned int i)
{
	return mTextureData[i];
}

kTexture* texture::getId(unsigned int i)
{
	if (mId.size() < i)
		return NULL;
	else
		return mId[i];
}

bool texture::containsFilename(const std::string& name)
{
	std::vector<std::string>::iterator it;
	for (it = mFilenames.begin(); it != mFilenames.end(); it++)
	{
		if ((*it).find(name) != std::string::npos)
			return true;
	}

	return false;
}

textureStage::textureStage(unsigned short index)
{
	mIndex = index;
	mAngle = 0;
	mBlendSrc = 0;
	mBlendDst = 0;
	mRotate = 0;
	mScroll.x = 0;
	mScroll.y = 0;

	mTexCoordType = TEXCOORD_UV;
	mTexture = NULL;
	mReplaceByLightmap = false;
}
			
void textureStage::setBlendMode(unsigned short src, unsigned short dst)
{
	mBlendSrc = src;
	mBlendDst = dst;
}
			
void textureStage::setLightmapReplace(bool lm)
{
	mReplaceByLightmap = lm;
}

void textureStage::setTexture(texture* tex)
{
	kAssert(tex);
	mTexture = tex;
}

unsigned short textureStage::getImagesCount()
{
	if (mTexture)
		return mTexture->getSize();
	else
		return 0;
}

unsigned short textureStage::getWidth()
{
	if (mTexture)
		return mTexture->getWidth();
	else
		return 0;
}

unsigned short textureStage::getHeight()
{
	if (mTexture)
		return mTexture->getHeight();
	else
		return 0;
}

bool textureStage::getReplaceByLightmap()
{
	return mReplaceByLightmap;
}
			
kTexture* textureStage::getTexture(int i)
{
	if (mTexture)
		return mTexture->getId(i);
	else
		return NULL;
}

void textureStage::setProgram(const std::string& name)
{
	mProgram = name;	
}
	
void textureStage::setTexCoordType(texCoordType type)
{
	mTexCoordType = type;
}

void textureStage::setScroll(vector2 scroll)
{
	mScroll = scroll;
}
			
void textureStage::setScale(vector2 scale)
{
	mScale = scale;
}

void textureStage::setRotate(vec_t angle)
{
	mRotate = angle;
}
			
texCoordType textureStage::getTexCoordType()
{
	return mTexCoordType;
}
			
bool textureStage::containsTexture(const std::string& name)
{
	if (mTexture)
		return mTexture->containsFilename(name);
	else
		return false;
}
			
}

