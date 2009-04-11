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

#include "material.h"
#include "root.h"
#include "logger.h"

namespace k {

material::material()
{
	mTextureUnits = 0;
	mCull = CULLMODE_FRONT;
	mDepthTest = true;
	mNoDraw = false;
}

void material::prepare()
{
	// Material Properties
	renderSystem* rs = root::getSingleton().getRenderSystem();
	kAssert(rs != NULL);

	if (mNoDraw)
		return;

	rs->bindMaterial(this);
	rs->matAmbient(mAmbient);
	rs->matDiffuse(mDiffuse);
	rs->matSpecular(mSpecular);
	rs->setCulling(mCull);

	if (!mDepthTest)
		rs->setDepthTest(mDepthTest);

	// Color/Light Only
	if (!mTextureUnits)
	{
		rs->setColorChannels(0);
		rs->setTextureGenerations(0);
		rs->setTextureUnits(1);
	}
	else
	{
		rs->setColorChannels(1);
		rs->setTextureGenerations(mTextureUnits);
		rs->setTextureUnits(mTextureUnits);
	}

	// Cycle through textures
	std::list<textureStage*>::iterator it;
	for (it = mTextures.begin(); it != mTextures.end(); it++)
	{
		textureStage* tex = (*it);
		kAssert(tex != NULL);
		
		tex->draw();
	}
}

void material::finish()
{
	if (mNoDraw)
		return;

	// Cycle through textures
	std::list<textureStage*>::iterator it;
	for (it = mTextures.begin(); it != mTextures.end(); it++)
	{
		textureStage* tex = (*it);
		kAssert(tex != NULL);
		
		tex->finish();
	}
}
			
unsigned int material::getTextureUnits()
{
	return mTextureUnits;
}
			
void material::setDepthTest(bool test)
{
	mDepthTest = test;
}
			
void material::setNoDraw(bool nd)
{
	mNoDraw = nd;
}

void material::setTextureUnits(unsigned int tex)
{
	mTextureUnits = tex;
}
			
void material::setAmbient(const vector3& color)
{
	mAmbient = color;
}

void material::setDiffuse(const vector3& color)
{
	mDiffuse = color;
}

void material::setSpecular(const vector3& color)
{
	mSpecular = color;
}
			
void material::setCullMode(CullMode cull)
{
	mCull = cull;
}
			
unsigned int material::getNumberOfTextureStages() const
{
	return mTextures.size();
}
			
bool material::getNoDraw()
{
	return mNoDraw;
}

textureStage* material::getTextureStage(unsigned short index)
{
	unsigned short i = 0;
	std::list<textureStage*>::iterator it;

	for (it = mTextures.begin(); it != mTextures.end(); it++)
	{
		if (i++ == index)
			return (*it);
	}

	return NULL;
}
			
bool material::containsTexture(const std::string& name)
{
	std::list<textureStage*>::iterator it;
	for (it = mTextures.begin(); it != mTextures.end(); it++)
	{
		if ((*it)->containsTexture(name))
			return true;
	}

	return false;
}

void material::pushTexture(textureStage* tex)
{
	kAssert(tex);
	mTextures.push_back(tex);
}
			
void material::setSingleTexture(texture* tex)
{
	kAssert(tex);

	platTextureStage* newTexStage = new platTextureStage(0);
	if (newTexStage)
	{
		newTexStage->setTexture(tex);
		pushTexture(newTexStage);
	}
	else 
	{
		S_LOG_INFO("Failed to allocate texture stage.");
	}
}

void material::setSingleTexture(unsigned int w, unsigned int h, kTexture* tex)
{
	kAssert(tex);

	texture* newTexture = textureManager::getSingleton().createEmptyTexture();
	platTextureStage* newTexStage = new platTextureStage(0);

	if (newTexture && newTexStage)
	{
		newTexture->push(tex, w, h);
		newTexStage->setTexture(newTexture);

		pushTexture(newTexStage);
	}
	else 
	{
		S_LOG_INFO("Failed to allocate texture stage.");
	}
}

}

