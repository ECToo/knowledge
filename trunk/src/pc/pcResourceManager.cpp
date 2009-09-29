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

#include "resourceManager.h"
#include "textureManager.h"
#include "materialManager.h"
#include "logger.h"
#include "root.h"
#include "loadscr.h"
	
#include <dirent.h>

namespace k {

void resourceGroup::filterResource(const std::string& path, bool material)
{
	std::string extension = getExtension(path);
	if (!extension.length())
		return;

	// Check if we have a loading screen
	loadScreen* loadingScreen = resourceManager::getSingleton().getLoadingScreen();

	if (material && (mLoadOptions & (1 << LOAD_MATERIALS)) && extension == ".material")
	{
		if (loadingScreen)
			loadingScreen->update(path);

		materialManager::getSingleton().parseMaterialScript(path, &mMaterials);
	}
	else
	if (!material && (mLoadOptions & (1 << LOAD_SCRIPTS)) && extension == ".particle")
	{
		if (loadingScreen)
			loadingScreen->update(path);

		particle::manager::getSingleton().parseScript(path);
	}
}

void resourceGroup::scanDir(std::string path, bool recursive, bool materialParsing)
{
	DIR* dp;
	struct dirent* dirp;

	if ((dp  = opendir(path.c_str())) == NULL) 
	{
		S_LOG_INFO("Failed to access directory " + path + ".");
		return;
	}
    
	while ((dirp = readdir(dp)) != NULL) 
	{
		if (!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		std::string fullPath = path + dirp->d_name;
		DIR* tmpDirPtr = opendir(fullPath.c_str());

		if (!tmpDirPtr)
		{
			// Do something with file names
			filterResource(fullPath, materialParsing);
		}
		else
		if (recursive)
		{
			// This is a directory
			// save the original filename
			std::string original = path;

			path += dirp->d_name;
			path += "/";

			scanDir(path, false, materialParsing);

			path = original;
		}
			
		closedir(tmpDirPtr);
	}

	closedir(dp);
}

}

