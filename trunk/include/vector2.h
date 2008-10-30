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

#ifndef _VECTOR2D_H_ 
#define _VECTOR2D_H_ 

#include "prerequisites.h"

namespace k
{
	/**
	 * Vector2 Class
	 * Used to hold 2d coordinates
	 */
	class vector2 
	{
		public:
			vec_t x, y;

		public:
			/**
	 		 * Default Constructor
			 * Start as a blank vector(0,0)
	 		 */
			vector2()
			{ 
				x = y = 0;
			}
			
			/**
	 		 * Constructor
			 * @param xx The Vector X
			 * @param yy The Vector Y
	 		 */
			vector2(const vec_t xx, const vec_t yy)
			{ 
				x = xx;
				y = yy;
			}

			vector2 operator+ (const vector2 &newVec)
			{
				vector2 tempVec(newVec.x + x, newVec.y + y);
				return tempVec;
			}
			
			/**
	 		 * Assign the vector to another one with same dimension
	 		 */
			vector2& operator = (const vector2 &newVec)
			{
				x = newVec.x;
				y = newVec.y;

				return *this;				
			}
			
			vector2 operator - (const vector2 &newVec)
			{
				vector2 tempVec(x - newVec.x , y - newVec.y);
				return tempVec;				
			}

			vector2 operator * (const vec_t scalar)
			{
				vector2 tempVec(x * scalar, y * scalar);
				return tempVec;				
			}
			
			vector2 operator * (const vector2 &newVec)
			{
				vector2 tempVec(x * newVec.x, y * newVec.y);
				return tempVec;				
			}
			
			vector2 operator / (const vec_t scalar)
			{
				assert (scalar != 0.0);
				
				vector2 tempVec(x/scalar, y/scalar);
				return tempVec;				
			}
			
			vector2 operator / (const vector2 &newVec)
			{
				vector2 tempVec(x / newVec.x, y / newVec.y);
				return tempVec;				
			}
			
			bool operator == (const vector2 &newVec)
			{
				return ((x == newVec.x) && (y == newVec.y));
			}

			bool operator != (const vector2 &newVec)
			{
				return !((x == newVec.x) && (y == newVec.y));
			}
			
			/**
	 		 * Get the opposite of this vector
			 * If vector is (1,1) it becomes (-1,-1)
	 		 */
			void negate()
			{
				x = -x;
				y = -y;
			}
			
			const vec_t length()
			{
				return sqrt((x*x)+(y*y));
			}
			
			/**
	 		 * Dot product between this vector and another one
	 		 */
			const vec_t dotProduct(const vector2 &newVec)
			{
				return ((x * newVec.x) + (y * newVec.y));
			}
			
			/**
	 		 * Transform this vector into a unit vector
	 		 */
			void normalise()
			{
				vec_t sq = length();

				x *= 1.0 / sq;
				y *= 1.0 / sq;
			}
			
			/**
	 		 * Distance between this vector and another vector
	 		 */
			inline vec_t distance(const vector2& newVec)
			{
				return sqrt(pow((newVec.x-x), 2) + pow((newVec.y-y), 2));
			}
			
			/**
	 		 * Print Vector information
	 		 */
			void cout()
			{
				std::cout << "X: " << x << std::endl;
				std::cout << "Y: " << y << std::endl;
			}		
	};	
}

#endif
