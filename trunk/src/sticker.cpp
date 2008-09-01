#include "sticker.h"
#include "root.h"
#include "materialManager.h"

namespace k {
			
sticker::sticker(const std::string& matName)
{
	materialManager* mm = root::getSingleton().getMaterialManager();
	mMaterial = mm->createMaterial(matName);

	assert(mMaterial != NULL);
}

sticker::~sticker()
{
}

material* sticker::getMaterial()
{
	return mMaterial;
}

void sticker::draw()
{
	renderSystem* rs = root::getSingleton().getRenderSystem();
	assert(rs != NULL);

	// Prepare the material
	mMaterial->prepare();

	rs->startVertices(VERTEXMODE_QUAD);

	rs->texCoord(vector2(0.0f, 1.0f));
	rs->vertex(vector3(mPosition.x, mPosition.y, 0));

	rs->texCoord(vector2(1.0f, 1.0f));
	rs->vertex(vector3(mPosition.x + mScale.x, mPosition.y, 0));

	rs->texCoord(vector2(1.0f, 0.0f));
	rs->vertex(vector3(mPosition.x + mScale.x, mPosition.y + mScale.y, 0));

	rs->texCoord(vector2(0.0f, 0.0f));
	rs->vertex(vector3(mPosition.x, mPosition.y + mScale.y, 0));

	rs->endVertices();
}

}

