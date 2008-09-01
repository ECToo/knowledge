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

