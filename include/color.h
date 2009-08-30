/*
    Copyright (C) 2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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

#ifndef _COLOR_H_ 
#define _COLOR_H_ 

#include "prerequisites.h"

namespace k
{
	/**
	 * Holds a 4 component (R,G,B,A) color.
	 */
	class DLL_EXPORT color
	{
		public:
			// Allow us to access like packed data.
			union
			{
				struct 
				{
					vec_t r, g, b, a;
				};
				vec_t c[4];
			};

			/**
			 * Create a white color.
			 */
			color()
			{
				r = g = b = a = 1.0f;
			}

			/**
			 * Create color from hex string 			 
			 * @param name The color in hex (like HTML colors - MUST be 6 or 8 characters length)
			 */
			color(const std::string& name)
			{
				if (name.length() == 6)
				{
					r = (strtol(name.substr(0, 2).c_str(), NULL, 16)) / 255.0f;
					g = (strtol(name.substr(2, 2).c_str(), NULL, 16)) / 255.0f;
					b = (strtol(name.substr(4, 2).c_str(), NULL, 16)) / 255.0f;
					a = 1.0f;
				}
				else
				if (name.length() == 8)
				{
					r = (strtol(name.substr(0, 2).c_str(), NULL, 16)) / 255.0f;
					g = (strtol(name.substr(2, 2).c_str(), NULL, 16)) / 255.0f;
					b = (strtol(name.substr(4, 2).c_str(), NULL, 16)) / 255.0f;
					a = (strtol(name.substr(6, 2).c_str(), NULL, 16)) / 255.0f;
				}
			}

			/**
			 * Create color from separated components.
			 * @param rc Red Component
			 * @param gc Green Component
			 * @param bc Blue Component
			 * @param ac Alpha Component (defaults to 1.0f)
			 */
			color(float rc, float gc, float bc, float ac = 1.0f)
			{
				r = rc;
				g = gc;
				b = bc;
				a = ac;
			}

			/**
			 * Copy a color over this one.
			 * @param c The color to copy from.
			 */
			inline color& operator = (const color& c)
			{
				r = c.r;
				g = c.g;
				b = c.b;
				a = c.a;

				return *this;
			}

			/**
			 * Sum a color whit this one.
			 * @param c The color to sum.
			 */
			inline color& operator += (const color& c)
			{
				r += c.r;
				g += c.g;
				b += c.b;
				a += c.a;

				return *this;
			}

			/**
	 		 * Print Vector information.
	 		 */
			inline void cout() const
			{
				printf("R: %f - G: %f - B: %f - A: %f\n", r, g, b, a);
			}		
	};
}

#endif

