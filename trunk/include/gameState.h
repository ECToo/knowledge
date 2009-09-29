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
		/**
		 * Constructor
		 */
		stateManager();

		/**
		 * Destructor
		 */
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

