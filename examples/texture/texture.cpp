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

// I hate globals but...
bool loadFinished;

void* loadResources(void* arg)
{
	k::resourceManager::getSingleton().loadGroup("common");
	k::resourceManager::getSingleton().loadGroup("texture");

	loadFinished = true;

	return NULL;
}

static inline void gear(GLfloat inner_radius, GLfloat outer_radius, GLint teeth)
{
	k::renderSystem* rs = k::root::getSingleton().getRenderSystem();

   float r0, r1, r2;
   float angle, da;

   r0 = inner_radius;
   r1 = outer_radius - 0.7 / 2.0;
   r2 = outer_radius + 0.7 / 2.0;

   da = 2.0 * M_PI / teeth / 4.0;

   /* draw front face */
	rs->startVertices(k::VERTEXMODE_TRI_STRIP);
	for (int i = 0; i <= teeth; i++) 
	{
		angle = i * 2.0 * M_PI / teeth;

		rs->color(k::vector3(1.0, 1.0, 1.0));
		rs->vertex(k::vector3(r0 * cos(angle), r0 * sin(angle), 0.5));
		rs->color(k::vector3(1.0, 1.0, 1.0));
		rs->vertex(k::vector3(r1 * cos(angle), r1 * sin(angle), 0.5));

      if (i < teeth) 
		{
			rs->color(k::vector3(1.0, 1.0, 1.0));
			rs->vertex(k::vector3(r0 * cos(angle), r0 * sin(angle), 0.5));
			rs->color(k::vector3(1.0, 1.0, 1.0));
			rs->vertex(k::vector3(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), 0.5));
      }
   }
	rs->endVertices();

	rs->startVertices(k::VERTEXMODE_QUAD);
	da = 2.0 * M_PI / teeth / 4.0;
   for (int i = 0; i < teeth; i++) 
	{
		angle = i * 2.0 * M_PI / teeth;

		rs->color(k::vector3(1.0, 1.0, 1.0));
		rs->vertex(k::vector3(r1 * cos(angle), r1 * sin(angle), 0.5));
		rs->color(k::vector3(1.0, 1.0, 1.0));
		rs->vertex(k::vector3(r2 * cos(angle + da), r2 * sin(angle + da), 0.5));
		rs->color(k::vector3(1.0, 1.0, 1.0));
		rs->vertex(k::vector3(r2 * cos(angle + 2 * da), r2 * sin(angle + 2 * da), 0.5));
		rs->color(k::vector3(1.0, 1.0, 1.0));
		rs->vertex(k::vector3(r1 * cos(angle + 3 * da), r1 * sin(angle + 3 * da), 0.5));
	}
	rs->endVertices();
}

void drawLoadingScreen()
{
	k::renderSystem* rs = k::root::getSingleton().getRenderSystem();
	k::textureManager* texMgr = &k::textureManager::getSingleton();
	k::inputManager* mInputManager = k::root::getSingleton().getInputManager();

	texMgr->allocateTextureData("loading.jpg");
	k::texture* loadTex = texMgr->getTexture("loading.jpg");

	assert(loadTex);

	// Load Resources
	kthread loadThread;
	k::createKThread(&loadThread, &loadResources, NULL);

	float angle = 0;
	bool running = true;
	while (running && !loadFinished)
	{
		mInputManager->feed();

		// User clicked on Close Window
		if (mInputManager->getQuitEvent() || mInputManager->getKbdKeyDown(K_KBD_ESCAPE))
			running = false;

		// Quit Application
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_HOME))
			running = false;

		rs->frameStart();
		rs->setCulling(k::CULLMODE_NONE);

		rs->setMatrixMode(k::MATRIXMODE_PROJECTION);
		rs->setOrthographic(0, rs->getScreenWidth(), rs->getScreenHeight(), 0, -1, 1);

		rs->setMatrixMode(k::MATRIXMODE_MODELVIEW);
		rs->identityMatrix();

		rs->setDepthMask(false);

		rs->bindTexture(loadTex->mId[0], 0);
		rs->startVertices(k::VERTEXMODE_QUAD);
			rs->texCoord(k::vector2(0.0f, 0.0f));
			rs->vertex(k::vector3(0, 0, -0.5));
			rs->texCoord(k::vector2(1.0f, 0.0f));
			rs->vertex(k::vector3(rs->getScreenWidth(), 0, -0.5));
			rs->texCoord(k::vector2(1.0f, 1.0f));
			rs->vertex(k::vector3(rs->getScreenWidth(), rs->getScreenHeight(), -0.5));
			rs->texCoord(k::vector2(0.0f, 1.0f));
			rs->vertex(k::vector3(0, rs->getScreenHeight(), -0.5));
		rs->endVertices();

		rs->setDepthMask(true);
		rs->unBindTexture(0);

		rs->setMatrixMode(k::MATRIXMODE_PROJECTION);
		rs->setOrthographic(-10.0f, 10.0f, -10.0f, 10.0f, -1, 1);

		// 
		rs->setMatrixMode(k::MATRIXMODE_MODELVIEW);
		rs->identityMatrix();
   	rs->translateScene(-5.0, -2.0, 0.0);
   	rs->rotateScene(angle, 0.0, 0.0, 1.0);
   	gear(1.0, 4.0, 20);

		rs->identityMatrix();
   	rs->translateScene(0.2, 1.6, 0.0);
   	rs->rotateScene(-2.0 * angle - 9.0, 0.0, 0.0, 1.0);
   	gear(0.5, 2.0, 10);

		angle += 0.05;
		rs->frameEnd();
	}
}

int main(int argc, char** argv)
{
	// Initialize knowledge
	k::root* appRoot = new k::root();
	k::renderSystem* mRenderSystem = appRoot->getRenderSystem();
	k::materialManager* mMaterialManager = appRoot->getMaterialManager();
	k::inputManager* mInputManager = appRoot->getInputManager();

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setWindowTitle("knowledge, the power of mind");

	#ifdef __WII__
	new k::resourceManager("/knowledge/resources.cfg");
	#else
	new k::resourceManager("../resources.cfg");
	#endif

	loadFinished = false;
	drawLoadingScreen();

	/**
	 * Setup the input Manager
	 */
	assert(mInputManager != NULL);
	mInputManager->initWii(false);
	mInputManager->setupWiiMotes(1);
	mInputManager->setWiiMoteTimeout(60);
	mInputManager->setWiiMoteEmulation(true);

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

	// Return to normal mode
	mRenderSystem->setMatrixMode(k::MATRIXMODE_PROJECTION);
	mRenderSystem->identityMatrix();
	mRenderSystem->setPerspective(90, 
			mRenderSystem->getScreenWidth()/mRenderSystem->getScreenHeight(), 
			0.1f, 5000.0f);

	bool running = true;
	while (running)
	{
		mInputManager->feed();

		// User clicked on Close Window
		if (mInputManager->getQuitEvent() || mInputManager->getKbdKeyDown(K_KBD_ESCAPE))
			running = false;

		// Quit Application
		if (mInputManager->getWiiMoteDown(0, WIIMOTE_BUTTON_HOME))
			running = false;

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

	delete appRoot;
	return 0;
}

