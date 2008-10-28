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
#include "rendersystem.h"

#ifdef __WII__
#include <wiiuse/wpad.h>
#endif

int main(int argc, char** argv)
{
	// Initialize knowledge
	k::root* appRoot = new k::root();
	k::renderSystem* mRenderSystem = appRoot->getRenderSystem();
	k::renderer* mRenderer = appRoot->getRenderer();

	#ifdef __WII__
	WPAD_Init();
	#endif

	assert(mRenderer != NULL);
	assert(mRenderSystem != NULL);

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setDepthTest(true);

	// Rotations
	vec_t rtri = 0;
	vec_t rquad = 0;

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
					switch (events.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running = false;
							break;
					}
					break;
			}
		}
		#else
		WPAD_ScanPads();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) 
		{
			running = false;
		}
		#endif

		// Clear screen, etc...
		mRenderSystem->frameStart();

		mRenderSystem->setMatrixMode(k::MATRIXMODE_MODELVIEW);
		mRenderSystem->identityMatrix();
		mRenderSystem->translateScene(-2.0f, 0.0f, -4.0f);
		mRenderSystem->rotateScene(rtri, 0.0f, 1.0f, 0.0f);

		// Draw Piramid
		mRenderSystem->startVertices(k::VERTEXMODE_TRIANGLES);
      	mRenderSystem->color(k::vector3(1.0f,  0.0f,  0.0f));
			mRenderSystem->vertex(k::vector3(0.0f,  1.0f,  0.0f));
      	mRenderSystem->color(k::vector3(0.0f,  1.0f,  0.0f));
			mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));
			mRenderSystem->color(k::vector3(0.0f,  0.0f,  1.0f)); 	      
			mRenderSystem->vertex(k::vector3(1.0f, -1.0f,  1.0f)); 

   	   mRenderSystem->color(k::vector3(1.0f,  0.0f,  0.0f));
			mRenderSystem->vertex(k::vector3(0.0f,  1.0f,  0.0f)); 	      
			mRenderSystem->color(k::vector3(0.0f,  0.0f,  1.0f)); 
			mRenderSystem->vertex(k::vector3(1.0f, -1.0f,  1.0f)); 
			mRenderSystem->color(k::vector3(0.0f,  1.0f,  0.0f)); 
			mRenderSystem->vertex(k::vector3(1.0f, -1.0f, -1.0f));

	      mRenderSystem->color(k::vector3(1.0f,  0.0f,  0.0f)); 
			mRenderSystem->vertex(k::vector3(0.0f,  1.0f,  0.0f)); 
			mRenderSystem->color(k::vector3(0.0f,  1.0f,  0.0f)); 
			mRenderSystem->vertex(k::vector3(1.0f, -1.0f, -1.0f)); 
			mRenderSystem->color(k::vector3(0.0f,  0.0f,  1.0f)); 
			mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));

	      mRenderSystem->color(k::vector3(1.0f,  0.0f,  0.0f)); 
			mRenderSystem->vertex(k::vector3(0.0f,  1.0f,  0.0f)); 
			mRenderSystem->color(k::vector3(0.0f,  0.0f,  1.0f )); 
			mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f)); 
			mRenderSystem->color(k::vector3(0.0f,  1.0f,  0.0f )); 
			mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f)); 
		mRenderSystem->endVertices();

		// Draw Cube
		mRenderSystem->setMatrixMode(k::MATRIXMODE_MODELVIEW);
		mRenderSystem->identityMatrix();
		mRenderSystem->translateScene(2.0f, 0.0f, -4.0f);
		mRenderSystem->rotateScene(rquad, 0.0f, 1.0f, 0.0f);

		mRenderSystem->startVertices(k::VERTEXMODE_QUAD);
      	mRenderSystem->color(k::vector3(0.0f,  1.0f,  0.0f)); 
	      mRenderSystem->vertex(k::vector3(1.0f,  1.0f, -1.0f)); 
      	mRenderSystem->color(k::vector3(0.0f,  1.0f,  0.0f)); 
   	   mRenderSystem->vertex(k::vector3(-1.0f,  1.0f, -1.0f)); 
      	mRenderSystem->color(k::vector3(0.0f,  1.0f,  0.0f)); 
	      mRenderSystem->vertex(k::vector3(-1.0f,  1.0f,  1.0f));
      	mRenderSystem->color(k::vector3(0.0f,  1.0f,  0.0f)); 
	      mRenderSystem->vertex(k::vector3(1.0f,  1.0f,  1.0f));

	      mRenderSystem->color(k::vector3(1.0f,  0.5f,  0.0f)); 
	      mRenderSystem->vertex(k::vector3(1.0f, -1.0f,  1.0f));
	      mRenderSystem->color(k::vector3(1.0f,  0.5f,  0.0f)); 
	      mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));
	      mRenderSystem->color(k::vector3(1.0f,  0.5f,  0.0f)); 
	      mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));
	      mRenderSystem->color(k::vector3(1.0f,  0.5f,  0.0f)); 
	      mRenderSystem->vertex(k::vector3(1.0f, -1.0f, -1.0f));

	      mRenderSystem->color(k::vector3(1.0f,  0.0f,  0.0f));
	      mRenderSystem->vertex(k::vector3(1.0f,  1.0f,  1.0f));
	      mRenderSystem->color(k::vector3(1.0f,  0.5f,  0.0f)); 
	      mRenderSystem->vertex(k::vector3(-1.0f,  1.0f,  1.0f));
	      mRenderSystem->color(k::vector3(1.0f,  0.5f,  0.0f)); 
	      mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));
	      mRenderSystem->color(k::vector3(1.0f,  0.5f,  0.0f)); 
	      mRenderSystem->vertex(k::vector3(1.0f, -1.0f,  1.0f));

   	   mRenderSystem->color(k::vector3(1.0f,  1.0f,  0.0f));
	      mRenderSystem->vertex(k::vector3(1.0f, -1.0f, -1.0f));
   	   mRenderSystem->color(k::vector3(1.0f,  1.0f,  0.0f));
	      mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));
   	   mRenderSystem->color(k::vector3(1.0f,  1.0f,  0.0f));
	      mRenderSystem->vertex(k::vector3(-1.0f,  1.0f, -1.0f));
   	   mRenderSystem->color(k::vector3(1.0f,  1.0f,  0.0f));
	      mRenderSystem->vertex(k::vector3(1.0f,  1.0f, -1.0f));
	
	      mRenderSystem->color(k::vector3(0.0f,  0.0f,  1.0f));
	      mRenderSystem->vertex(k::vector3(-1.0f,  1.0f,  1.0f));
	      mRenderSystem->color(k::vector3(0.0f,  0.0f,  1.0f));
	      mRenderSystem->vertex(k::vector3(-1.0f,  1.0f, -1.0f));
	      mRenderSystem->color(k::vector3(0.0f,  0.0f,  1.0f));
	      mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));
	      mRenderSystem->color(k::vector3(0.0f,  0.0f,  1.0f));
	      mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));

	      mRenderSystem->color(k::vector3(1.0f,  0.0f,  1.0f)); 
	      mRenderSystem->vertex(k::vector3(1.0f,  1.0f, -1.0f));
	      mRenderSystem->color(k::vector3(1.0f,  0.0f,  1.0f)); 
	      mRenderSystem->vertex(k::vector3(1.0f,  1.0f,  1.0f));
	      mRenderSystem->color(k::vector3(1.0f,  0.0f,  1.0f)); 
	      mRenderSystem->vertex(k::vector3(1.0f, -1.0f,  1.0f));
	      mRenderSystem->color(k::vector3(1.0f,  0.0f,  1.0f)); 
	      mRenderSystem->vertex(k::vector3(1.0f, -1.0f, -1.0f));
		mRenderSystem->endVertices();
		
		// Upload to video
		mRenderSystem->frameEnd();

		// Update Rotations
		rtri += 0.4f;
		rquad -= 0.35f;
	}

	// delete appRoot;
	return 0;
}

