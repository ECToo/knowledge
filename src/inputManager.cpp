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

void inputManager::setMouseEmulation(bool state)
{
	mEmulatingMouse = state;

	if (mEmulatingMouse && !mDevices[INPUT_MOUSE])
	{
		mInputPeripherals |= (1 << INPUT_MOUSE);
		try 
		{
			mDevices[INPUT_MOUSE] = new inputMouse();
		}

		catch (...)
		{
			S_LOG_INFO("Failed to create mouse device.");
			mInputPeripherals &= ~(1 << INPUT_MOUSE);
		}
	}
	else
	if (!mEmulatingMouse && mDevices[INPUT_MOUSE])
	{
		delete mDevices[INPUT_MOUSE];
		mDevices[INPUT_MOUSE] = NULL;
	}
}

bool inputManager::getQuitEvent() const
{
	return mReceivedQuitEvent;
}
			
void inputPeripheral::callEvent(eventHandlers type, unsigned int id)
{
	kAssert(type < HANDLER_MAX_HANDLERS);
	
	std::vector<inputFunctionPtr>::iterator it;
	for (it = mHandlers[type].begin(); it != mHandlers[type].end(); it++)
	{
		inputFunctionPtr funcPtr = (*it);
		inputEventHandler* handler = funcPtr.first;
		inputFunction function = funcPtr.second;
		(handler->*function)(id, this);
	}
}

}
