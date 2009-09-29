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

#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H

#include "prerequisites.h"
#include "singleton.h"
#include "keysyms.h"
#include "logger.h"
#include "vector2.h"
#include "vector3.h"

namespace k
{
	/**
	 * List of keyboard keys. Letters
	 * and numbers match ascii, others match
	 * SDL valeus very closely.
	 */
	typedef enum
	{
		KB_0 = 48,
		KB_1,
		KB_2,
		KB_3,
		KB_4,
		KB_5,
		KB_6,
		KB_7,
		KB_8,
		KB_9,

		KB_num_0 = 256,
		KB_num_1,
		KB_num_2,
		KB_num_3,
		KB_num_4,
		KB_num_5,
		KB_num_6,
		KB_num_7,
		KB_num_8,
		KB_num_9,

		KB_a = 97,
		KB_b,
		KB_c,
		KB_d,
		KB_e,
		KB_f,
		KB_g,
		KB_h,
		KB_i,
		KB_j,
		KB_k,
		KB_l,
		KB_m,
		KB_n,
		KB_o,
		KB_p,
		KB_q,
		KB_r,
		KB_s,
		KB_t,
		KB_u,
		KB_v,
		KB_w,
		KB_x,
		KB_y,
		KB_z,

		KB_f1 = 282,
		KB_f2,
		KB_f3,
		KB_f4,
		KB_f5,
		KB_f6,
		KB_f7,
		KB_f8,
		KB_f9,
		KB_f10,
		KB_f11,
		KB_f12,

		KB_left_ctrl,
		KB_left_shift,
		KB_left_alt,

		KB_right_ctrl,
		KB_right_shift,
		KB_right_alt,

		KB_return,
		KB_backspace,

		KB_up,
		KB_down,
		KB_left,
		KB_right,

		KB_home,
		KB_end,
		KB_pgup,
		KB_pgdown,

		KB_caps_lock,
		KB_tab,

		KB_left_parenthesis,
		KB_right_parenthesis,

		KB_left_bracket,
		KB_right_bracket

	} keyboardKeys;

	/**
	 * This is the list of all peripherals available
	 * to the engine. Each peripheral active should set 
	 * its bit in the availablePeripherals variable in
	 * inputManager. You can check for a peripheral
	 * availability using:
	 *
	 * bool active = inputManager->getPeripherals() & (1 << INPUT_MOUSE);
	 * or
	 * bool active = inputManager->isPeripheralActive(INPUT_MOUSE);
	 * for example.
	 */
	typedef enum
	{
		INPUT_MOUSE = 0,
		INPUT_KEYBOARD,

		/**
		 * Wiimotes.
		 */
		INPUT_WIIMOTE_1,
		INPUT_WIIMOTE_2,
		INPUT_WIIMOTE_3,
		INPUT_WIIMOTE_4,

		/**
		 * Gamepads OR gamecube controllers, depends on the system.
		 */
		INPUT_GAMEPAD_1,
		INPUT_GAMEPAD_2,
		INPUT_GAMEPAD_3,
		INPUT_GAMEPAD_4,

		/**
		 * Classic controllers connected to wiimotes.
		 */
		INPUT_CLASSIC_1,
		INPUT_CLASSIC_2,
		INPUT_CLASSIC_3,
		INPUT_CLASSIC_4,

		INPUT_MAX_PERIPHERALS

	} inputPeripherals;

	/**
	 * Wiimote buttons bits.
	 */
	typedef enum
	{
		WIIMOTE_A = 0,
		WIIMOTE_B,
		WIIMOTE_1,
		WIIMOTE_2,
		WIIMOTE_MINUS,
		WIIMOTE_PLUS,
		WIIMOTE_HOME,
		WIIMOTE_UP,
		WIIMOTE_LEFT,
		WIIMOTE_RIGHT,
		WIIMOTE_DOWN,

		WIIMOTE_MAX_BUTTONS

	} wiimoteButtons;

	/**
	 * Mouse button bits.
	 */
	typedef enum
	{
		MOUSE_LEFT,
		MOUSE_RIGHT,
		MOUSE_WHEEL,
		MOUSE_WHEEL_UP,
		MOUSE_WHEEL_DOWN
	} mouseButtons;

	#define INPUT_MAX_AXIS 4

	/**
	 * Possible event handlers
	 */
	typedef enum
	{
		HANDLER_DOWN,
		HANDLER_UP,
		HANDLER_PRESS
	} eventHandlers;

	/**
	 * This class is a base for input event handlers.
	 * If you want to handle input events, make your class
	 * a derivative of this one and create the receiver
	 * methods. Check eventHandlers for the possible handlers.
	 * Keyboards will generate the keys and mouse/gamepads will generate
	 * the buttons as the event parameter.
	 */
	class DLL_EXPORT inputEventHandler
	{};

	class DLL_EXPORT inputPeripheral;
	class DLL_EXPORT inputManager;

	/**
	 * This is the event handler method definition.
	 * The function returns true if the event should be blocked (it doesnt propagate any further).
	 * In your method, the first parameter will be the button/key identifier and the second
	 * a pointer to the peripheral structure that generated the event.
	 */
	typedef DLL_EXPORT bool(inputEventHandler::*inputFunction)(unsigned int, inputPeripheral*);

	/**
	 * This is a pair representing the event handler. It contains a pointer to the
	 * handler and the pointer to the function.
	 */
	typedef DLL_EXPORT std::pair<inputEventHandler*, inputFunction> inputFunctionPtr;
	
	/**
	 * This is a base class for each input peripheral.
	 * Every connected peripheral should be represented
	 * by this class and put its information on it.
	 */
	class DLL_EXPORT inputPeripheral
	{
		protected:
			/**
			 * Hold buttons as bits. 
			 * From 32 to 64 buttons, depends on the system.
			 */
			long mButtons;

