/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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
	for (it = mStages.begin(); it != mStages.end(); it++)
		delete (*it);

	mStages.clear();
}

void material::start()
{
	// Material Properties
	renderSystem* rs = root::getSingleton().getRenderSystem();

	if (mNoDraw)
		return;

	rs->bindMaterial(this);

	if (mAmbient != k::vector3::zero)
		rs->matAmbient(mAmbient);

	if (mDiffuse != k::vector3::zero)
		rs->matDiffuse(mDiffuse);

	if (mSpecular != k::vector3::zero)
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

