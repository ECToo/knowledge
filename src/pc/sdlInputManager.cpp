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

#include "pc/sdlInputManager.h"

namespace k {

inputManager::inputManager()
{
	for (unsigned int i = 0; i < INPUT_MAX_PERIPHERALS; i++)
		mDevices[i] = NULL;

	mInputPeripherals = 0;
	mInputPeripherals |= (1 << INPUT_MOUSE);
	// mInputPeripherals |= (1 << INPUT_KEYBOARD);

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

inputManager::~inputManager()
{
	for (unsigned int i = 0; i < INPUT_MAX_PERIPHERALS; i++)
		delete mDevices[i];
}
			
void inputManager::setPointerLock(bool lock)
{
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
}

void inputManager::feed()
{
	// Reset Mouse Motion
	SDL_Event ev, *events = &ev;
	while (SDL_PollEvent (events))
	{	
		switch (events->type)
		{
			case SDL_QUIT:
				mReceivedQuitEvent = true;
				break;
		}
	}

	// feed devices
	for (unsigned int i = 0; i < INPUT_MAX_PERIPHERALS; i++)
	{
		if (mDevices[i]) 
		{
			mDevices[i]->feed();
		}
	}
}

inputMouse::inputMouse() : inputPeripheral() {}

inputMouse::~inputMouse()
{
	// If we have grabbed the mouse, release it.
	SDL_WM_GrabInput(SDL_GRAB_OFF);
	SDL_ShowCursor(1);
}

void inputMouse::feed() 
{
	int x, y;
	Uint8 mouseButtons = SDL_GetMouseState(&x, &y);

	mLastPosition = mPosition;
	mPosition = vector2(x, y);

	// Left
	if (mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT))
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

	// Middle 
	if (mouseButtons & SDL_BUTTON(SDL_BUTTON_MIDDLE))
	{
		mButtons |= (1 << MOUSE_WHEEL);
		callEvent(HANDLER_DOWN, MOUSE_WHEEL);
	}
	else
	{
		if (mButtons & (1 << MOUSE_WHEEL))
			callEvent(HANDLER_PRESS, MOUSE_WHEEL);

		mButtons &= ~(1 << MOUSE_WHEEL);
		callEvent(HANDLER_UP, MOUSE_WHEEL);
	}

	// Middle Up
	if (mouseButtons & SDL_BUTTON(SDL_BUTTON_WHEELUP))
	{
		mButtons |= (1 << MOUSE_WHEEL_UP);
		callEvent(HANDLER_DOWN, MOUSE_WHEEL_UP);
	}
	else
	{
		if (mButtons & (1 << MOUSE_WHEEL_UP))
			callEvent(HANDLER_PRESS, MOUSE_WHEEL_UP);

		mButtons &= ~(1 << MOUSE_WHEEL_UP);
		callEvent(HANDLER_UP, MOUSE_WHEEL_UP);
	}

	// Middle Down
	if (mouseButtons & SDL_BUTTON(SDL_BUTTON_WHEELDOWN))
	{
		mButtons |= (1 << MOUSE_WHEEL_DOWN);
		callEvent(HANDLER_DOWN, MOUSE_WHEEL_DOWN);
	}
	else
	{
		if (mButtons & (1 << MOUSE_WHEEL_DOWN))
			callEvent(HANDLER_PRESS, MOUSE_WHEEL_DOWN);

		mButtons &= ~(1 << MOUSE_WHEEL_DOWN);
		callEvent(HANDLER_UP, MOUSE_WHEEL_DOWN);
	}

	// Right
	if (mouseButtons & SDL_BUTTON(SDL_BUTTON_RIGHT))
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

} // namespace k

