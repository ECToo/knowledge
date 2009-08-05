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

#ifndef _MATERIAL_MANAGER_H
#define _MATERIAL_MANAGER_H

#include "prerequisites.h"
#include "fileParser.h"
#include "singleton.h"
#include "material.h"

namespace k
{
	typedef std::list<std::string> materialList;
	typedef std::map<std::string, material*> materialMap;

	class DLL_EXPORT materialManager : public singleton<materialManager>
	{
		private:
			materialMap mMaterials;

		public:
			materialManager();
			~materialManager();

			static materialManager& getSingleton();

			material* createMaterial(const std::string& name);
			material* createMaterial(const std::string& name, texture* tex);
			material* createMaterial(const std::string& name, const std::string& filename);

			material* getMaterial(const std::string& name) const;
			material* getMaterialWithFilename(const std::string& filename) const;

			void destroyMaterial(const std::string& name);

			void parseTextureSection(material* mat, parsingFile* file, unsigned short index);

			void parseMaterialScript(const std::string& filename, materialList* map = NULL);
			void parseMaterialScript(parsingFile* file, materialList* map = NULL);

			void parseMaterial(material* mat, parsingFile* file);

			/**
			 * Create System materials (k_base_white, k_base_black, k_base_null)
			 */
			void createSystemMaterials();

			// Quake 3 Shaders
			void parseQ3TextureSection(material* mat, parsingFile* file, unsigned short index);
			void parseQ3MaterialScript(const std::string& filename, materialList* map = NULL);
			void parseQ3MaterialScript(parsingFile* file, materialList* map = NULL);
			void parseQ3Material(material* mat, parsingFile* file);
	};
}

#endif

