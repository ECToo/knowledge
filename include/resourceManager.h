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

#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H

#include "prerequisites.h"
#include "fileParser.h"
#include "singleton.h"
#include "loadscr.h"

#include "material.h"
#include "md5.h"

namespace k
{
	/**
	 * Group loading types
	 */
	enum groupLoading
	{
		LOAD_TEXTURES,
		LOAD_MATERIALS,
		LOAD_MODELS,
		LOAD_SCRIPTS
	};

	/**
	 * \brief A group in resource manager, please check resource manager documentation.
	 */
	class DLL_EXPORT resourceGroup
	{
		private:
			std::string mRoot;
			char mLoadOptions;
			bool mRecursive;

			std::list<std::string> mMaterials;
			std::map<std::string, md5model*> mModels;

		public:
			/**
			 * Destructor.
			 */
			~resourceGroup();

			/**
			 * Return the root path of this resourceGroup
			 */
			const std::string& getRoot();

			/**
			 * Return true if this group looks recursively for data.
			 */
			bool getRecursivity();

			/**
			 * Set group root path.
			 */
			void setRoot(const std::string& root);
			
			/**
			 * Set the group recursivity.
			 */
			void setRecursivity(bool recursive);

			/**
			 * Set group load options. @see groupLoading
			 */
			void setLoadOptions(char opt);

			/**
			 * Scan a directory, following options it will allocate found data.
			 */
			void scanDir(std::string dir, bool recursive, bool materialParsing);
			void filterResource(const std::string& path, bool material);
	};

	/**
	 * \brief Responsible for automatically load and unload resources from disk.
	 * Please check the resource manager documentation.
	 */
	class DLL_EXPORT resourceManager : public singleton<resourceManager>
	{
		private:
			void parseGroup(parsingFile* file, resourceGroup* group);

			std::map<std::string, resourceGroup*> mGroups;
			resourceGroup* findGroup(const std::string& name);

			/*
			 * Base path, where all resources groups will
			 * start loading things.
			 */
			std::string mBasePath;

			/**
			 * For loading screens ;)
			 */
			loadScreen* mLoadingScreen;

			/**
			 * Memory usage
			 */
			unsigned long mUsedMemory;

		public:
			/**
			 * Constructor. You need to pass the resource manager the full path
			 * of the resources file, and this path will turn to the global looking path.
			 *
			 * @param[in] resourceCfg The full path to the resources file.
			 */
			resourceManager(const std::string& resourceCfg = "./resources.cfg");

			/**
			 * Destructor.
			 */
			~resourceManager();

			/**
			 * Return an instance of this resourceManager singleton.
			 */
			static resourceManager& getSingleton();

			/**
			 * Load a resource group by name.
			 * @param[in] name The name of the group to be loaded.
			 */
			void loadGroup(const std::string& name);

			/**
			 * Unload a resource group by name.
			 * @param[in] name The name of the group to be unloaded.
			 */
			void unloadGroup(const std::string& name);

			/**
			 * Add some amount of memory to the memory usage counter.
			 *
			 * @param[in] m The amoun to add to the counter.
			 */
			void addMemoryUse(unsigned long m);

			/**
			 * Return the memory usage of resource manager allocated data.
			 */
			unsigned long getMemoryUsage();

			/**
			 * Return the active loading screen if theres one, otherwise returns NULL.
			 */
			loadScreen* getLoadingScreen();

			/**
			 * Set the active loading screen.
			 *
			 * @param[in] scr A valid pointer to the active loading screen.
			 */
			void setLoadingScreen(loadScreen* scr);

			/**
			 * Unset the loading screen internal pointers.
			 */
			void unsetLoadingScreen();

			/**
			 * Return full path of the resourceManager root. Please check the resourceManager documentation.
			 */
			const std::string& getRoot();
	};
}

#endif

