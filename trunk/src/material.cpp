#include "material.h"
#include "root.h"

namespace k {

void material::prepare()
{
	// Material Properties
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	rs->matAmbient(mAmbient);
	rs->matDiffuse(mDiffuse);
	rs->matSpecular(mSpecular);

	// Cycle through textures
	std::list<texture*>::iterator it;
	for (it = mTextures.begin(); it != mTextures.end(); it++)
	{
		texture* tex = (*it);
		assert(tex != NULL);
		
		tex->draw();
	}
}
			
void material::setAmbient(const vector3& color)
{
	mAmbient = color;
}

void material::setDiffuse(const vector3& color)
{
	mDiffuse = color;
}

void material::setSpecular(const vector3& color)
{
	mSpecular = color;
}
			
void material::pushTexture(texture* tex)
{
	assert(tex != NULL);
	mTextures.push_back(tex);
}

}

