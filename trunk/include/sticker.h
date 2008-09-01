#ifndef _STICKER_H_
#define _STICKER_H_

#include "prerequisites.h"
#include "drawable.h"
#include "material.h"

namespace k
{
	class sticker : public drawable2D
	{
		protected:
			material* mMaterial;

		public:
			sticker(const std::string& matName);
			~sticker();

			material* getMaterial();
			void draw();
	};
}

#endif

