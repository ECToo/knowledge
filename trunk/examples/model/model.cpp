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
#include "md5.h"
#include "root.h"
#include "renderer.h"
#include "rendersystem.h"
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
	k::guiManager* mGuiManager = appRoot->getGuiManager();

	#ifdef __WII__
	WPAD_Init();
	#endif

	assert(mRenderer != NULL);
	assert(mRenderSystem != NULL);

	// Doesnt matter on wii
	mRenderSystem->createWindow(800, 600);
	mRenderSystem->setDepthTest(true);

	// Common library
	#ifdef __WII__
	k::parsingFile* commonMaterialFile = new k::parsingFile("/knowledge/common.material");
	#else
	k::parsingFile* commonMaterialFile = new k::parsingFile("common.material");
	#endif

	assert(commonMaterialFile != NULL);
	mMaterialManager->parseMaterialScript(commonMaterialFile);

	// Parse material file
	#ifdef __WII__
	k::parsingFile* matFile = new k::parsingFile("/knowledge/goku/goku.material");
	#else
	k::parsingFile* matFile = new k::parsingFile("goku.material");
	#endif

	mMaterialManager->parseMaterialScript(matFile);

	// Create Model
	k::vector3 modelPosition;
	modelPosition.z = -100;

	#ifdef __WII__
	k::md5model* newModel = new k::md5model("/knowledge/goku/goku.md5mesh");
	#else
	k::md5model* newModel = new k::md5model("goku.md5mesh");
	#endif

	assert(newModel != NULL);
	mRenderer->push3D(newModel);

	#ifdef __WII__
	assert(mGuiManager != NULL);
	mGuiManager->setCursor("wiiCursor", k::vector2(32, 32));

	u32 resX, resY;
	WPAD_SetVRes(WPAD_CHAN_0, resX, resY);
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetIdleTimeout(60);
	#endif

	// Angles
	int rX = 0;
	int rY = 0;

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

		Uint8* keys = SDL_GetKeyState(NULL);
		if (keys[SDLK_LEFT])
			modelPosition.x++;
		else
		if (keys[SDLK_RIGHT])
			modelPosition.x--;

		if (keys[SDLK_UP])
			modelPosition.y++;
		else
		if (keys[SDLK_DOWN])
			modelPosition.y--;

		if (keys[SDLK_a])
			modelPosition.z += 0.3;
		else
		if (keys[SDLK_z])
			modelPosition.z -= 0.3;

		if (keys[SDLK_i])
			rY += 0.1;
		else
		if (keys[SDLK_k])
			rY -= 0.1;

		if (keys[SDLK_j])
			rX += 0.1;
		else
		if (keys[SDLK_l])
			rX -= 0.1;

		int mx, my;
		Uint8 mouse = SDL_GetRelativeMouseState(&mx, &my);
		if (mouse & SDL_BUTTON(SDL_BUTTON_LEFT))
		{
			rX += mx;
			rY += my;
		}
		else
		if (mouse & SDL_BUTTON(SDL_BUTTON_RIGHT))
		{
			modelPosition.x += mx;
			modelPosition.y -= my;
		}
		#else
		WPAD_ScanPads();
		u32 bHeld = WPAD_ButtonsHeld(0);

		if (bHeld & WPAD_BUTTON_HOME) 
		{
			running = false;
		}

		if (bHeld & WPAD_BUTTON_LEFT)
		{
			rX -= 1;
		}
		else
		if (bHeld & WPAD_BUTTON_RIGHT)
		{
			rX += 1;
		}

		if (bHeld & WPAD_BUTTON_UP)
		{
			rY += 1;
		}
		else
		if (bHeld & WPAD_BUTTON_DOWN)
		{
			rY -= 1;
		}

		if (bHeld & WPAD_BUTTON_MINUS)
		{
			modelPosition.z += 1.0f;
		}
		else
		if (bHeld & WPAD_BUTTON_PLUS)
		{
			modelPosition.z -= 1.0f;
		}

		WPADData* moteData = WPAD_Data(WPAD_CHAN_0);
		mGuiManager->setCursorPos(k::vector2(moteData->ir.ax, moteData->ir.ay));
		#endif

		k::quaternion yQuat = k::quaternion(rY, k::vector3(1, 0, 0));
		k::quaternion xQuat = k::quaternion(rX, k::vector3(0, 1, 0));
		k::quaternion modelQuat = xQuat * yQuat;

		newModel->setPosition(modelPosition);
		newModel->setOrientation(modelQuat);

		mRenderer->draw();
	}

	// delete appRoot;
	return 0;
}

