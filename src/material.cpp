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

	#ifdef __WII__
	mCull = CULLMODE_BACK;
	#else
	mCull = CULLMODE_FRONT;
	#endif

	mDepthTest = true;
}

void material::prepare()
{
	// Material Properties
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	rs->bindMaterial(this);
	rs->matAmbient(mAmbient);
	rs->matDiffuse(mDiffuse);
	rs->matSpecular(mSpecular);
	rs->setCulling(mCull);
	rs->setDepthTest(mDepthTest);

	// Color/Light Only
	#ifdef __WII__
	if (!mTextureUnits)
	{
		GX_SetNumTevStages(1);
		GX_SetNumChans(0);
		GX_SetNumTexGens(0);
		GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	}
	else
	{
		GX_SetNumChans(1);
		GX_SetNumTevStages(mTextureUnits);
		GX_SetNumTexGens(mTextureUnits);
	}
	#endif

	// Cycle through textures
	std::list<textureStage*>::iterator it;
	for (it = mTextures.begin(); it != mTextures.end(); it++)
	{
		textureStage* tex = (*it);
		assert(tex != NULL);
		
		tex->draw();
	}
}

void material::finish()
{
	// Cycle through textures
	std::list<textureStage*>::iterator it;
	for (it = mTextures.begin(); it != mTextures.end(); it++)
	{
		textureStage* tex = (*it);
		assert(tex != NULL);
		
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
			
unsigned int material::getNumberOfTextureStages()
{
	return mTextures.size();
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

void material::pushTexture(textureStage* tex)
{
	assert(tex != NULL);
	mTextures.push_back(tex);
}

}

