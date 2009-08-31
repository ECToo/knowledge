/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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

#ifndef _GAMESTATE_H_
#define _GAMESTATE_H_

#include "prerequisites.h"
#include "singleton.h"

namespace k 
{

/**
 * \brief A class to make game states. Shouldn't be used by itself, only derived.
 */
class DLL_EXPORT gameState 
{
	public:
		virtual ~gameState() {}

		/**
		 * This function gets called when the state gets active.
		 */
		virtual void start() = 0;

		/**
		 * This function gets called when the state gets inactive.
		 */
		virtual void end() = 0;

		/**
		 * Called at each frame start
		 */
		virtual void frameStart() = 0;

		/**
		 * Called at each frame end
		 */
		virtual void frameEnd() = 0;
};


/**
 * \brief The stateManager singleton.
 * This class is used to handle gameState 's.
 */
class DLL_EXPORT stateManager : public k::singleton<stateManager>
{
	protected:
		std::map<std::string, gameState*> mStates;
		gameState* mActiveState;

	public:
		stateManager();
		~stateManager();

		/**
		 * Returns an instance of the state manager class.
		 */
		static stateManager& getSingleton();

		/**
		 * Find a game state by name.
		 * @param name The state name.
		 */
		gameState* findState(const std::string& name);

		/**
		 * Set a state wich have been pushed to be active.
		 * @param name The state name.
		 */
		void setState(const std::string& name);

		/**
		 * Push an state to stateManager list of states.
		 * @param name The State name.
		 * @param ptr A pointer to gameState.
		 */
		void pushState(const std::string& name, gameState* ptr);
		
		/**
		 * Remove an state (by name) from stateManager list.
		 * @param name The name of the state to be removed.
		 */
		void popState(const std::string& name);

		/**
		 * This function should be called on your engine frame start, it will
		 * then call the active state frameStart() method.
		 */
		void frameStart();

		/**
		 * This function should be called on your engine frame end, it will
		 * then call the active state frameEnd() method.
		 */
		void frameEnd();
};

}

#endif

