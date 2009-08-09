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

#include "renderer.h"
#include "root.h"
#include "logger.h"

namespace k {

template<> renderer* singleton<renderer>::singleton_instance = 0;

renderer& renderer::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

renderer::renderer()
{
	m3DObjects.clear();
	m2DObjects.clear();
	mSprites.clear();
	mLights.clear();

	mActiveCamera = NULL;
	mSkybox = NULL;
	mSkyPlane = NULL;
	mActiveWorld = NULL;

	// mRenderToTexture = false;
	mCalculateFps = true;
	mRenderToTexture = false;
	mFpsCount = 0;
	mLastFps = 1;
}

renderer::~renderer()
{
	m3DObjects.clear();
	m2DObjects.clear();
	mSprites.clear();
}
			
void renderer::setWorld(world* w)
{
	kAssert(w);
	mActiveWorld = w;
}
			
light::light* renderer::createPointLight()
{
	try
	{
		light::light* newLight = new light::light();
		mLights.push_back(newLight);

		return newLight;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate point light.");
	}
		
	return NULL;
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

sprite* renderer::createSprite(const std::string& mat, vec_t radius)
{
	try
	{
		sprite* newSpr = new sprite(mat, radius);
		mSprites.push_back(newSpr);
		return newSpr;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate sprite.");
		return NULL;
	}
}

sprite* renderer::createSprite(vec_t radi, material* mat)
{
	kAssert(mat);

	try
	{
		sprite* newSpr = new sprite(mat, radi);
		mSprites.push_back(newSpr);
		return newSpr;
	}

	catch (...)
	{
		S_LOG_INFO("Failed to allocate sprite.");
		return NULL;
	}
}

void renderer::fullRemoveSprite(sprite* spr)
{
	removeSprite(spr);
	delete spr;
}

void renderer::removeSprite(sprite* spr)
{
	kAssert(spr);

	std::list<sprite*>::iterator it;
	for (it = mSprites.begin(); it != mSprites.end(); it++)
	{
		if (spr == (*it))
		{
			mSprites.erase(it);
			return;
		}
	}

	S_LOG_INFO("Failed to remove sprite.");
}
			
void renderer::push3D(drawable3D* object)
{
	kAssert(object);

	// Put transparent/translucent objects to be drawn later
	if (!object->isOpaque())
		m3DObjects.push_back(object);
	else
		m3DObjects.push_front(object);
}

void renderer::pop3D(drawable3D* object)
{
	kAssert(object);

	std::list<drawable3D*>::iterator it;
	for (it = m3DObjects.begin(); it != m3DObjects.end(); it++)
	{
		if (object == (*it))
		{
			m3DObjects.erase(it);
			return;
		}
	}
}
			
void renderer::setSkyBox(const std::string& matName)
{
	materialManager* matMgr = &materialManager::getSingleton();
	material* mat = matMgr->getMaterial(matName);

	if (!mat)
	{
		S_LOG_INFO("Invalid pointer to skybox material");
		return;
	}

	mSkybox = mat;
	mSkyPlane = NULL;
}

void renderer::setSkyBox(material* mat)
{
	kAssert(mat);

	mSkybox = mat;
	mSkyPlane = NULL;
}

void renderer::setSkyPlane(const std::string& matName)
{
	materialManager* matMgr = &materialManager::getSingleton();
	material* mat = matMgr->getMaterial(matName);
	
	if (!mat)
	{
		S_LOG_INFO("Invalid pointer to skybox material");
		return;
	}

	mSkyPlane = mat;
	mSkybox = NULL;
}

void renderer::setSkyPlane(material* mat)
{
	kAssert(mat);
	mSkyPlane = mat;
	mSkybox = NULL;
}

static inline bool compare2D(drawable2D* first, drawable2D* second)
{
	kAssert(first);
	kAssert(second);

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
	kAssert(object);
	m2DObjects.push_back(object);

	sort2D();
}

void renderer::pop2D(drawable2D* object)
{
	kAssert(object);

	std::list<drawable2D*>::iterator it;
	for (it = m2DObjects.begin(); it != m2DObjects.end(); )
	{
		if (object == (*it))
		{
			m2DObjects.erase(it);
			return;
		}
	}
}

void renderer::_drawSkyPlane()
{
	if (!mSkyPlane)
		return;

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

	// Rendersystem
	renderSystem* rs = root::getSingleton().getRenderSystem();

	// Disable depth test drawing on orthogonal way
	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	rs->setOrthographic(0, 1.0, 0, 1.0, -1, 1);

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->identityMatrix();

	// Prepare Material
	mSkyPlane->start();

	// Obrigatory, independent of material settings.
	// rs->setDepthTest(false);
	rs->setDepthMask(false);
	rs->setCulling(CULLMODE_NONE);

	rs->clearArrayDesc(VERTEXMODE_QUAD);
	rs->setVertexArray(vertices);
	rs->setTexCoordArray(uvs);
	rs->setVertexIndex(indices);

	rs->setVertexCount(4);
	rs->setIndexCount(4);
	rs->drawArrays();

	/*
 	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(0, 2)); rs->vertex(vector3( 1.0f, -1.0f, -1.0f));
		rs->texCoord(vector2(2, 2)); rs->vertex(vector3(-1.0f, -1.0f, -1.0f));
		rs->texCoord(vector2(2, 0)); rs->vertex(vector3(-1.0f,  1.0f, -1.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 1.0f,  1.0f, -1.0f));

		rs->texCoord(vector2(0, 2)); rs->vertex(vector3( 1.0f, -1.0f,  1.0f));
		rs->texCoord(vector2(2, 2)); rs->vertex(vector3( 1.0f, -1.0f, -1.0f));
		rs->texCoord(vector2(2, 0)); rs->vertex(vector3( 1.0f,  1.0f, -1.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 1.0f,  1.0f,  1.0f));

		rs->texCoord(vector2(0, 2)); rs->vertex(vector3(-1.0f, -1.0f, -1.0f));
		rs->texCoord(vector2(2, 2)); rs->vertex(vector3(-1.0f, -1.0f,  1.0f));
		rs->texCoord(vector2(2, 0)); rs->vertex(vector3(-1.0f,  1.0f,  1.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3(-1.0f,  1.0f, -1.0f));
	rs->endVertices();
	*/

	mSkyPlane->finish();
	rs->setDepthMask(true);
	// rs->setDepthTest(true);

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

	// Draw
	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	if (mActiveCamera)
	{
		matrix4 cameraRot = mActiveCamera->getRotInverseTranspose();
		rs->copyMatrix(cameraRot);
	}
	else
	{
		rs->identityMatrix();
	}

	// Material
	mSkybox->start();

	// Material Independent Settings
	rs->setCulling(CULLMODE_NONE);
	rs->setDepthMask(false);

	const materialStage* matStage = mSkybox->getStage(0);
	kAssert(matStage);
	kAssert(matStage->getImagesCount() == 6);

	// Render the front quad
	rs->bindTexture(matStage->getTexture(CUBE_FRONT)->getPointer(), 0);
 	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3( 400.0f, -200.0f, -400.0f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3(-400.0f, -200.0f, -400.0f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3(-400.0f,  200.0f, -400.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 400.0f,  200.0f, -400.0f));
	rs->endVertices();

	// Render the left quad
	rs->bindTexture(matStage->getTexture(CUBE_LEFT)->getPointer(), 0);
	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3( 400.0f, -200.0f,  400.0f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3( 400.0f, -200.0f, -400.0f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3( 400.0f,  200.0f, -400.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 400.0f,  200.0f,  400.0f));
	rs->endVertices();

	// Render the back quad
	rs->bindTexture(matStage->getTexture(CUBE_BACK)->getPointer(), 0);
	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3(-400.0f, -200.0f,  400.0f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3( 400.0f, -200.0f,  400.0f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3( 400.0f,  200.0f,  400.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3(-400.0f,  200.0f,  400.0f));
	rs->endVertices();

	// Render the right quad
	rs->bindTexture(matStage->getTexture(CUBE_RIGHT)->getPointer(), 0);
	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3(-400.0f, -200.0f, -400.0f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3(-400.0f, -200.0f,  400.0f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3(-400.0f,  200.0f,  400.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3(-400.0f,  200.0f, -400.0f));
	rs->endVertices();

	// Render the top quad
	rs->bindTexture(matStage->getTexture(CUBE_UP)->getPointer(), 0);
	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3(-400.0f,  200.0f, -400.0f));
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3(-400.0f,  200.0f,  400.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 400.0f,  200.0f,  400.0f));
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3( 400.0f,  200.0f, -400.0f));
	rs->endVertices();

	// Render the bottom quad
	rs->bindTexture(matStage->getTexture(CUBE_DOWN)->getPointer(), 0);
	rs->startVertices(VERTEXMODE_QUAD);
		rs->texCoord(vector2(1, 0)); rs->vertex(vector3(-400.0f, -200.0f, -400.0f));
		rs->texCoord(vector2(1, 1)); rs->vertex(vector3(-400.0f, -200.0f,  400.0f));
		rs->texCoord(vector2(0, 1)); rs->vertex(vector3( 400.0f, -200.0f,  400.0f));
		rs->texCoord(vector2(0, 0)); rs->vertex(vector3( 400.0f, -200.0f, -400.0f));
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
}

void renderer::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();

	if (mRenderToTexture)
		rs->setViewPort(0, 0, mRTTSize[0], mRTTSize[1]);

	/**
	 * Call the frame start
	 */
	rs->frameStart();

	// Time since frame start.
	mFrameTime.reset();

	// Set default perspective
	if (mActiveCamera)
	{
		mActiveCamera->setPerspective();
	}
	else
	{
		rs->setMatrixMode(MATRIXMODE_PROJECTION);
		rs->identityMatrix();
		rs->setPerspective(90, 1.33, 0.1, 1000.0f);
	}

	// Draw Skybox, Plane
	_drawSkybox();
	_drawSkyPlane();

	rs->setDepthTest(true);
	if (mActiveCamera)
	{
		mActiveCamera->setPerspective();
		if ((mActiveCamera->getPosition() != mLastCameraPos) ||
			 (mActiveCamera->getOrientation() != mLastCameraOrientation))
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
	 * Draw world
	 */
	if (mActiveWorld && mActiveCamera)
	{
		mActiveCamera->copyView();
		mActiveWorld->draw(mActiveCamera);
	}

	/**
	 * Camera is ready, draw objects
	 * keep in mind that you wont call identityMatrix() on the
	 * rendersystem (for modelview), otherwise it will 
	 * remove camera parameters.
	 */
	for (std::list<drawable3D*>::const_iterator it = m3DObjects.begin(); it != m3DObjects.end(); ++it)
	{
		bool lightFound = false;
		unsigned int lightIndex = 0;

		drawable3D* obj = *it;
		kAssert(obj);

		// Frustum test obj against camera
		if (mActiveCamera && !mActiveCamera->isBoxInsideFrustum(obj->getAABoundingBox()))
			continue;

		// loop lights
		std::list<light::light*>::const_iterator lightIt;
		for (lightIt = mLights.begin(); lightIt != mLights.end(); lightIt++)
		{
			if (!(*lightIt)->getEnabled() || !(*lightIt)->isInLightRange(obj->getAbsolutePosition()))
				continue;

			if (lightIndex >= 8)
				break;

			// Light is valid for this object
			if (!lightFound)
			{
				rs->setLighting(true);
				lightFound = true;
			}

			// Lets Setup
			rs->setLightPosition(lightIndex, (*lightIt)->getPosition(), false);
			rs->setLightDiffuse(lightIndex, (*lightIt)->getDiffuse());
			rs->setLightSpecular(lightIndex, (*lightIt)->getSpecular());
			rs->setLightAmbient(lightIndex, (*lightIt)->getAmbient());
			rs->setLightAttenuation(lightIndex, (*lightIt)->getAttenuation());
			rs->setLight(lightIndex, true);

			lightIndex++;
		}

		obj->draw();

		if (lightFound)
			rs->setLighting(false);
	}

	std::list<sprite*>::const_iterator it;
	for (it = mSprites.begin(); it != mSprites.end(); it++)
	{
		sprite* spr = (*it);
		kAssert(spr);
		spr->draw();
	}

	// Particles
	particle::manager::getSingleton().drawParticles();

	// Render to Texture
	if (!mRenderToTexture)
	{
		/**
		 * Set the 2D projection here and draw the 2d objects on it
		 */
		rs->setMatrixMode(MATRIXMODE_PROJECTION);
		rs->setOrthographic(0, rs->getScreenWidth(), rs->getScreenHeight(), 0, -128, 128);

		for (std::list<drawable2D*>::const_iterator dit = m2DObjects.begin(); dit != m2DObjects.end(); dit++)
		{
			drawable2D* obj = (*dit);
			kAssert(obj != NULL);

			rs->setMatrixMode(MATRIXMODE_MODELVIEW);
			rs->identityMatrix();

			obj->draw();
		}
	}

	/**
	 * Call the frame end
	 */
	rs->frameEnd();

	if (mRenderToTexture)
	{
		rs->setViewPort(0, 0, rs->getScreenWidth(), rs->getScreenHeight());
		mRenderToTexture = false;
	}
			
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
	kAssert(cam != NULL);
	mActiveCamera = cam;

	mLastCameraPos = cam->getPosition();
}

camera* renderer::getCamera()
{
	return mActiveCamera;
}
			
long renderer::getTimeNow()
{
	return mRendererTimer.getMilliSeconds();
}

void renderer::prepareRTT(unsigned int w, unsigned int h, platformTexturePointer* tex)
{
	renderSystem* rs = root::getSingleton().getRenderSystem();

	rs->setRttSize(w, h);
	rs->setRttTarget(tex);

	mRenderToTexture = true;
}

}

