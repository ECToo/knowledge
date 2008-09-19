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

	rs->setMatrixMode(MATRIXMODE_MODELVIEW);
	rs->identityMatrix();

	rs->setMatrixMode(MATRIXMODE_PROJECTION);
	rs->identityMatrix();
	rs->setPerspective(90, (vec_t)rs->getScreenWidth()/rs->getScreenHeight(), 0.1, 5000.0f);

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

	rs->frameEnd();
	return;

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

