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

#ifndef _SINGLETON_H
#define _SINGLETON_H 

#include "prerequisites.h"

namespace k
{
	template <typename T> class DLL_EXPORT singleton
	{
		protected:
			static T* singleton_instance;

    	public:
			singleton()
			{
				assert (!singleton_instance);	
				singleton_instance = static_cast<T*>(this);
			}

			~singleton()
        	{
				assert(singleton_instance);
				singleton_instance = 0;
			}

        	static T& getSingleton()
			{		
				assert(singleton_instance);
				return (*singleton_instance); 
			}
	};
};
#endif
