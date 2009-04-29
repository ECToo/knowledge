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

#include "loadscr.h"
#include "logger.h"
#include "rendersystem.h"
#include "textureManager.h"
#include "root.h"

namespace k {

const vec_t vertices[] ATTRIBUTE_ALIGN(32) = 
{ 
	0.0, 0.0, -0.5, 
	1.0, 0.0, -0.5, 
	1.0, 1.0, -0.5, 
	0.0, 1.0, -0.5
};

const vec_t uvs[] ATTRIBUTE_ALIGN(32) = 
{ 
	0.0, 1.0,
	1.0, 1.0, 
	1.0, 0.0, 
	0.0, 0.0
};

const index_t indices[] ATTRIBUTE_ALIGN(32) = { 0, 1, 2, 3 };

void bgLoadScreen::loadBg(const std::string& filename)
{
	k::textureManager* texMgr = &k::textureManager::getSingleton();
	texMgr->allocateTextureData(filename);

	k::texture* newTexture = texMgr->getTexture(filename);
	if (!newTexture)
	{
		S_LOG_INFO("Failed to load texture for loading screen.");
		return;
	}

	try
	{
		mBackground = new k::material;
		mBackground->setSingleTexture(newTexture);
		mBackground->setCullMode(CULLMODE_NONE);
	}
	catch (...)
	{
		S_LOG_INFO("Failed to create material for loading screen.");
	}

}

void bgLoadScreen::update(const std::string& filename)
{
	if (!mBackground)
		return;

	k::renderSystem* rs = k::root::getSingleton().getRenderSystem();
	rs->frameStart();

	rs->setMatrixMode(k::MATRIXMODE_PROJECTION);
	rs->setOrthographic(0, 1.0, 0, 1.0, -1, 1);

	rs->setMatrixMode(k::MATRIXMODE_MODELVIEW);
	rs->identityMatrix();

	mBackground->prepare();
	rs->setDepthMask(false);

	rs->clearArrayDesc(VERTEXMODE_QUAD);
	rs->setVertexArray(vertices);
	rs->setTexCoordArray(uvs);
	rs->setVertexIndex(indices);

	rs->setVertexCount(4);
	rs->setIndexCount(4);
	rs->drawArrays();

	rs->endVertices();

	mBackground->finish();

	rs->setDepthMask(true);
	rs->frameEnd();
}

void imgLoadScreen::loadBg(const std::string& filename)
{
	k::textureManager* texMgr = &k::textureManager::getSingleton();
	texMgr->allocateTextureData(filename);

	k::texture* newTexture = texMgr->getTexture(filename);
	if (!newTexture)
	{
		S_LOG_INFO("Failed to load texture for loading screen.");
		return;
	}

	try
	{
		mImgMaterial = new k::material();
		mImgMaterial->setSingleTexture(newTexture->getWidth(), newTexture->getHeight(), newTexture->getId(0));
		mImgMaterial->setCullMode(CULLMODE_NONE);
	}

	catch (...)
	{
		S_LOG_INFO("Failed to create material for loading screen.");
	}

}

void imgLoadScreen::setBgColor(const vector3& color)
{
	mBgColor = color;
}

void imgLoadScreen::setImgDimension(const vector2& dimension)
{
	mDimension = dimension;
}

void imgLoadScreen::update(const std::string& filename)
{
	k::renderSystem* rs = k::root::getSingleton().getRenderSystem();

	if (!mImgMaterial)
		return;

	uint32_t rsWidth = rs->getScreenWidth();
	uint32_t rsHeight = rs->getScreenHeight();

	vec_t mRealDimX;
	vec_t mRealDimY;

	if (!rsWidth || !rsHeight)
	{
		mRealDimX = 1.0f;
		mRealDimY = 1.0f;
	}
	else
	{
		mRealDimX = mDimension.x / rsWidth;
		mRealDimY = mDimension.y / rsHeight;
	}

	rs->setClearColor(mBgColor);
	rs->frameStart();

	rs->setMatrixMode(k::MATRIXMODE_PROJECTION);
	rs->setOrthographic(0, 1.0, 0, 1.0, -1, 1);

	rs->setMatrixMode(k::MATRIXMODE_MODELVIEW);
	rs->identityMatrix();
	rs->translateScene(0.5 - mRealDimX / 2.0f, 0.5 - mRealDimY / 2.0f, 0);

	mImgMaterial->prepare();
	rs->setDepthMask(false);

	rs->startVertices(VERTEXMODE_QUAD);

	rs->texCoord(vector2(0.0, 1.0));
	rs->vertex(vector3(0, 0, -0.5));

	rs->texCoord(vector2(1.0, 1.0));
	rs->vertex(vector3(mRealDimX, 0, -0.5));

	rs->texCoord(vector2(1.0, 0.0));
	rs->vertex(vector3(mRealDimX, mRealDimY, -0.5));

	rs->texCoord(vector2(0.0, 0.0));
	rs->vertex(vector3(0, mRealDimY, -0.5));

	rs->endVertices();

	mImgMaterial->finish();
	rs->setDepthMask(true);
	// 

	rs->frameEnd();
}

}

