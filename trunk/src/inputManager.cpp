/*
Copyright (c) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
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

inputManager::~inputManager()
{
	for (unsigned int i = 0; i < INPUT_MAX_PERIPHERALS; i++)
		delete mDevices[i];
}
			
const bool* inputKeyboard::getKeySnapshot(bool lastFrame) const
{
	if (lastFrame)
	{
		return mKeySnapshot;
	}
	else
	{
		return mLastKeySnapshot;
	}
}

bool inputKeyboard::isKeyDown(keyboardKeys key) const
{
	return mKeySnapshot[key];
}
	
const char* keyboardKeysStrings[KB_MAX_KEYS] =
{
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",

	"num_0",
	"num_1",
	"num_2",
	"num_3",
	"num_4",
	"num_5",
	"num_6",
	"num_7",
	"num_8",
	"num_9",

	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	"p",
	"q",
	"r",
	"s",
	"t",
	"u",
	"v",
	"w",
	"x",
	"y",
	"z",

	"f1",
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",
	"f9",
	"f10",
	"f11",
	"f12",

	"left_ctrl",
	"left_shift",
	"left_alt",

	"right_ctrl",
	"right_shift",
	"right_alt",

	"return",
	"backspace",

	"up",
	"down",
	"left",
	"right",

	"home",
	"end",
	"pgup",
	"pgdown",

	"caps_lock",
	"tab",

	"(",
	")",

	"[",
	"]",

	"escape",
	"/",
	"\\",
	"@",
	"_"	
};

/*
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

	SDL_Event ev, *events = &ev;
	while (SDL_PollEvent (events))
	{	
		switch (events->type)
		{
			case SDL_QUIT:
				mQuitEvent = true;
				break;
			case SDL_MOUSEMOTION:
				mLastMouseMotion.x = events->motion.xrel;
				mLastMouseMotion.y = events->motion.yrel;
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
*/

void inputManager::setMouseEmulation(bool state)
{
	// TODO
	mEmulatingMouse = state;
}

bool inputManager::getQuitEvent() const
{
	return mReceivedQuitEvent;
}
			
void inputPeripheral::callEvent(eventHandlers type, unsigned int id)
{
	std::map<eventHandlers, inputFunctionPtr>::iterator it = mHandlers.find(type);
	if (it != mHandlers.end())
	{
		inputFunctionPtr funcPtr = mHandlers[type];
		inputEventHandler* handler = funcPtr.first;
		inputFunction function = funcPtr.second;
		(handler->*function)(id, this);
	}
}

}
