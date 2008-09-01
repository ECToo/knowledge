#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "prerequisites.h"
#include "vector3.h"
#include "texture.h"

namespace k
{
	class material
	{
		private:
			vector3 mAmbient;
			vector3 mDiffuse;
			vector3 mSpecular;

			std::list<texture*> mTextures;

		public:
			void setAmbient(const vector3& color);
			void setDiffuse(const vector3& color);
			void setSpecular(const vector3& color);

			void pushTexture(texture* tex);
			void prepare();
	};
}

#endif

