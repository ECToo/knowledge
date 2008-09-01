#include "renderer.h"
#include "root.h"

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
}

renderer::~renderer()
{
	//TODO
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
	// Save cpu cycles
	#ifndef WIN32
		usleep(1);
	#else
		Sleep(1);
	#endif
		
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	/**
	 * Call the frame start
	 */
	rs->frameStart();

	/**
	 * We need to set the camera projection here and draw
	 * the 3d objects after it.
	 */
	std::list<drawable3D*>::iterator it;
	for (it = m3DObjects.begin(); it != m3DObjects.end(); it++)
	{
		drawable3D* obj = (*it);
		assert(obj != NULL);

		obj->draw();
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

	std::list<drawable2D*>::iterator dit;
	for (dit = m2DObjects.begin(); dit != m2DObjects.end(); dit++)
	{
		drawable2D* obj = (*dit);
		assert(obj != NULL);

		obj->draw();
	}

	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	rs->popMatrix();

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->popMatrix();

	rs->setDepthTest(true);

	/**
	 * Call the frame end
	 */
	rs->frameEnd();
}

}

