#include "root.h"
#include "glRenderSystem.h"
#include "wiiRenderSystem.h"

namespace k {

template<> root* singleton<root>::singleton_instance = 0;

root& root::getSingleton()
{  
	assert(singleton_instance);
	return (*singleton_instance);  
}

root::root()
{
	// Initialize the render system
	#ifndef __WII__
	mActiveRS = new glRenderSystem();
	#else
	mActiveRS = new wiiRenderSystem();
	#endif

	mActiveRS->initialise();
	mActiveRS->configure();

	// Create the renderer
	mRenderer = new renderer();

	// Create a new Texture Manager
	mTextureManager = new textureManager();

	// Create the Material manager
	mMaterialManager = new materialManager();
}

root::~root()
{
	delete mActiveRS;
	delete mRenderer;
	delete mTextureManager;
	delete mMaterialManager;
}
			
renderSystem* root::getRenderSystem()
{
	return mActiveRS;
}
			
renderer* root::getRenderer()
{
	assert(mRenderer != NULL);
	return mRenderer;
}

materialManager* root::getMaterialManager()
{
	assert(mMaterialManager != NULL);
	return mMaterialManager;
}

} // namespace k

