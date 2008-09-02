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
	k::textureManager* mTextureManager = &k::textureManager::getSingleton();

	#ifdef __WII__
	WPAD_Init();
	#endif

	assert(mRenderer != NULL);
	assert(mRenderSystem != NULL);

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setDepthTest(true);

	// Rotations
	vec_t rquad = 0;

	// Lets Create the texture
	k::texture* cubeTexture = mTextureManager->createTexture("/knowledge/textures/crate.png");

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
		mRenderSystem->translateScene(0.0f, 0.0f, -4.0f);
		mRenderSystem->rotateScene(rquad, 0.0f, 1.0f, 0.0f);

		// Bind Texture
		cubeTexture->draw();

		// Start Vertices
		mRenderSystem->startVertices(k::VERTEXMODE_QUAD);

		// Front Face
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f,  1.0f));

		// Back Face
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
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
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3( 1.0f, -1.0f,  1.0f));

		// Left Face
		mRenderSystem->texCoord(k::vector2(0.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f, -1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 0.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f, -1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(1.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f,  1.0f));
		mRenderSystem->texCoord(k::vector2(0.0f, 1.0f)); 
		mRenderSystem->vertex(k::vector3(-1.0f,  1.0f, -1.0f));

		// End
		mRenderSystem->endVertices();
		
		// Upload to video
		mRenderSystem->frameEnd();

		// Update Rotations
		rquad += 0.5f;
	}

	// delete appRoot;
	return 0;
}

