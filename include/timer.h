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

#ifndef _TIMER_H_
#define _TIMER_H_

#include "prerequisites.h"

namespace k
{
	class timer
	{
		private:
			/**
			 * Keep reference of the start
			 * of the counting.
			 */
			struct timeval start;

		public:
			timer()
			{
				gettimeofday(&start, NULL);
			}

			void reset()
			{
				gettimeofday(&start, NULL);
			}

			int getMilliSeconds()
			{
				struct timeval end;
				gettimeofday(&end, NULL);

				return (end.tv_sec-start.tv_sec)*1000+(end.tv_usec-start.tv_usec)/1000;
			}
	};
}

#endif

