/*
    Copyright (C) 2008 Rômulo Fernandes Machado <romulo@castorgroup.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

