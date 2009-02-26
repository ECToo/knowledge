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

// Static for full screen =]
vec_t mVertices[] = { 0.0f, 1.0f };
vec_t mTexCoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
index_t mIndices[] = { 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0 };

void bgLoadScreen::loadBg(const std::string& filename)
{
	k::textureManager* texMgr = &k::textureManager::getSingleton();
	kAssert(texMgr);

	texMgr->allocateTextureData(filename);

	k::texture* newTexture = texMgr->getTexture(filename);
	if (!newTexture)
	{
		S_LOG_INFO("Failed to load texture for loading screen.");
		return;
	}

	mBackground = new k::material();
	if (!mBackground)
	{
		S_LOG_INFO("Failed to create material for loading screen.");
		return;
	}

	mBackground->setSingleTexture(newTexture->getWidth(), newTexture->getHeight(), newTexture->getId(0));
	mBackground->setCullMode(CULLMODE_NONE);
}

void bgLoadScreen::update(const std::string& filename)
{
	k::renderSystem* rs = k::root::getSingleton().getRenderSystem();
	kAssert(rs);
	kAssert(mBackground);

	rs->frameStart();

	rs->setMatrixMode(k::MATRIXMODE_PROJECTION);
	rs->setOrthographic(0, 1.0, 0, 1.0, -1, 1);

	rs->setMatrixMode(k::MATRIXMODE_MODELVIEW);
	rs->identityMatrix();

	mBackground->prepare();
	rs->setDepthMask(false);

	rs->startVertices(VERTEXMODE_QUAD);

	rs->texCoord(vector2(0.0, 1.0));
	rs->vertex(vector3(0, 0, -0.5));

	rs->texCoord(vector2(1.0, 1.0));
	rs->vertex(vector3(1, 0, -0.5));

	rs->texCoord(vector2(1.0, 0.0));
	rs->vertex(vector3(1, 1, -0.5));

	rs->texCoord(vector2(0.0, 0.0));
	rs->vertex(vector3(0, 1, -0.5));

	rs->endVertices();

	mBackground->finish();
	rs->setDepthMask(true);
	// 

	rs->frameEnd();
}

void imgLoadScreen::loadBg(const std::string& filename)
{
	k::textureManager* texMgr = &k::textureManager::getSingleton();
	kAssert(texMgr);

	texMgr->allocateTextureData(filename);

	k::texture* newTexture = texMgr->getTexture(filename);
	if (!newTexture)
	{
		S_LOG_INFO("Failed to load texture for loading screen.");
		return;
	}

	mImgMaterial = new k::material();
	if (!mImgMaterial)
	{
		S_LOG_INFO("Failed to create material for loading screen.");
		return;
	}

	mImgMaterial->setSingleTexture(newTexture->getWidth(), newTexture->getHeight(), newTexture->getId(0));
	mImgMaterial->setCullMode(CULLMODE_NONE);
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
	kAssert(rs);
	kAssert(mImgMaterial);

	uint32_t rsWidth = rs->getScreenWidth();
	uint32_t rsHeight = rs->getScreenHeight();
	vec_t mRealDimX = mDimension.x / rsWidth;
	vec_t mRealDimY = mDimension.y / rsHeight;

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

