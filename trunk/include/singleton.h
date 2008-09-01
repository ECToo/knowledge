#ifndef _SINGLETON_H
#define _SINGLETON_H 

#include "prerequisites.h"

namespace k
{
	template <typename T> class singleton
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
