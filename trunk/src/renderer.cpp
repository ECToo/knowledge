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

#include "renderer.h"
#include "root.h"
#include "logger.h"

namespace k {

template<> renderer* singleton<renderer>::singleton_instance = 0;

renderer& renderer::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}

renderer::renderer()
{
	m3DObjects.clear();
	m2DObjects.clear();
	mSprites.clear();

	mActiveCamera = NULL;
}

renderer::~renderer()
{
	//TODO
}

void renderer::setFpsCounter(bool status)
{
	mCalculateFps = status;
}

unsigned int renderer::getFps()
{
	return mFpsCount;
}

unsigned int renderer::getLastFps()
{
	return mLastFps;
}

sprite* renderer::createSprite(vec_t radi, material* mat)
{
	assert(mat != NULL);

	sprite* newSpr = new sprite(mat, radi);
	if (newSpr)
	{
		mSprites.push_back(newSpr);
		return newSpr;
	}
	else
	{
		S_LOG_INFO("Failed to allocate sprite.");
	}
}

void renderer::fullRemoveSprite(sprite* spr)
{
	removeSprite(spr);
	delete spr;
}

void renderer::removeSprite(sprite* spr)
{
	assert(spr != NULL);
	std::list<sprite*>::iterator it;
	for (it = mSprites.begin(); it != mSprites.end(); )
	{
		if (spr == (*it))
		{
			mSprites.erase(it);
			return;
		}
		else ++it;
	}

	S_LOG_INFO("Failed to remove sprite.");
}

void renderer::push3D(drawable3D* object)
{
	assert(object != NULL);
	m3DObjects.push_back(object);
}

void renderer::pop3D(drawable3D* object)
{
	assert(object != NULL);

	std::list<drawable3D*>::iterator it;
	for (it = m3DObjects.begin(); it != m3DObjects.end(); )
	{
		drawable3D* obj = (*it);
		if (obj == object)
		{
			it = m3DObjects.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void renderer::push2D(drawable2D* object)
{
	assert(object != NULL);
	m2DObjects.push_back(object);
}

void renderer::pop2D(drawable2D* object)
{
	assert(object != NULL);
	std::list<drawable2D*>::iterator it;
	for (it = m2DObjects.begin(); it != m2DObjects.end(); )
	{
		drawable2D* obj = (*it);
		if (obj == object)
		{
			it = m2DObjects.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void renderer::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	/**
	 * Call the frame start
	 */
	rs->frameStart();
	rs->setDepthTest(true);

	// Time since frame start.
	mFrameTime.reset();

	if (mActiveCamera)
	{
		mActiveCamera->setPerspective();

		if (mActiveCamera->getPosition() != mLastCameraPos)
		{
			// Invalidate all Sprite positions
			std::list<sprite*>::const_iterator it;
			for (it = mSprites.begin(); it != mSprites.end(); it++)
			{
				(*it)->invalidate();
			}
		}
	}
	else
	{
		rs->setMatrixMode(MATRIXMODE_PROJECTION);
		rs->identityMatrix();
		rs->setPerspective(90, 1.33, 0.1, 1000.0f);
	}

	/**
	 * Camera is ready, draw objects
	 * keep in mind that you wont call identityMatrix() on the
	 * rendersystem (for modelview), otherwise it will 
	 * remove camera parameters.
	 */
	for (std::list<drawable3D*>::const_iterator it = m3DObjects.begin(); it != m3DObjects.end(); ++it)
	{
		drawable3D* obj = *it;
		assert(obj != NULL);

		if (!mActiveCamera)
		{
			rs->setMatrixMode(MATRIXMODE_MODELVIEW);
			rs->identityMatrix();
		}
		else
		{
			mActiveCamera->copyView();
		}

		obj->draw();
	}

	std::list<sprite*>::const_iterator it;
	for (it = mSprites.begin(); it != mSprites.end(); it++)
	{
		sprite* spr = (*it);
		assert(spr != NULL);

		if (!mActiveCamera)
		{
			rs->setMatrixMode(MATRIXMODE_MODELVIEW);
			rs->identityMatrix();
		}
		else
		{
			mActiveCamera->copyView();
		}

		spr->draw();
	}

	/**
	 * Set the 2D projection here and draw the 2d objects on it
	 */
	rs->setDepthTest(false);

	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	rs->pushMatrix();
	rs->identityMatrix();
	rs->setOrthographic(0, rs->getScreenWidth(), 0, rs->getScreenHeight(), -1, 1);

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->pushMatrix();
	rs->identityMatrix();

	for (std::list<drawable2D*>::const_iterator dit = m2DObjects.begin(); dit != m2DObjects.end(); ++dit)
	{
		drawable2D* obj = (*dit);
		assert(obj != NULL);

		obj->draw();
	}

	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	rs->popMatrix();

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->popMatrix();

	/**
	 * Call the frame end
	 */
	rs->frameEnd();
			
	if (mCalculateFps)
	{
		mFpsCount++;
		if (mFpsTimer.getMilliSeconds() > 1000)
		{
			mFpsTimer.reset();
			mLastFps = mFpsCount;
			mFpsCount = 0;
		}
	}
}

void renderer::setCamera(camera* cam)
{
	assert(cam != NULL);
	mActiveCamera = cam;

	mLastCameraPos = cam->getPosition();
}

camera* renderer::getCamera()
{
	return mActiveCamera;
}

}

