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

#include "materialManager.h"

namespace k {

template<> materialManager* singleton<materialManager>::singleton_instance = 0;

materialManager& materialManager::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}

materialManager::materialManager()
{
	mMaterials.clear();
}

materialManager::~materialManager()
{
	//TODO: Free all allocated materials
}
			
material* materialManager::createMaterial(const std::string& name)
{
	material* newMaterial = getMaterial(name);
	if (newMaterial)
	{
		// Material already exists
		return newMaterial;
	}
	else
	{
		// Allocate a new material
		newMaterial = new material();
		if (newMaterial)
		{
			mMaterials[name] = newMaterial;
			return newMaterial;
		}
	}

	return NULL;
}

material* materialManager::getMaterial(const std::string& name)
{
	std::map<std::string, material*>::iterator it = mMaterials.find(name);
	if (it != mMaterials.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}

void materialManager::destroyMaterial(const std::string& name)
{
	std::map<std::string, material*>::iterator it = mMaterials.find(name);
	if (it != mMaterials.end())
	{
		material* mat = it->second;

		mMaterials.erase(it);
		delete mat;
	}
}
		
void materialManager::parseMaterialScript(parsingFile* file)
{
}

}

