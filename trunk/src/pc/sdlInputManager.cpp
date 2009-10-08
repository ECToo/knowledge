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

const int returnKeyFromSDL(int i)
{
	// 0 to 9
	if (i >= SDLK_0 && i <= SDLK_9)
	{
		return (KB_0 + (i - SDLK_0));
	}
	else
	// numpad 0 to 9
	if (i >= SDLK_KP0 && i <= SDLK_KP9)
	{
		return (KB_num_0 + (i - SDLK_KP0));
	}
	else
	// a to z
	if (i >= SDLK_a && i <= SDLK_z)
	{
		return (KB_a + (i - SDLK_a));
	}
	else
	// f1 to f12
	if (i >= SDLK_F1 && i <= SDLK_F12)
	{
		return (KB_f1 + (i - SDLK_F1));
	}
	else
	{
		switch (i)
		{
			default:
				return -1;
			case SDLK_LCTRL:
				return KB_left_ctrl;
			case SDLK_LALT:
				return KB_left_alt;
			case SDLK_LSHIFT:
				return KB_left_shift;
			case SDLK_RCTRL:
				return KB_right_ctrl;
			case SDLK_RALT:
				return KB_right_alt;
			case SDLK_RSHIFT:
				return KB_right_shift;
			case SDLK_RETURN:
				return KB_return;
			case SDLK_BACKSPACE:
				return KB_backspace;
			case SDLK_SPACE:
				return KB_space;
			case SDLK_UP:
				return KB_up;
			case SDLK_DOWN:
				return KB_down;
			case SDLK_LEFT:
				return KB_left;
			case SDLK_RIGHT:
				return KB_right;
			case SDLK_HOME:
				return KB_home;
			case SDLK_END:
				return KB_end;
			case SDLK_PAGEUP:
				return KB_pgup;
			case SDLK_PAGEDOWN:
				return KB_pgdown;
			case SDLK_CAPSLOCK:
				return KB_caps_lock;
			case SDLK_TAB:
				return KB_tab;
			case SDLK_LEFTPAREN:
				return KB_left_parenthesis;
			case SDLK_RIGHTPAREN:
				return KB_right_parenthesis;
			case SDLK_LEFTBRACKET:
				return KB_left_bracket;
			case SDLK_RIGHTBRACKET:
				return KB_right_bracket;
			case SDLK_ESCAPE:
				return KB_escape;
			case SDLK_SLASH:
				return KB_slash;
			case SDLK_BACKSLASH:
				return KB_backslash;
			case SDLK_AT:
				return KB_at;
			case SDLK_UNDERSCORE:
				return KB_underscore;
		};
	}
}

