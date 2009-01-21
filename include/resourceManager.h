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

#ifndef _RESOURCE_MANAGER_H
#define _RESOURCE_MANAGER_H

#include "prerequisites.h"
#include "fileAccess.h"
#include "singleton.h"
#include "loadscr.h"

#include "material.h"
#include "md5.h"

namespace k
{
	enum groupLoading
	{
		LOAD_TEXTURES,
		LOAD_MATERIALS,
		LOAD_MODELS,
		LOAD_SCRIPTS
	};

	class resourceGroup
	{
		private:
			std::string mRoot;
			char mLoadOptions;
			bool mRecursive;

			std::list<std::string> mMaterials;
			std::map<std::string, md5model*> mModels;

		public:
			~resourceGroup();

			const std::string& getRoot();
			bool getRecursivity();

			void setRoot(const std::string& root);
			void setRecursivity(bool recursive);
			void setLoadOptions(char opt);

			void scanDir(std::string dir, bool recursive, bool materialParsing);
			void filterResource(const std::string& path, bool material);
	};

	class resourceManager : public singleton<resourceManager>
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

		public:
			resourceManager(const std::string& resourceCfg = "./resources.cfg");
			~resourceManager();

			static resourceManager& getSingleton();

			void loadGroup(const std::string& name);
			void unloadGroup(const std::string& name);

			loadScreen* getLoadingScreen();
			void setLoadingScreen(loadScreen* scr);
			void unsetLoadingScreen();

			const std::string& getRoot();
	};
}

#endif

