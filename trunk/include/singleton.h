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

#ifndef _SINGLETON_H
#define _SINGLETON_H 

#include "prerequisites.h"

namespace k
{
	/**
	 * \brief Creates unique instances of other object types.
	 * The singleton class is responsible for letting classes only
	 * have one instance per-program-execution of them.
	 */
	template <typename T> class DLL_EXPORT singleton
	{
		protected:
			static T* singleton_instance;

    	public:
			/**
			 * Constructor.
			 */
			singleton()
			{
				assert (!singleton_instance);	
				singleton_instance = static_cast<T*>(this);
			}

			/**
			 * Destructor.
			 */
			~singleton()
        	{
				assert(singleton_instance);
				singleton_instance = 0;
			}

			/**
			 * Returns an instance of the singleton class.
			 */
        	static T& getSingleton()
			{		
				assert(singleton_instance);
				return (*singleton_instance); 
			}
	};
};
#endif
