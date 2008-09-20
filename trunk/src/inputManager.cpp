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
	assert(singleton_instance);
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
	#endif

	mEmulationEnabled = false;
}

inputManager::~inputManager()
{
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

unsigned char inputManager::setupWiiMotes(unsigned char num)
{
	#ifdef __WII__
	if (mUseCube)
		PAD_ScanPads();

	WPAD_ScanPads();
	
	u32 wDev;
	u32 wResX, wResY;

	mRenderSystem* rs = root::getSingleton().getRenderSystem();
	wResX = rs->getScreenWidth();
	wResY = rs->getScreenHeight();

	if (WPAD_Probe(WPAD_CHAN0, &wDev) != WPAD_ERR_NONE)
	{
		mConnnectedMotes = 0;
		S_LOG_INFO("Warning! No connected wiimotes detected!");

		return mConnnectedMotes;
	}

	WPAD_SetVRes(WPAD_CHAN_0, wResX, wResY);
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

	if (num > 1)
	{
		if (WPAD_Probe(WPAD_CHAN1, &wDev) != WPAD_ERR_NONE)
		{
			mConnnectedMotes = 1;
			S_LOG_INFO("Warning! Only one wiimote detected!");

			return mConnnectedMotes;
		}

		WPAD_SetVRes(WPAD_CHAN_1, wResX, wResY);
		WPAD_SetDataFormat(WPAD_CHAN_1, WPAD_FMT_BTNS_ACC_IR);

		if (num > 2)
		{
			if (WPAD_Probe(WPAD_CHAN2, &wDev) != WPAD_ERR_NONE)
			{
				mConnnectedMotes = 2;
				S_LOG_INFO("Warning! Only two wiimotes detected!");

				return mConnnectedMotes;
			}

			WPAD_SetVRes(WPAD_CHAN_2, wResX, wResY);
			WPAD_SetDataFormat(WPAD_CHAN_2, WPAD_FMT_BTNS_ACC_IR);

			if (num > 3)
			{
				if (WPAD_Probe(WPAD_CHAN3, &wDev) != WPAD_ERR_NONE)
				{
					mConnnectedMotes = 2;
					S_LOG_INFO("Warning! Only two wiimotes detected!");

					return mConnnectedMotes;
				}

				WPAD_SetVRes(WPAD_CHAN_3, wResX, wResY);
				WPAD_SetDataFormat(WPAD_CHAN_3, WPAD_FMT_BTNS_ACC_IR);
			}
			// num > 3
		}
		// num > 2
	}
	// num > 1

	// No Errors reported =]
	mConnnectedMotes = num;

	return mConnnectedMotes;
	#else
	return 0;
	#endif
}

void inputManager::setWiiMoteTimetout(unsigned short time)
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
	switch (num)
	{
		default:
		case 0:
			break;
		case 1:
			return vector2(mWiiData[0]->ir.ax, mWiiData[0]->ir.ay);
		case 2:
			return vector2(mWiiData[1]->ir.ax, mWiiData[1]->ir.ay);
		case 3:
			return vector2(mWiiData[2]->ir.ax, mWiiData[2]->ir.ay);
		case 4:
			return vector2(mWiiData[3]->ir.ax, mWiiData[3]->ir.ay);
	}
	#else
	if (mEmulationEnabled)
	{
		return mLastMousePos;
	}
	#endif
}

void inputManager::feed()
{
	#ifdef __WII__
	switch (mConnnectedMotes)
	{
		default:
		case 0:
			if (!mUseCube)
			{
				S_LOG_INFO("Warning: No wiimotes connected!");
			}
			break;

		case 4:
			mWiiData[3] = WPAD_Data(WPAD_CHAN_3);
		case 3:
			mWiiData[2] = WPAD_Data(WPAD_CHAN_2);
		case 2:
			mWiiData[1] = WPAD_Data(WPAD_CHAN_1);
		case 1:
			mWiiData[0] = WPAD_Data(WPAD_CHAN_0);
			break;
	};
	#else
	SDL_PumpEvents();

	unsigned int mx, my;
	mSDLMouseSnapshot = SDL_GetRelativeMouseState(&mx, &my);
	mSDLKbdSnapshot = SDL_GetKeyState(NULL);

	mLastMousePos.x = mx;
	mLastMousePos.y = my;
	#endif
}

}
