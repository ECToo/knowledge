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

#include "resourceManager.h"
#include "textureManager.h"
#include "materialManager.h"
#include "logger.h"

#ifndef __WII__
#include <dirent.h>
#endif

namespace k {

void resourceGroup::setRoot(const std::string& root)
{
	mRoot = root;
}

void resourceGroup::setRecursivity(bool recursive)
{
	mRecursive = recursive;
}
			
void resourceGroup::setLoadOptions(char opt)
{
	mLoadOptions = opt;
}

const std::string& resourceGroup::getRoot()
{
	return mRoot;
}

bool resourceGroup::getRecursivity()
{
	return mRecursive;
}
			
resourceGroup::~resourceGroup()
{
	// Unload loaded materials
	if (mLoadOptions & (1 << LOAD_MATERIALS))
	{
		std::list<std::string>::iterator it;
		for (it = mMaterials.begin(); it != mMaterials.end(); )
		{
			materialManager::getSingleton().destroyMaterial(*it);
			it = mMaterials.erase(it);
		}
	}

}

template<> resourceManager* singleton<resourceManager>::singleton_instance = 0;

resourceManager& resourceManager::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}
			
static inline std::string getExtension(const std::string& file)
{
	char* dot = strstr(file.c_str(), ".");

	if (!dot)
		return std::string("");
	else
	{
		unsigned int pos = (strrchr(file.c_str(), '.') - file.c_str());
		std::string extension;
		extension.append(file.c_str() + pos, file.length() - pos); 

		return extension;
	}
}

void resourceGroup::filterResource(const std::string& path)
{
	std::string extension = getExtension(path);
	if (!extension.length())
		return;

	if ((mLoadOptions & (1 << LOAD_MATERIALS)) && extension == ".material")
		materialManager::getSingleton().parseMaterialScript(path, &mMaterials);
}

#ifndef __WII__
void resourceGroup::scanDir(std::string path, bool recursive)
{
	DIR *dp;
	struct dirent *dirp;

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
		if (opendir(fullPath.c_str()) == NULL)
		{
			// Do something with file names.
			filterResource(fullPath);
		}
		else
		if (recursive)
		{
			// This is a directory
			// save the original filename
			std::string original = path;

			path += dirp->d_name;
			path += "/";

			scanDir(path, false);

			path = original;
		}
	}

	closedir(dp);
}
#else
void resourceGroup::scanDir(std::string path, bool recursive)
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
			filterResource(fullPath);
		}
		else
		if (recursive)
		{
			// This is a directory
			// save the original filename
			std::string original = path;

			path += filename;
			path += "/";

			scanDir(path, false);

			path = original;
		}
	}

	dirclose(dp);
}
#endif

// Helper
static inline std::string getDirectory(const std::string& path)
{
	const char* rightBar = strstr(path.c_str(), "/");
	const char* leftBar = strstr(path.c_str(), "\\");

	if (!rightBar && !leftBar)
	{
		return "./";
	}
	else
	{
		// String does have a directory
		unsigned int pos = 0;
		std::string tempPath;

		if (rightBar)
		{
			pos = (strrchr(path.c_str(), '/') - path.c_str() + 1);
		}
		else
		{
			pos = (strrchr(path.c_str(), '\\') - path.c_str() + 1);
		}

		tempPath.clear();
		tempPath.append(path.c_str(), pos);
		
		return tempPath;
	}
}
			
resourceGroup* resourceManager::findGroup(const std::string& name)
{
	std::map<std::string, resourceGroup*>::const_iterator it = mGroups.find(name);
	if (it != mGroups.end())
		return it->second;
	else
		return NULL;
}
			
void resourceManager::parseGroup(parsingFile* file, resourceGroup* group)
{
	assert(group != NULL);
	assert(file != NULL);

	std::string token = file->getNextToken(); // {
	unsigned int openBraces = 1;

	std::string mRoot;
	bool recursive = false;
	char loadOptions = 0xff;

	while (openBraces)
	{
		if (file->eof())
			return;

		if (token == "root")
		{
			token = file->getNextToken();
			mRoot = token;
		}
		else
		if (token == "recursive")
		{
			token = file->getNextToken();
			if (token == "true")
				recursive = true;
		}
		else
		if (token == "loadMaterials")
		{
			token = file->getNextToken();
			if (token == "false")
				loadOptions &= ~(1 << LOAD_MATERIALS);
		}
		else
		if (token == "loadModels")
		{
			token = file->getNextToken();
			if (token == "false")
				loadOptions &= ~(1 << LOAD_MODELS);
		}
		else
		if (token == "loadTextures")
		{
			token = file->getNextToken();
			if (token == "false")
				loadOptions &= ~(1 << LOAD_TEXTURES);
		}
		else
		if (token == "loadScripts")
		{
			token = file->getNextToken();
			if (token == "false")
				loadOptions &= ~(1 << LOAD_SCRIPTS);
		}

		token = file->getNextToken();

		// Script properties
		if (token == "}")
			openBraces--;
		else
		if (token == "{")
			openBraces++;
	}

	group->setRecursivity(recursive);
	group->setRoot(mRoot);
	group->setLoadOptions(loadOptions);
}

resourceManager::resourceManager(const std::string& resourceCfg)
{
	mGroups.clear();

	// Save the base path from the resource file full path
	parsingFile* script = new parsingFile(resourceCfg);	
	assert(script != NULL);

	if (!script->isReady())
		return;

	mBasePath = getDirectory(resourceCfg);

	std::string mLastGroupName;
	std::string token = script->getNextToken();
	while (!script->eof())
	{
		if (token == "group")
		{
			token = script->getNextToken();
			mLastGroupName = token;

			if (findGroup(token))
			{
				unsigned int openBraces = 0;

				S_LOG_INFO("Resource Group " + token + " already exists.");
				script->skipNextToken(); // {
				while(true)
				{
					token = script->getNextToken();
					if (token == "{")
						openBraces++;
					else
					if (token == "}")
					{
						if (openBraces == 0)
							break;
						else
							openBraces--;
					}
				}
			}
			else
			{
				resourceGroup* newGroup = new resourceGroup();
				assert(newGroup != NULL);

				parseGroup(script, newGroup);
				mGroups[mLastGroupName] = newGroup;
				S_LOG_INFO("Resource group " + mLastGroupName + " loaded.");
			}
		}

		// Next Token
		token = script->getNextToken();
	}

	delete script;
}

resourceManager::~resourceManager()
{
	std::map<std::string, resourceGroup*>::iterator it;
	for (it = mGroups.begin(); it != mGroups.end(); )
	{
		resourceGroup* rsc = it->second;
		mGroups.erase(it++);
		delete rsc;
	}
}
			
void resourceManager::loadGroup(const std::string& name)
{
	resourceGroup* group = findGroup(name);
	if (!group)
	{
		S_LOG_INFO("Resource group " + name + " doesnt exist.");
		return;
	}

	group->scanDir(mBasePath + group->getRoot(), group->getRecursivity());
}

void resourceManager::unloadGroup(const std::string& name)
{
	std::map<std::string, resourceGroup*>::iterator it = mGroups.find(name);
	if (it != mGroups.end())
	{
		resourceGroup* rsc = it->second;
		mGroups.erase(it++);
		delete rsc;

		S_LOG_INFO("Resource group " + name + " unloaded.");
	}
	else
	{
		S_LOG_INFO("Resource group " + name + " doesnt exist.");
	}
}
			
const std::string& resourceManager::getRoot()
{
	return mBasePath;
}

}

