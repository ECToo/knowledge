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

#ifndef _COLOR_H_ 
#define _COLOR_H_ 

#include "prerequisites.h"

namespace k
{
	/**
	 * \brief Holds a 4 component (R,G,B,A) color.
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

