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
	mParticles.clear();

	mActiveCamera = NULL;
	mSkybox = NULL;
	mSkyPlane = NULL;
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
		
	S_LOG_INFO("Failed to allocate sprite.");
	return NULL;
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
			
void renderer::pushParticle(particleSystem* p)
{
	assert(p != NULL);
	mParticles.push_back(p);
}

void renderer::popParticle(particleSystem* p)
{
	assert(p != NULL);

	std::list<particleSystem*>::iterator it;
	for (it = mParticles.begin(); it != mParticles.end(); )
	{
		particleSystem* obj = (*it);
		if (obj == p)
		{
			it = mParticles.erase(it);
		}
		else
		{
			++it;
		}
	}
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
			
void renderer::setSkyBox(const std::string& matName)
{
	materialManager* matMgr = &materialManager::getSingleton();
	assert(matMgr != NULL);

	material* mat = matMgr->getMaterial(matName);
	assert(mat != NULL);

	mSkybox = mat;
	mSkyPlane = NULL;
}

void renderer::setSkyBox(material* mat)
{
	assert(mat != NULL);
	mSkybox = mat;
	mSkyPlane = NULL;
}

void renderer::setSkyPlane(const std::string& matName)
{
	materialManager* matMgr = &materialManager::getSingleton();
	assert(matMgr != NULL);

	material* mat = matMgr->getMaterial(matName);
	assert(mat != NULL);

	mSkyPlane = mat;
	mSkybox = NULL;
}

void renderer::setSkyPlane(material* mat)
{
	assert(mat != NULL);
	mSkyPlane = mat;
	mSkybox = NULL;
}

static inline bool compare2D(drawable2D* first, drawable2D* second)
{
	assert(first != NULL);
	assert(second != NULL);

	if (first->getZ() > second->getZ())
		return true;
	else
		return false;
}

void renderer::sort2D()
{
	m2DObjects.sort(compare2D);
}

void renderer::push2D(drawable2D* object)
{
	assert(object != NULL);
	m2DObjects.push_back(object);

	// sort2D();
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

void renderer::_drawSkyPlane()
{
	if (!mSkyPlane)
		return;

	// Rendersystem
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	// Disable depth test drawing on orthogonal way
	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	rs->setOrthographic(0, 0.5, 0, 0.5, -1, 1);

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->identityMatrix();

	// Prepare Material
	mSkyPlane->prepare();

	// Obrigatory, independent of material settings.
	rs->setDepthTest(false);
	rs->setDepthMask(false);
	rs->setCulling(CULLMODE_NONE);

 	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3( 0.5f, -0.5f, -0.5f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3(-0.5f, -0.5f, -0.5f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3(-0.5f,  0.5f, -0.5f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 0.5f,  0.5f, -0.5f));

		rs->texCoord(vector2(0, 1)); rs->vertex(vector3( 0.5f, -0.5f,  0.5f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3( 0.5f, -0.5f, -0.5f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3( 0.5f,  0.5f, -0.5f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 0.5f,  0.5f,  0.5f));

		rs->texCoord(vector2(0, 1)); rs->vertex(vector3(-0.5f, -0.5f, -0.5f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3(-0.5f, -0.5f,  0.5f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3(-0.5f,  0.5f,  0.5f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3(-0.5f,  0.5f, -0.5f));
	rs->endVertices();

	mSkyPlane->finish();
	rs->setDepthMask(true);
	rs->setDepthTest(true);

	// Take it back to default
	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	rs->identityMatrix();

	if (mActiveCamera)
	{
		mActiveCamera->setPerspective();
	}
	else
	{
		rs->setPerspective(90, 1.33, 0.1, 1000.0f);
	}

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
}

void renderer::_drawSkybox()
{
	if (!mSkybox)
		return;

	// Rendersystem
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	// Disable depth test drawing on orthogonal way
	
	/*
	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	rs->setOrthographic(0, 0.5, 0, 0.5, -2, 2);
	*/

	// Draw
	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	if (mActiveCamera)
	{
		matrix4 cameraRot = mActiveCamera->getOrientation().toMatrix();

		#ifdef __WII__
		matrix4 newRot = cameraRot.transpose();
		rs->copyMatrix(newRot.m);
		#else
		rs->copyMatrix(cameraRot.m[0]);
		#endif
	}
	else
	{
		rs->identityMatrix();
	}

	// Material
	mSkybox->prepare();


	// Material Independent Settings
	rs->setCulling(CULLMODE_NONE);
	rs->setDepthTest(false);
	rs->setDepthMask(false);

	textureStage* texStage = mSkybox->getTextureStage(0);
	assert(texStage != NULL);
	assert(texStage->getImagesCount() == 6);

	std::vector<kTexture*>* mId = texStage->getId();

	// Render the front quad
	rs->bindTexture((*mId)[CUBE_FRONT], 0);
 	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3( 1.0f, -1.0f, -1.0f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3(-1.0f, -1.0f, -1.0f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3(-1.0f,  1.0f, -1.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 1.0f,  1.0f, -1.0f));
	rs->endVertices();

	// Render the left quad
	rs->bindTexture((*mId)[CUBE_LEFT], 0);
	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3( 1.0f, -1.0f,  1.0f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3( 1.0f, -1.0f, -1.0f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3( 1.0f,  1.0f, -1.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 1.0f,  1.0f,  1.0f));
	rs->endVertices();

	// Render the back quad
	rs->bindTexture((*mId)[CUBE_BACK], 0);
	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3(-1.0f, -1.0f,  1.0f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3( 1.0f, -1.0f,  1.0f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3( 1.0f,  1.0f,  1.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3(-1.0f,  1.0f,  1.0f));
	rs->endVertices();

	// Render the right quad
	rs->bindTexture((*mId)[CUBE_RIGHT], 0);
	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3(-1.0f, -1.0f, -1.0f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3(-1.0f, -1.0f,  1.0f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3(-1.0f,  1.0f,  1.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3(-1.0f,  1.0f, -1.0f));
	rs->endVertices();

	// Render the top quad
	rs->bindTexture((*mId)[CUBE_UP], 0);
	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3(-1.0f,  1.0f, -1.0f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3(-1.0f,  1.0f,  1.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 1.0f,  1.0f,  1.0f));
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3( 1.0f,  1.0f, -1.0f));
	rs->endVertices();

	// Render the bottom quad
	rs->bindTexture((*mId)[CUBE_DOWN], 0);
	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3(-1.0f, -1.0f, -1.0f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3(-1.0f, -1.0f,  1.0f));
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3( 1.0f, -1.0f,  1.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 1.0f, -1.0f, -1.0f));
	rs->endVertices();
	
	// Take it back to default
	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	if (mActiveCamera)
		mActiveCamera->setPerspective();
	else
		rs->setPerspective(90, 1.33, 0.1, 1000.0f);

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->identityMatrix();

	// Set back depth mask
	rs->setDepthMask(true);
	rs->setDepthTest(true);
}

void renderer::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	/**
	 * Call the frame start
	 */
	rs->frameStart();

	// Time since frame start.
	mFrameTime.reset();

	// Draw Skybox, Plane
	_drawSkybox();
	_drawSkyPlane();

	rs->setDepthTest(true);
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

	// Particles
	std::list<particleSystem*>::const_iterator pIt;
	for (pIt = mParticles.begin(); pIt != mParticles.end(); pIt++)
	{
		(*pIt)->cycle(mActiveCamera);
	}

	/**
	 * Set the 2D projection here and draw the 2d objects on it
	 */
	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	rs->setOrthographic(0, rs->getScreenWidth(), rs->getScreenHeight(), 0, -128, 128);

	for (std::list<drawable2D*>::const_iterator dit = m2DObjects.begin(); dit != m2DObjects.end(); dit++)
	{
		drawable2D* obj = (*dit);
		assert(obj != NULL);

		rs->setMatrixMode(MATRIXMODE_MODELVIEW);
		rs->identityMatrix();

		obj->draw();
	}

	/**
	 * Call the frame end
	 */
	rs->frameEnd();
			
	// Frames per Second 
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

