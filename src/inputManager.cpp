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

#include "inputManager.h"
#include "logger.h"
#include "root.h"

namespace k {

template<> inputManager* singleton<inputManager>::singleton_instance = 0;

inputManager& inputManager::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

inputManager::inputManager()
{
	#ifdef __WII__
	mUseCube = false;
	mConnnectedMotes = 0;
	mWiiData[0] = mWiiData[1] = mWiiData[2] = mWiiData[3] = NULL;
	#else
	mSDLKbdSnapshot = NULL;
	memset(&mSDLMouseSnapshot, 0, sizeof(Uint8));
	mQuitEvent = false;
	#endif

	mEmulationEnabled = false;
}

inputManager::~inputManager()
{
	#ifndef __WII__
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(1);
	#endif
}

void inputManager::initWii(bool cube)
{
	#ifdef __WII__
	WPAD_Init();

	if (cube)
	{
		PAD_Init();
		mUseCube = true;
	}
	#endif
}
			
void inputManager::setPointerLock(bool lock)
{
	#ifndef __WII__
	if (lock)
	{
		SDL_WM_GrabInput(SDL_GRAB_ON);
		SDL_ShowCursor(0);
	}
	else
	{
		SDL_WM_GrabInput(SDL_GRAB_OFF);
		SDL_ShowCursor(1);
	}	
	#endif
}

unsigned char inputManager::setupWiiMotes(unsigned char num)
{
	#ifdef __WII__
	if (mUseCube)
		PAD_ScanPads();

	WPAD_ScanPads();
	
	// u32 wDev;
	u32 wResX, wResY;

	renderSystem* rs = root::getSingleton().getRenderSystem();
	wResX = rs->getScreenWidth();
	wResY = rs->getScreenHeight();

	for (unsigned int i = 0; i < num; i++)
	{
		WPAD_SetVRes(i, wResX, wResY);
		WPAD_SetDataFormat(i, WPAD_FMT_BTNS_ACC_IR);
	}

	// No Errors reported =]
	mConnnectedMotes = num;

	return mConnnectedMotes;
	#else
	return 0;
	#endif
}

void inputManager::setWiiMoteTimeout(unsigned short time)
{
	#ifdef __WII__
	WPAD_SetIdleTimeout(time);
	#endif
}
			
void inputManager::setWiiMoteEmulation(bool state)
{
	mEmulationEnabled = state;
}

vector2 inputManager::getWiiMotePosition(unsigned char num)
{
	#ifdef __WII__
	if (num > 3)
	{
		S_LOG_INFO("Failed to retrieve controller data. Wiimote index is too big");
		return vector2(0, 0);
	}
	return vector2(mWiiData[num]->ir.ax, mWiiData[num]->ir.ay);
	#else
	if (mEmulationEnabled)
	{
		return mLastMousePos;
	}
	#endif

	return vector2(0, 0);
}

void inputManager::feed()
{
	#ifdef __WII__

	// Reset Mouse Motion
	for (int i = 0; i < 4; i++)
		mLastMouseMotion[i].x = mLastMouseMotion[i].y = 0;

	if (mUseCube)
		PAD_ScanPads();

	WPAD_ScanPads();
	memset(mWiiMoteHeld, 0, sizeof(u32)*4);
	switch (mConnnectedMotes)
	{
		default:
		case 0:
			break;
		case 4:
			mWiiMoteHeld[3] = WPAD_ButtonsHeld(3);
			mWiiData[3] = WPAD_Data(WPAD_CHAN_3);
		case 3:
			mWiiMoteHeld[2] = WPAD_ButtonsHeld(2);
			mWiiData[2] = WPAD_Data(WPAD_CHAN_2);
		case 2:
			mWiiMoteHeld[1] = WPAD_ButtonsHeld(1);
			mWiiData[1] = WPAD_Data(WPAD_CHAN_1);
		case 1:
			mWiiMoteHeld[0] = WPAD_ButtonsHeld(0);
			mWiiData[0] = WPAD_Data(WPAD_CHAN_0);
			break;
	};

	#else

	// Reset Mouse Motion
	mLastMouseMotion.x = mLastMouseMotion.y = 0;

	SDL_Event events;
	while (SDL_PollEvent (&events))
	{	
		switch (events.type)
		{
			case SDL_QUIT:
				mQuitEvent = true;
				break;
			case SDL_MOUSEMOTION:
				mLastMouseMotion.x = events.motion.xrel;
				mLastMouseMotion.y = events.motion.yrel;
				break;
		}
	}

	int mx, my;
	mSDLMouseSnapshot = SDL_GetMouseState(&mx, &my);
	mSDLKbdSnapshot = SDL_GetKeyState(NULL);

	mLastMousePos.x = mx;
	mLastMousePos.y = my;
	#endif
}
			
vector2 inputManager::getWiiMoteMotion(unsigned char num)
{
	#ifdef __WII__
	return mLastMouseMotion[num];
	#else
	return mLastMouseMotion;
	#endif
}
			
bool inputManager::getQuitEvent()
{
	#ifdef __WII__
	return false;
	#else
	return mQuitEvent;
	#endif
}

bool inputManager::getKbdKeyDown(unsigned int id)
{
	#ifdef __WII__
	return false;
	#else
	return mSDLKbdSnapshot[id];
	#endif
}
			
bool inputManager::getWiiMoteDown(unsigned char num, unsigned int id)
{
	#ifdef __WII__
	if (num > 3)
	{
		S_LOG_INFO("Error: Invalid wiimote index!");
		return false;
	}
	else 
	{
		return (mWiiMoteHeld[num] & id);
	}
	#else
	if (mEmulationEnabled)
	{
		if (id == WIIMOTE_BUTTON_A)
			return (mSDLMouseSnapshot & SDL_BUTTON(SDL_BUTTON_LEFT));
		else
		if (id == WIIMOTE_BUTTON_B)
			return (mSDLMouseSnapshot & SDL_BUTTON(SDL_BUTTON_RIGHT));
	}
	#endif

	return false;
}

}
