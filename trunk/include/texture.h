#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "prerequisites.h"
#include "vector2.h"

namespace k
{
	class texture 
	{
		private:
			/**
			 * Amount to scroll per frame on x and y
			 */
			vector2 mScroll;

			/**
			 * Amount to rotate per frame in degrees
			 */
			vec_t mRotate;

			/**
			 * Internals of the texture file
			 */
			unsigned int mWidth, mHeight;

			#ifdef __WII__
				GXTexObj* mTextureId;
			#else
				GLuint mTextureId;
				ILuint* mDevilTextureId;
			#endif

		public:
			#ifndef __WII__
			texture(ILuint* src, unsigned int width, unsigned int height);
			#else
			texture(GXTexObj* src, unsigned int width, unsigned int height);
			#endif

			#ifndef __WII__
			GLuint& getTextureId();
			#else
			GXTexObj* getTextureId();
			#endif

			void draw();

			unsigned int getWidth();
			unsigned int getHeight();
	};
}

#endif

