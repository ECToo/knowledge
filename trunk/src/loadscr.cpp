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
	if (!texMgr->allocateTexture(filename))
	{
		S_LOG_INFO("Failed to allocate load screen texture.");
	}

	k::texture* newTexture = texMgr->getTexture(filename);
	if (!newTexture)
	{
		S_LOG_INFO("Failed to load texture for loading screen.");
		return;
	}

	try
	{
		mBackground = new k::material(newTexture);
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

	mBackground->start();
	rs->setDepthMask(false);

	rs->clearArrayDesc(VERTEXMODE_QUAD);
	rs->setVertexArray(vertices);
	rs->setTexCoordArray(uvs);
	rs->setVertexIndex(indices);

	rs->setVertexCount(4);
	rs->setIndexCount(4);
	rs->drawArrays();

	mBackground->finish();

	rs->setDepthMask(true);
	rs->frameEnd();
}

void imgLoadScreen::loadBg(const std::string& filename)
{
	k::textureManager* texMgr = &k::textureManager::getSingleton();
	if (!texMgr->allocateTexture(filename))
	{
		S_LOG_INFO("Failed to allocate load screen texture.");
	}

	k::texture* newTexture = texMgr->getTexture(filename);
	if (!newTexture)
	{
		S_LOG_INFO("Failed to load texture for loading screen.");
		return;
	}

	try
	{
		mImgMaterial = new k::material(newTexture);
		mImgMaterial->setCullMode(CULLMODE_NONE);
	}

	catch (...)
	{
		S_LOG_INFO("Failed to create material for loading screen.");
	}

	mBgColor = color(0, 0, 0, 1);
}

void imgLoadScreen::setBgColor(const color& clr)
{
	mBgColor = clr;
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

	mImgMaterial->start();
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