			/**
			 * Input position, in case of the mouse
			 * or wiimote IR.
			 */
			vector2 mPosition;

			/**
			 * Last input position.
			 */
			vector2 mLastPosition;

			/**
			 * Supports up to 4 3D axis per device.
			 */
			vector3 mAxis[INPUT_MAX_AXIS];

			/**
			 * Is device connected?
			 */
			bool mIsConnected;

			/**
			 * Event handlers.
			 */
			std::map<eventHandlers, inputFunctionPtr> mHandlers;
		
			// Make input manager friend of this class
			// in the chance of input manager wants to change stuff
			// here.
			friend class inputManager;

		public:

			/**
			 * Base constructor, initialize all 
			 * peripheral data.
			 */
			inputPeripheral()
			{
				mButtons = 0;
				mPosition = mLastPosition = vector2(0,0);
				mIsConnected = false;

				for (unsigned int i = 0; i < INPUT_MAX_AXIS; i++)
					mAxis[i] = vector3::zero;
			}

			/**
			 * Get peripheral buttons. Each button is a bit
			 * set in the variable. To know what bits are
			 * each button, refer to the peripheral documentation.
			 */
			long getButtons() const
			{
				return mButtons;
			}

			/**
			 * Returns true if a peripheral button is down.
			 */
			bool isButtonDown(mouseButtons button) const
			{
				return (mButtons & (1 << button));
			}

			/**
			 * Get an axis by its index. If the index is greater
			 * than INPUT_MAX_AXIS the engine will throw an assertion and
			 * quit.
			 */
			const vector3& getAxis(const unsigned int i) const
			{
				kAssert(i < INPUT_MAX_AXIS);
				return mAxis[i];
			}

			/**
			 * Returns if the peripheral is connected or not.
			 */
			bool isConnected() const
			{
				return mIsConnected;
			}

			/**
			 * Returns the peripheral position. In case of mouse
			 * returns the cursor position, in case of wiiimotes
			 * returns the IR position.
			 */
			const vector2& getPosition() const
			{
				return mPosition;
			}

			/**
			 * Returns the peripheral delta position.
			 */
			vector2 getDeltaPosition() const
			{
				vector2 result = mPosition - mLastPosition;
				return result;
			}

			/**
			 * Push an event handler for a certain event of
			 * this peripheral.
			 *
			 * @param type The event handler type.
			 * @param handler The handler function and class instance.
			 */
			void pushEvent(eventHandlers type, inputFunctionPtr function)
			{
				kAssert(function.first);
				mHandlers[type] = function;
			}

			/**
			 * Remove an event hanlder for a certain event type.
			 * @param type The event handler type. If no event is assigned, nothing happens.
			 */
			void popEvent(eventHandlers type)
			{
				std::map<eventHandlers, inputFunctionPtr>::iterator it = mHandlers.find(type);
				if (it != mHandlers.end())
					mHandlers.erase(it);
			}

			/**
			 * Call the event related to the event type.
			 * @param type The event handler type, @see eventHandlers
			 * @param id The event identifier, like the button pressed.
			 */
			void callEvent(eventHandlers type, unsigned int id);

			/**
			 * Ask the peripheral to feed the data fields.
			 * Called by inputManager at each feed()
			 */
			virtual void feed() = 0;
	};

	/**
	 * Input Manager, reponsible for all kind of attached peripherals.
	 * Each platform implements its own version of the inputManager, making
	 * it a totally dynamic class.
	 */
	class DLL_EXPORT inputManager : public singleton<inputManager>
	{
		protected:
			/**
			 * List of active peripherals, each system
			 * should fill this variable with its flags.
			 */
			long mInputPeripherals;

			/**
			 * Array of peripherals, if a peripheral is not
			 * present, its place on array should be set to NULL.
			 */
			inputPeripheral* mDevices[INPUT_MAX_PERIPHERALS];

			/**
			 * Holds if the inputManager received the quit event.
			 */
			bool mReceivedQuitEvent;

			/**
			 * Are we emulating mouse behavior?
			 */
			bool mEmulatingMouse;

		public:
			/**
			 * Controls the input System
			 * Keep in mind that this class is automatically 
			 * instantiated by the root
			 */
			inputManager();
			
			/**
			 * Destructor
			 */
			~inputManager();

			/**
			 * Return the variable holding the
			 * peripherals bits.
			 * You can check for a peripheral using
			 * masks, like getPeripherals() & (1 << INPUT_MOUSE)
			 */
			long getPeripherals() const
			{
				return mInputPeripherals;
			}

			/**
			 * Returns if a peripheral is active.
			 */
			bool isPeripheralActive(inputPeripherals per) const
			{
				return (mInputPeripherals & (1 << per));
			}

			/**
			 * Return a pointer to a peripheral.
			 * @param per The peripheral identifier, see @inputPeripherals
			 */
			inputPeripheral* getDevice(inputPeripherals per)
			{
				return mDevices[per];
			}

			/**
			 * Return the inputManager singleton instance.
			 */
			static inputManager& getSingleton();

			/**
			 * If the pointer should be confined in the
			 * game area, set this to true.
			 */
			void setPointerLock(bool lock);

			/**
			 * This function will make the mouse (even if not present, like in wii)
			 * to behave exactly like wiimote 1 (if present, using IR coordinates).
			 * Mouse left click will correspond to wiimote A and right click
			 * to wiimote B.
			 */
			void setMouseEmulation(bool state);

			/**
			 * Return if the quit event was triggered
			 * Note that this is only valid on PC.
			 */
			bool getQuitEvent() const;

			/**
			 * Retrieve data from hardware. Must be called every frame
			 * before using the input.
			 */
			void feed();
	};
}

#endif

