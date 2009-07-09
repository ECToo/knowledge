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

