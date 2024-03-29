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

#include "renderer.h"
#include "root.h"
#include "logger.h"
#include "guiManager.h"

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

	// Deallocate lights
	std::list<light::light*>::iterator it;
	for (it = mLights.begin(); it != mLights.end(); ++it)
	{
		light::light* tempLight = (*it);
		delete tempLight;
	}

	mLights.clear();
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

	if (first->getZ() < second->getZ())
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
	for (it = m2DObjects.begin(); it != m2DObjects.end(); it++)
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

	rs->setVertexCount(4);
	rs->drawArrays(true);

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
				if ((*it)->isVisible())
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

		if (!obj->isVisible())
			continue;

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
				
		if (!(*it)->isVisible())
			continue;

		if (mActiveCamera && !mActiveCamera->isPointInsideFrustum(spr->getPosition()))
			continue;

		bool lightFound = false;
		unsigned int lightIndex = 0;

		// loop lights
		std::list<light::light*>::const_iterator lightIt;
		for (lightIt = mLights.begin(); lightIt != mLights.end(); lightIt++)
		{
			if (!(*lightIt)->getEnabled() || !(*lightIt)->isInLightRange(spr->getPosition()))
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

		spr->draw();

		if (lightFound)
			rs->setLighting(false);
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
			kAssert(obj);
		
			// Dont draw if object is invisible
			if (!obj->isVisible())
				continue;

			vector2 objPos = obj->getAbsolutePosition();
			vector2 objDimension = obj->getDimension();

			vector2 screenDim;
			screenDim.x = rs->getScreenWidth(); 
			screenDim.y = rs->getScreenHeight(); 

			// cull object from screen
			if (objPos.x > screenDim.x ||
				 objPos.y > screenDim.y ||
				 objDimension.x + objPos.x < 0 ||
				 objDimension.y + objPos.y < 0)
			{
				return;
			}

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

	// Update GUI Manager
	guiManager::getSingleton().update();
			
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

