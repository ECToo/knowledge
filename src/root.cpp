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
	delete mInputManager;
	delete mGuiManager;
	delete mRenderer;
	delete mTextureManager;
	delete mMaterialManager;
	delete mParticleManager;
	delete mLogger;
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
			
unsigned int getHashKey(const std::string& filename)
{
	unsigned int hashKey = 0;

	// Generic key generation
	for (unsigned int i = 0; i < filename.length(); i++)
		hashKey += filename[i] * i;

	return hashKey;
}

long root::getGlobalTime()
{
	return mGlobalTimer.getMilliSeconds();
}
	
bool isNumeric(const std::string& str)
{
	std::istringstream sStr(str);
	double num;
	sStr >> num;

	if (!sStr) return false;
	return (sStr.rdbuf()->in_avail() == 0);
}

} // namespace k

