/*
Copyright (c) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "material.h"
#include "root.h"
#include "logger.h"

namespace k {

material::material()
{
	mCull = CULLMODE_FRONT;
	mDepthTest = true;
	mDepthWrite = true;
	mNoDraw = false;
	mIsOpaque = true;
	mReceiveLight = 1;
}

material::material(texture* tex)
{
	kAssert(tex);

	try 
	{
		materialStage* newStage = new materialStage(0);
		newStage->setTexture(tex, 0);
		pushStage(newStage);
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate material stage.");
	}

	mCull = CULLMODE_FRONT;
	mDepthTest = true;
	mDepthWrite = true;
	mNoDraw = false;
	mIsOpaque = true;
	mReceiveLight = 1;
}
			
material::material(const std::string& filename)
{
	texture* newTexture = textureManager::getSingleton().getTexture(filename);
	if (!newTexture)
	{
		S_LOG_INFO("Failed to make a new material, texture not found.");
		return;
	}

	try 
	{
		materialStage* newStage = new materialStage(0);
		newStage->setTexture(newTexture, 0);
		pushStage(newStage);
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate material stage.");
	}

	mCull = CULLMODE_FRONT;
	mDepthTest = true;
	mDepthWrite = true;
	mNoDraw = false;
	mIsOpaque = true;
	mReceiveLight = 1;
}
			
material::~material()
{
	std::vector<materialStage*>::iterator it;
	while (!mStages.empty())
	{
		it = mStages.begin();
		delete (*it);
		mStages.erase(it);
	}
}

void material::start()
{
	// Material Properties
	renderSystem* rs = root::getSingleton().getRenderSystem();

	if (mNoDraw)
		return;

	rs->bindMaterial(this);
	rs->matAmbient(mAmbient);
	rs->matDiffuse(mDiffuse);
	rs->matSpecular(mSpecular);

	rs->setCulling(mCull);
	rs->setDepthTest(mDepthTest);
	rs->setDepthMask(mDepthWrite);

	// Color/Light Only
	if (!mStages.size())
	{
		rs->setColorChannels(0);
		rs->setTextureGenerations(0);
		rs->setTextureUnits(1);
	}
	else
	{
		rs->setColorChannels(1);
		rs->setTextureGenerations(mStages.size());
		rs->setTextureUnits(mStages.size());
	}

	if (!mReceiveLight && rs->isLightOn())
	{
		// Magic trick
		mReceiveLight = -rs->getEnabledLightCount();
		rs->setLighting(false);
	}

	// Cycle through textures
	std::vector<materialStage*>::const_iterator it;
	for (it = mStages.begin(); it != mStages.end(); it++)
	{
		(*it)->feedAnims();
		(*it)->draw();
	}
}

void material::finish()
{
	if (mNoDraw)
		return;

	renderSystem* rs = root::getSingleton().getRenderSystem();
	if (mReceiveLight < 0)
	{
		rs->setLighting(true);
		for (int i = 0; i < -mReceiveLight; i++)
			rs->setLight(i, true);

		// Disabled again
		mReceiveLight = 0;
	}

	// Cycle through textures
	std::vector<materialStage*>::const_iterator it;
	for (it = mStages.begin(); it != mStages.end(); it++)
		(*it)->finish();
}
			
materialStage::materialStage(unsigned short index)
{
	mIndex = index;
	mAngle = 0;
	mBlendSrc = 0;
	mBlendDst = 0;
	mRotate = 0;
	mScroll.x = 0;
	mScroll.y = 0;
	
	mLastFeedTime = root::getSingleton().getGlobalTime();
	mNumberOfFrames = 0;
	mCurrentFrame = 0;
	mFrameRate = 0;

	mTexEnv = TEXENV_REPLACE;
	mCoordType = TEXCOORD_UV;
	
	for (int i = 0; i < K_MAX_STAGE_TEXTURES; i++)
		mTextures[i] = NULL;
}

materialStage::~materialStage()
{}

void materialStage::setEnv(unsigned int tev)
{
	mTexEnv = tev;
}

void materialStage::setBlendMode(unsigned short src, unsigned short dst)
{
	mBlendSrc = src;
	mBlendDst = dst;
}

void materialStage::feedAnims()
{
	if (mNumberOfFrames == 0)
		return;

	long timeNow = root::getSingleton().getGlobalTime();

	mCurrentFrame += (mFrameRate * (timeNow - mLastFeedTime)) / 1000.0f;
	mLastFeedTime = timeNow;

	while ((uint32_t)mCurrentFrame >= mNumberOfFrames)
		mCurrentFrame -= mNumberOfFrames;
}
			
void materialStage::setTexture(texture* tex, unsigned int index)
{
	kAssert(tex);
	mTextures[index] = tex;
}

const texture* materialStage::getTexture(unsigned int i) const
{
	return mTextures[i];
}

unsigned int materialStage::getWidth() const
{
	for (int i = 0; i < K_MAX_STAGE_TEXTURES; i++)
	{
		if (mTextures[i])
			return mTextures[i]->getWidth();
	}
		
	return 0;
}

unsigned int materialStage::getHeight() const
{
	for (int i = 0; i < K_MAX_STAGE_TEXTURES; i++)
	{
		if (mTextures[i])
			return mTextures[i]->getHeight();
	}
		
	return 0;
}

bool materialStage::isOpaque() const
{
	if (!mBlendDst && !mBlendSrc)
		return true;
	else
		return false;
}
			
void materialStage::setCoordType(TextureCoordType type)
{
	mCoordType = type;
}

void materialStage::setScroll(vector2 scroll)
{
	mScroll = scroll;
}
			
unsigned int materialStage::getImagesCount() const
{
	int i = 0;
	for (i = 0; i < K_MAX_STAGE_TEXTURES; i++)
		if (!mTextures[i])
			break;

	return i;
}
			
void materialStage::setScale(vector2 scale)
{
	mScale = scale;
}

void materialStage::setRotate(vec_t angle)
{
	mRotate = angle;
}
			
bool materialStage::containsTexture(const std::string& name) const
{
	for (int i = 0; i < K_MAX_STAGE_TEXTURES; i++)
	{
		if (!mTextures[i])
			break;

		if (mTextures[i]->containsFilename(name))
			return true;
	}
				
	return false;
}
			
}

