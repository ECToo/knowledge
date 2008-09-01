#include "prerequisites.h"
#include "root.h"
#include "renderer.h"
#include "sticker.h"

int main(int argc, char** argv)
{
	// Initialize knowledge
	k::root* appRoot = new k::root();
	k::renderSystem* mRenderSystem = appRoot->getRenderSystem();
	k::renderer* mRenderer = appRoot->getRenderer();
	k::textureManager* mTextureManager = &k::textureManager::getSingleton();

	assert(mRenderer != NULL);
	assert(mRenderSystem != NULL);

	mRenderSystem->createWindow(512, 512);

	// Create the sticker with a texture =]
	k::sticker* newSticker = new k::sticker("logo");
	newSticker->setScale(k::vector2(512, 512));

	// Create the texture
	k::texture* newTexture = mTextureManager->createTexture("logo.png");
	newSticker->getMaterial()->pushTexture(newTexture);

	// Push the sticker to the graphics renderer
	mRenderer->push2D(newSticker);
	
	bool running = true;
	while (running)
	{
		#ifndef __WII__
		SDL_Event events;
		while (SDL_PollEvent (&events))
		{
			switch (events.type)
			{
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					{
						switch (events.key.keysym.sym)
						{
							case SDLK_ESCAPE:
								running = false;
								break;
						}
					}
					break;
			}
		}
		#else
		#endif

		mRenderer->draw();
	}

	delete appRoot;
	return 0;
}

