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

#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H

#include "prerequisites.h"
#include "singleton.h"
#include "keysyms.h"
#include "vector2.h"

namespace k
{
	class inputManager : public singleton<inputManager>
	{
		private:
			#ifdef __WII__
			/**
			 * Number of wiimotes connected.
			 * The input Manager will probe
			 * the wii if they are really connected and save here if they are.
			 *
			 * @see setupWiiMotes()
			 */
			bool mUseCube;
			unsigned short mConnnectedMotes;

			u32 mWiiMoteHeld[4];
			WPADData* mWiiData[4];
			#else
			/**
			 * A keyboard snapshot saved from each frame.
			 */
			Uint8* mSDLKbdSnapshot;

			/**
			 * A mouse snapshot saved from each frame.
			 */
			Uint8 mSDLMouseSnapshot;
			vector2 mLastMousePos;

			/**
			 * If quit event was triggered
			 */
			bool mQuitEvent;
			#endif

			/**
			 * If wiimote emulation (mouse movements for pointer)
			 * is enabled
			 */
			bool mEmulationEnabled;

		public:
			/**
			 * Controls the input System
			 * Keep in mind that this class is automatically 
			 * instantiated by the root
			 */
			inputManager();
			~inputManager();

			static inputManager& getSingleton();

			/**
			 * Initialize the Wiimotes/Gamecube Controllers.
			 * @cube If true, also initialize gamecube controllers
			 */
			void initWii(bool cube);

			/**
			 * Setup the wiimote data reception.
			 * WARNING: Be sure to call this function ONLY AFTER you setup
			 * the Video.
			 *
			 * @num The number of wiimotes to set
			 *
			 * @return The number of wiimotes REALLY connected, retrieved from the hardware.
			 */
			unsigned char setupWiiMotes(unsigned char num);

			/**
			 * Set the timeout of wiimotes (time it stays on idleing)
			 * @time Timeout in seconds.
			 */
			void setWiiMoteTimeout(unsigned short time);
			
			/**
			 * This function will convert each mouse coordinates on screen
			 * to wiimote coordinates (like if it was pointing on the screen).
			 * This is useful for testing wiimote pointing capabilities
			 * when you only have a mouse, and archive the same effect. Keep
			 * in mind that this only works on PC. When working with this function,
			 * if you query wiimote A it will respond for mouse left click and
			 * wiimote B will respond for mouse right click.
			 */
			void setWiiMoteEmulation(bool state);

			/**
			 * Returns wiimote position on the screen (pointer)
			 *
			 * @num The wiimote channel to query.
			 */
			vector2 getWiiMotePosition(unsigned char num);

			/**
			 * Tells if a key is down
			 *
			 * @id The key id, generally assigned on keysyms
			 * @return True if down, false if up
			 */
			bool getKbdKeyDown(unsigned int id);

			/**
			 * Tells if a button on wiimote is down
			 *
			 * @id The Button id
			 * @num Wiimote number
			 * @return True if down, false if up
			 */
			bool getWiiMoteDown(unsigned char num, unsigned int id);

			/**
			 * Return if the quit event was triggered
			 * Note that this is only valid on PC.
			 */
			bool getQuitEvent();

			/**
			 * Retrieve data from hardware. Must be called every frame
			 * before using the input.
			 */
			void feed();
	};
}

#endif
