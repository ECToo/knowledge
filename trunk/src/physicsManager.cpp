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

#include "physicsManager.h"
#include "logger.h"
#include "root.h"

namespace k {

template<> physicsManager* singleton<physicsManager>::singleton_instance = 0;

physicsManager& physicsManager::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}
			
void physicsManager::setWorld(const std::string& name)
{
}

void physicsManager::setGravity(vector3 grav)
{
}

void physicsManager::setSpace(const std::string& name)
{
}

void physicsManager::createJointGroup(const std::string& name)
{
}

physicSphere::physicSphere(dWorldID wId, dSpaceID sId, vec_t radius)
{
	mGeom = dCreateSphere(sId, radius);
	mBody = dBodyCreate(wId);

	dGeomSetBody(mGeom, mBody);
}

physicSphere::~physicSphere()
{
	dGeomDestroy(mGeom);
	// TODO: Destroy Body?
}

}

