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