inputManager::inputManager()
{
	for (unsigned int i = 0; i < INPUT_MAX_PERIPHERALS; i++)
		mDevices[i] = NULL;

	mInputPeripherals = 0;
	mInputPeripherals |= (1 << INPUT_MOUSE);
	mInputPeripherals |= (1 << INPUT_KEYBOARD);

	try 
	{
		mDevices[INPUT_MOUSE] = new inputMouse();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to create mouse device.");
		mInputPeripherals &= ~(1 << INPUT_MOUSE);
	}

	try 
	{
		mDevices[INPUT_KEYBOARD] = new inputKeyboard();
	}

	catch (...)
	{
		S_LOG_INFO("Failed to create keyboard device.");
		mInputPeripherals &= ~(1 << INPUT_KEYBOARD);
	}
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
			case SDL_KEYDOWN:
				if (mDevices[INPUT_KEYBOARD])
				{
					// translate key from sdl to knowledge
					int key = returnKeyFromSDL(ev.key.keysym.sym);
					if (key != -1) mDevices[INPUT_KEYBOARD]->callEvent(HANDLER_DOWN, key);
				}
				break;
			case SDL_KEYUP:
				if (mDevices[INPUT_KEYBOARD])
				{
					// translate key from sdl to knowledge
					int key = returnKeyFromSDL(ev.key.keysym.sym);
					if (key != -1) mDevices[INPUT_KEYBOARD]->callEvent(HANDLER_UP, key);
				}
				break;
		}
	}

	// feed devices
	for (unsigned int i = 0; i < INPUT_MAX_PERIPHERALS; i++)
	{
		if (mDevices[i]) 
			mDevices[i]->feed();
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

inputKeyboard::inputKeyboard() : inputPeripheral()
{}

inputKeyboard::~inputKeyboard()
{}

void inputKeyboard::feed()
{
	memcpy(mLastKeySnapshot, mKeySnapshot, sizeof(bool) * KB_MAX_KEYS);
	memset(mKeySnapshot, 0, sizeof(bool) * KB_MAX_KEYS);

	// Translate keys from sdl to knowledge
	int numKeys = 0;
	Uint8* keyState = SDL_GetKeyState(&numKeys);

	for (int i = 0; i < numKeys; i++)
	{
		// 0 to 9
		if (i >= SDLK_0 && i <= SDLK_9)
		{
			mKeySnapshot[KB_0 + (i - SDLK_0)] = keyState[i];
		}
		else
		// numpad 0 to 9
		if (i >= SDLK_KP0 && i <= SDLK_KP9)
		{
			mKeySnapshot[KB_num_0 + (i - SDLK_KP0)] = keyState[i];
		}
		else
		// a to z
		if (i >= SDLK_a && i <= SDLK_z)
		{
			mKeySnapshot[KB_a + (i - SDLK_a)] = keyState[i];
		}
		else
		// f1 to f12
		if (i >= SDLK_F1 && i <= SDLK_F12)
		{
			mKeySnapshot[KB_f1 + (i - SDLK_F1)] = keyState[i];
		}
	}

	mKeySnapshot[KB_left_ctrl] = keyState[SDLK_LCTRL];
	mKeySnapshot[KB_left_alt] = keyState[SDLK_LALT];
	mKeySnapshot[KB_left_shift] = keyState[SDLK_LSHIFT];

	mKeySnapshot[KB_right_ctrl] = keyState[SDLK_RCTRL];
	mKeySnapshot[KB_right_alt] = keyState[SDLK_RALT];
	mKeySnapshot[KB_right_shift] = keyState[SDLK_RSHIFT];

	mKeySnapshot[KB_return] = keyState[SDLK_RETURN];
	mKeySnapshot[KB_backspace] = keyState[SDLK_BACKSPACE];
	mKeySnapshot[KB_space] = keyState[SDLK_SPACE];

	mKeySnapshot[KB_up] = keyState[SDLK_UP];
	mKeySnapshot[KB_down] = keyState[SDLK_DOWN];
	mKeySnapshot[KB_left] = keyState[SDLK_LEFT];
	mKeySnapshot[KB_right] = keyState[SDLK_RIGHT];

	mKeySnapshot[KB_home] = keyState[SDLK_HOME];
	mKeySnapshot[KB_end] = keyState[SDLK_END];
	mKeySnapshot[KB_pgup] = keyState[SDLK_PAGEUP];
	mKeySnapshot[KB_pgdown] = keyState[SDLK_PAGEDOWN];

	mKeySnapshot[KB_caps_lock] = keyState[SDLK_CAPSLOCK];
	mKeySnapshot[KB_tab] = keyState[SDLK_TAB];

	mKeySnapshot[KB_left_parenthesis] = keyState[SDLK_LEFTPAREN];
	mKeySnapshot[KB_right_parenthesis] = keyState[SDLK_RIGHTPAREN];

	mKeySnapshot[KB_left_bracket] = keyState[SDLK_LEFTBRACKET];
	mKeySnapshot[KB_right_bracket] = keyState[SDLK_RIGHTBRACKET];

	mKeySnapshot[KB_escape] = keyState[SDLK_ESCAPE];
	mKeySnapshot[KB_slash] = keyState[SDLK_SLASH];
	mKeySnapshot[KB_backslash] = keyState[SDLK_BACKSLASH];
	mKeySnapshot[KB_at] = keyState[SDLK_AT];
	mKeySnapshot[KB_underscore] = keyState[SDLK_UNDERSCORE];
}

} // namespace k

