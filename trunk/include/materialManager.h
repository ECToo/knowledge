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

#ifndef _MATERIAL_MANAGER_H
#define _MATERIAL_MANAGER_H

#include "prerequisites.h"
#include "fileAccess.h"
#include "singleton.h"
#include "material.h"
#include "tev.h"

namespace k
{
	class materialManager : public singleton<materialManager>
	{
		private:
			std::map<std::string, material*> mMaterials;

			#ifdef __WII__
			tevManager* mTev;
			#endif

		public:
			materialManager();
			~materialManager();

			static materialManager& getSingleton();

			material* createMaterial(const std::string& name);
			material* getMaterial(const std::string& name);
			void destroyMaterial(const std::string& name);

			void parseTextureSection(material* mat, parsingFile* file, unsigned short index);
			void parseMaterialScript(parsingFile* file);
			void parseMaterial(material* mat, parsingFile* file);
	};
}

#endif

