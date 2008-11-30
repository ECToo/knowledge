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
#include "resourceManager.h"
#include "logger.h"

#ifdef __WII__
#include <wiiuse/wpad.h>
#endif

int main(int argc, char** argv)
{
	// Initialize knowledge
	k::root* appRoot = new k::root();
	k::renderSystem* mRenderSystem = appRoot->getRenderSystem();
	k::renderer* mRenderer = appRoot->getRenderer();
	k::materialManager* mMaterialManager = appRoot->getMaterialManager();

	#ifdef __WII__
	WPAD_Init();
	#endif

	assert(mRenderer != NULL);
	assert(mRenderSystem != NULL);

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setWindowTitle("knowledge, the power of mind");
	mRenderSystem->setDepthTest(true);

	#ifdef __WII__
	new k::resourceManager("/knowledge/resources.cfg");
	#else
	new k::resourceManager("../resources.cfg");
	#endif

	k::resourceManager::getSingleton().loadGroup("common");
	k::resourceManager::getSingleton().loadGroup("texture");

	// Rotations
	vec_t rquad = 0;

	#ifdef __WII__
	k::parsingFile* tevFile = new k::parsingFile("/knowledge/tev.script");

	assert(tevFile != NULL);
	k::tevManager::getSingleton().parseTevScript(tevFile);
	#endif

	// Get material
	k::material* crateMaterial = mMaterialManager->getMaterial("crate");
	assert(crateMaterial);

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
						default:
							break;
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
		mRenderSystem->translateScene(0.0f, 0.0f, -4.0f);
		mRenderSystem->rotateScene(30, 1.0f, 0.0f, 0.0f);
		mRenderSystem->rotateScene(rquad, 0.0f, 1.0f, 0.0f);

		// Bind Texture
		crateMaterial->prepare();

		// Start Vertices
		mRenderSystem->startVertices(k::VERTEXMODE_QUAD);

		// Front Face
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f,  1.0f));

		// Back Face
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f, -1.0f));

		// Top Face
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f, -1.0f));

		// Bottom Face
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f,  1.0f));	
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));

		// Right face
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f,  1.0f));

		// Left Face
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f, -1.0f));

		// End
		mRenderSystem->endVertices();
		
		// Upload to video
		mRenderSystem->frameEnd();

		// Update Rotations
		#ifndef __WII__
		rquad += 0.08f;
		#else
		rquad += 0.5f;
		#endif
	}

	// delete appRoot;
	return 0;
}

