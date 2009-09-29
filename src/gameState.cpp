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

#include "gameState.h"
#include "logger.h"

namespace k {

template<> stateManager* k::singleton<stateManager>::singleton_instance = 0;

stateManager& stateManager::getSingleton()
{  
	kAssert(singleton_instance);
	return (*singleton_instance);  
}

stateManager::stateManager()
{
	mActiveState = NULL;
}

gameState* stateManager::findState(const std::string& name)
{
	std::map<std::string, gameState*>::const_iterator it = mStates.find(name);

	if (it != mStates.end())
		return it->second;
	else
		return NULL;
}

void stateManager::setState(const std::string& name)
{
	gameState* mState = findState(name);
	if (mState)
	{
		if (mActiveState)
			mActiveState->end();

		mActiveState = mState;
		mActiveState->start();
	}
	else
	{
		S_LOG_INFO("Game State " + name + " doesnt exist.");
	}
}

void stateManager::pushState(const std::string& name, gameState* newState)
{
	kAssert(newState);
	if (findState(name))
	{
		S_LOG_INFO("Game State already pushed to the manager.");
		return;
	}

	mStates[name] = newState;
}

void stateManager::popState(const std::string& name)
{
	std::map<std::string, gameState*>::iterator it = mStates.find(name);
	if (it != mStates.end())
	{
		// Call ending function on the state
		it->second->end();

		mStates.erase(it);
	}
}
		
void stateManager::frameStart()
{
	if (mActiveState)
		mActiveState->frameStart();
}

void stateManager::frameEnd()
{
	if (mActiveState)
		mActiveState->frameEnd();
}

} // namespace k

