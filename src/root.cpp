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

#include "root.h"
#include "logger.h"

namespace k {

template<> root* singleton<root>::singleton_instance = 0;

root& root::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

// root::root is implemented in platform specific fashion

root::~root()
{
	delete mActiveRS;
	delete mRenderer;
	delete mTextureManager;
	delete mMaterialManager;
}
			
particle::manager* root::getParticleManager()
{
	return mParticleManager;
}
			
renderSystem* root::getRenderSystem()
{
	return mActiveRS;
}
			
renderer* root::getRenderer()
{
	return mRenderer;
}

materialManager* root::getMaterialManager()
{
	return mMaterialManager;
}
			
guiManager* root::getGuiManager()
{
	return mGuiManager;
}

inputManager* root::getInputManager()
{
	return mInputManager;
}
	
std::string getExtension(const std::string& file)
{
	char* dot = strstr(file.c_str(), ".");

	if (!dot)
	{
		return std::string("");
	}
	else
	{
		unsigned int pos = (strrchr(file.c_str(), '.') - file.c_str());
		std::string extension;
		extension.append(file.c_str() + pos, file.length() - pos); 

		return extension;
	}
}
			
long root::getGlobalTime()
{
	return mGlobalTimer.getMilliSeconds();
}

} // namespace k

