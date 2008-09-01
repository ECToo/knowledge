#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "prerequisites.h"
#include "drawable.h"
#include "singleton.h"

namespace k
{
	class renderer : public singleton<renderer>
	{
		private:
			std::list<drawable3D*> m3DObjects;
			std::list<drawable2D*> m2DObjects;

		public:
			renderer();
			~renderer();

			static renderer& getSingleton();

			void push3D(drawable3D* object);
			void pop3D(drawable3D* object);

			void push2D(drawable2D* object);
			void pop2D(drawable2D* object);

			void draw();
	};
}

#endif

