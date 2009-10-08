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

#include "wii/wiiInputManager.h"
#include "rendersystem.h"
#include "root.h"

namespace k {

inputManager::inputManager()
{
	WPAD_Init();
	PAD_Init();
	PAD_ScanPads();
	WPAD_ScanPads();

	for (unsigned int i = 0; i < INPUT_MAX_PERIPHERALS; i++)
		mDevices[i] = NULL;

	mInputPeripherals = 0;

	// Setup resolution
	u32 wResX, wResY;

	renderSystem* rs = root::getSingleton().getRenderSystem();
	wResX = rs->getScreenWidth();
	wResY = rs->getScreenHeight();

	// discover connected wiimotes
	for (int i = 0; i < 4; i++)
	{
		// Setup format and resolution
		WPAD_SetVRes(i, wResX, wResY);
		WPAD_SetDataFormat(i, WPAD_FMT_BTNS_ACC_IR);

		// Get Data
		WPADData* wiimoteData = WPAD_Data(i);
		if (!wiimoteData->data_present)
			break;

		try	 
		{
			mDevices[INPUT_WIIMOTE_1 + i] = new inputWiimote(i);
		}

		catch (...)
		{
			std::stringstream errormsg;
			errormsg << "Failed to create wiimote(" << i << ") device.";

			S_LOG_INFO(errormsg.str());
			mInputPeripherals &= ~(1 << INPUT_WIIMOTE_1 + i);
		}
	}
}

void inputManager::setPointerLock(bool lock)
{
	// Doesnt do anything on wii
}

void inputManager::feed()
{
	// Verify wiimotes
	for (int i = 0; i < 4; i++)
	{
		// Get Data
		WPADData* wiimoteData = WPAD_Data(i);
		if (!wiimoteData->data_present)
			break;

		if (mDevices[INPUT_WIIMOTE_1 + i])
			continue;

		try	 
		{
			mDevices[INPUT_WIIMOTE_1 + i] = new inputWiimote(i);
		}

		catch (...)
		{
			std::stringstream errormsg;
			errormsg << "Failed to create wiimote(" << i << ") device.";

			S_LOG_INFO(errormsg.str());
			mInputPeripherals &= ~(1 << INPUT_WIIMOTE_1 + i);
		}
	}

	// feed devices
	for (unsigned int i = 0; i < INPUT_MAX_PERIPHERALS; i++)
	{
		if (mDevices[i]) 
			mDevices[i]->feed();
	}
}

inputWiimote::inputWiimote(unsigned int chan)
{
	mChan = chan;
	feed();
}

inputWiimote::~inputWiimote()
{}

void inputWiimote::feed()
{
	WPADData* wiimoteData = WPAD_Data(mChan);

	// Wiimote is connected now =]
	if (wiimoteData->data_present)
	{
		if (!mIsConnected)
			callEvent(HANDLER_CONNECT, mChan);

		mIsConnected = true;
	}
	else
	{
		if (mIsConnected)
			callEvent(HANDLER_DISCONNECT, mChan);

		mIsConnected = false;
		return;
	}
	
	// IR Position
	mLastPosition = mPosition;
	mPosition = vector2(wiimoteData->ir.ax, wiimoteData->ir.ay);

	// Buttons
	u32 buttons = WPAD_ButtonsDown(mChan);
	mButtons = 0;

	if (buttons & WPAD_BUTTON_1)
		mButtons |= (1 << WIIMOTE_1);
	if (buttons & WPAD_BUTTON_2)
		mButtons |= (1 << WIIMOTE_2);
	if (buttons & WPAD_BUTTON_A)
		mButtons |= (1 << WIIMOTE_A);
	if (buttons & WPAD_BUTTON_B)
		mButtons |= (1 << WIIMOTE_B);
	if (buttons & WPAD_BUTTON_MINUS)
		mButtons |= (1 << WIIMOTE_MINUS);
	if (buttons & WPAD_BUTTON_PLUS)
		mButtons |= (1 << WIIMOTE_PLUS);
	if (buttons & WPAD_BUTTON_HOME)
		mButtons |= (1 << WIIMOTE_HOME);

	if (buttons & WPAD_BUTTON_LEFT)
		mButtons |= (1 << WIIMOTE_LEFT);
	if (buttons & WPAD_BUTTON_RIGHT)
		mButtons |= (1 << WIIMOTE_RIGHT);
	if (buttons & WPAD_BUTTON_UP)
		mButtons |= (1 << WIIMOTE_UP);
	if (buttons & WPAD_BUTTON_DOWN)
		mButtons |= (1 << WIIMOTE_DOWN);
}

inputMouse::inputMouse() : inputPeripheral() {}

inputMouse::~inputMouse()
{}

void inputMouse::feed() 
{
	inputPeripheral* wiimote = inputManager::getSingleton().getDevice(INPUT_WIIMOTE_1);
	if (!wiimote)
		return;

	mLastPosition = mPosition;
	mPosition = wiimote->getPosition();
	long mouseButtons = wiimote->getButtons();

	// Left
	if (mouseButtons & (1 << WIIMOTE_A))
	{
		mButtons |= (1 << MOUSE_LEFT);
		callEvent(HANDLER_DOWN, MOUSE_LEFT);
	}
	else
	{
		if (mButtons & (1 << MOUSE_LEFT))
		{
			// Press
			callEvent(HANDLER_PRESS, MOUSE_LEFT);
		}

		mButtons &= ~(1 << MOUSE_LEFT);
		callEvent(HANDLER_UP, MOUSE_LEFT);
	}

	// Right
	if (mouseButtons & (1 << WIIMOTE_B))
	{
		mButtons |= (1 << MOUSE_RIGHT);
		callEvent(HANDLER_DOWN, MOUSE_RIGHT);
	}
	else
	{
		if (mButtons & (1 << MOUSE_RIGHT))
			callEvent(HANDLER_PRESS, MOUSE_RIGHT);

		mButtons &= ~(1 << MOUSE_RIGHT);
		callEvent(HANDLER_UP, MOUSE_RIGHT);
	}
}

// TODO: Keyboard support
inputKeyboard::inputKeyboard() : inputPeripheral()
{}

inputKeyboard::~inputKeyboard()
{}

void inputKeyboard::feed()
{
}

} // namespace k

