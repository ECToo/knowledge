#ifndef _MATERIAL_MANAGER_H
#define _MATERIAL_MANAGER_H

#include "prerequisites.h"
#include "fileAccess.h"
#include "singleton.h"
#include "material.h"

namespace k
{
	class materialManager : public singleton<materialManager>
	{
		private:
			std::map<std::string, material*> mMaterials;

		public:
			materialManager();
			~materialManager();

			static materialManager& getSingleton();

			material* createMaterial(const std::string& name);
			material* getMaterial(const std::string& name);
			void destroyMaterial(const std::string& name);

			void parseMaterialScript(parsingFile* file);
	};
}

#endif

