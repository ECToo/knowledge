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
			// Allow us to access like packed data.
			union
			{
				struct 
				{
					vec_t x, y;
				};
				vec_t vec[2];
			};

		public:
			/**
	 		 * Default Constructor
			 * Start as a blank vector(0,0)
	 		 */
			vector2()
			{ 
				x = y = 0;
			}

			vector2(const vector2& in)
			{
				*this = in;
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

			vector2 reflect(const vector2& normal)
			{
				vector2 out;
				vec_t dot = -2 * dotProduct(normal);

				out.x = x + dot * normal.x;
				out.y = y + dot * normal.y;
						
				return out;
			}

			inline vector2 operator+ (const vector2 &newVec) const
			{
				vector2 tempVec(newVec.x + x, newVec.y + y);
				return tempVec;
			}
			
			/**
	 		 * Assign the vector to another one with same dimension
	 		 */
			inline vector2& operator = (const vector2 &newVec)
			{
				x = newVec.x;
				y = newVec.y;

				return *this;				
			}
			
			inline vector2 operator - (const vector2 &newVec) const
			{
				vector2 tempVec(x - newVec.x , y - newVec.y);
				return tempVec;				
			}

			inline vector2 operator * (const vec_t scalar) const
			{
				vector2 tempVec(x * scalar, y * scalar);
				return tempVec;				
			}
			
			inline vector2 operator * (const vector2 &newVec) const
			{
				vector2 tempVec(x * newVec.x, y * newVec.y);
				return tempVec;				
			}
			
			inline vector2 operator / (const vec_t scalar) const
			{
				assert (scalar != 0.0);
				
				vector2 tempVec(x/scalar, y/scalar);
				return tempVec;				
			}
			
			inline vector2 operator / (const vector2 &newVec) const
			{
				vector2 tempVec(x / newVec.x, y / newVec.y);
				return tempVec;				
			}
			
			inline bool operator == (const vector2 &newVec) const
			{
				return ((x == newVec.x) && (y == newVec.y));
			}

			inline bool operator != (const vector2 &newVec) const
			{
				return !((x == newVec.x) && (y == newVec.y));
			}
			
			inline vector2 negate() const
			{
				return vector2(-x, -y);
			}

			inline vector2 negateItself() 
			{
				x = -x;
				y = -y;

				return *this;
			}
			
			inline const vec_t length() const
			{
				return sqrt((x*x)+(y*y));
			}
			
			/**
	 		 * Dot product between this vector and another one
	 		 */
			inline const vec_t dotProduct(const vector2 &newVec) const
			{
				return ((x * newVec.x) + (y * newVec.y));
			}
			
			/**
	 		 * Transform this vector into a unit vector
	 		 */
			inline void normalise() 
			{
				vec_t sq = length();

				x *= 1.0 / sq;
				y *= 1.0 / sq;
			}
			
			/**
	 		 * Distance between this vector and another vector
	 		 */
			inline vec_t distance(const vector2& newVec) const
			{
				return sqrt(pow((newVec.x-x), 2) + pow((newVec.y-y), 2));
			}
			
			/**
	 		 * Print Vector information
	 		 */
			inline void cout() const
			{
				printf("X: %f - Y: %f\n", x, y);
			}		
	};	
}

#endif
