#include "texture.h"
#include "root.h"

namespace k {

#ifndef __WII__
texture::texture(ILuint* src, unsigned int width, unsigned int height)
#else
texture::texture(GXTexObj* src, unsigned int width, unsigned int height)
#endif
{
	assert(src != NULL);

	#ifndef __WII__
	mDevilTextureId = src;
	ilBindImage(*mDevilTextureId);
	ilutRenderer(ILUT_OPENGL);
	mTextureId = ilutGLBindTexImage();
	#else
	mTextureId = *src;
	#endif

	mWidth = width;
	mHeight = height;
}

#ifndef __WII__
GLuint& texture::getTextureId()
#else
GXTexObj& texture::getTextureId()
#endif
{
	return mTextureId;
}
			
unsigned int texture::getWidth()
{
	return mWidth;
}

unsigned int texture::getHeight()
{
	return mHeight;
}

void texture::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	rs->bindTexture(mTextureId);
}

}

