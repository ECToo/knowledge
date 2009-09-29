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

	/**
	 * \brief The material manager.
	 * This class is responsible for handling material creation, loading and parsing.
	 */
	class DLL_EXPORT materialManager : public singleton<materialManager>
	{
		private:
			materialMap mMaterials;

		public:
			materialManager();
			~materialManager();

			/**
			 * Returns the singleton instance of the materialManager
			 */
			static materialManager& getSingleton();

			/**
			 * Create a new material, if material exists, return existing material.
			 * @param name The new material name.
			 */
			material* createMaterial(const std::string& name);

			/**
			 * Create a new material with a single texture stage, if it exists, return existing material.
			 * @param name The new material name.
			 * @param tex A valid pointer to a texture.
			 */
			material* createMaterial(const std::string& name, texture* tex);

			/**
			 * Create a new material with a texture on it, if it exists, return existing material.
			 * @param name The new material name.
			 * @param tex A full path to the texture file.
			 */
			material* createMaterial(const std::string& name, const std::string& filename);

			/**
			 * Returns an material by name.
			 * @param name The material name.
			 */
			material* getMaterial(const std::string& name) const;

			/**
			 * Returns an material by file name.
			 * @param name The material file name.
			 */
			material* getMaterialWithFilename(const std::string& filename) const;

			/**
			 * Deallocate (destroy) a material by name.
			 * @param name The material name.
			 */
			void destroyMaterial(const std::string& name);

			void parseTextureSection(material* mat, parsingFile* file, unsigned short index);

			/**
			 * Parse a material script by name.
			 * @param filename The script path.
			 * @param[out] map A list of loaded materials, in case you want to know what where loaded on the script.
			 */
			void parseMaterialScript(const std::string& filename, materialList* map = NULL);

			/**
			 * Parse a material script by parsingFile.
			 * @param file The script in a parsingFile.
			 * @param[out] map A list of loaded materials, in case you want to know what where loaded on the script.
			 */
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

