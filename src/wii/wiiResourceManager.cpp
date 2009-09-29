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
#include "particle.h"

#include <dirent.h>
#include "tev.h"

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
	// custom TEV scripts
	else
	if (mLoadOptions & (1 << LOAD_SCRIPTS) && extension == ".script")
	{
		if (loadingScreen)
			loadingScreen->update(path);

		tevManager::getSingleton().parseTevScript(path);
	}
}

void resourceGroup::scanDir(std::string path, bool recursive, bool materialParsing)
{
	// Hold Wii directories
	DIR_ITER *dp = NULL;

	// For each dirnext filename
	char filename[FILENAME_MAX + 1];

	if ((dp  = diropen(path.c_str())) == NULL) 
	{
		S_LOG_INFO("Failed to access directory " + path + ".");
		return;
	}
    
	while (dirnext(dp, filename, NULL) == 0) 
	{
		if (!strcmp(filename, ".") || !strcmp(filename, ".."))
			continue;

		std::string fullPath = path + filename;
		if (diropen(fullPath.c_str()) == NULL)
		{
			// Do something with file names.
			filterResource(fullPath, materialParsing);
		}
		else
		if (recursive)
		{
			// This is a directory
			// save the original filename
			std::string original = path;

			path += filename;
			path += "/";

			scanDir(path, false, materialParsing);

			path = original;
		}
	}

	dirclose(dp);
}

}

