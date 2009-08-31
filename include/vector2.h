/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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
	 * \brief Hold 2d coordinates
	 */
	class DLL_EXPORT vector2 
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

			/**
			 * Created a new vector as a copy of source vector.
			 *
			 * @param[in] in Source vector.
			 */
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

			/**
			 * Creates a vector from a formated string,
			 * where values are separated by white space.
			 */
			vector2(const std::string& str)
			{
				x = y = 0;

				if (str.length())
					sscanf(str.c_str(), "%f %f", &x, &y);
			}

			/**
			 * Reflect this vector over a normal.
			 */
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
			
			/**
			 * Multiply this vector by a scalar.
			 *
			 * @param[in] newVec Scalar number.
			 */
			inline vector2& operator *= (const vec_t& newVec)
			{
				x *= newVec;
				y *= newVec;

				return *this;				
			}

			/**
			 * Divide this vector by a scalar.
			 *
			 * @param[in] newVec Scalar number.
			 */
			inline vector2& operator /= (const vec_t& newVec)
			{
				x /= newVec;
				y /= newVec;

				return *this;				
			}
			
			/**
			 * Subtract another vector from this one.
			 * @param[in] newVec The Vector to be subtracted.
			 */
			inline vector2 operator - (const vector2& newVec) const
			{
				vector2 tempVec(x - newVec.x , y - newVec.y);
				return tempVec;				
			}

			/**
			 * Multiply this vector by a scalar.
			 */
			inline vector2 operator * (const vec_t scalar) const
			{
				vector2 tempVec(x * scalar, y * scalar);
				return tempVec;				
			}
			
			/**
			 * Multiply this vector by another one.
			 */
			inline vector2 operator * (const vector2 &newVec) const
			{
				vector2 tempVec(x * newVec.x, y * newVec.y);
				return tempVec;				
			}
			
			/**
			 * Divide this vector by a scalar.
			 */
			inline vector2 operator / (const vec_t scalar) const
			{
				assert (scalar != 0.0);
				
				vector2 tempVec(x/scalar, y/scalar);
				return tempVec;				
			}
			
			/**
			 * Divide this vector by another one. Each member will be divided by equivalent
			 * member on the second vector.
			 */
			inline vector2 operator / (const vector2 &newVec) const
			{
				vector2 tempVec(x / newVec.x, y / newVec.y);
				return tempVec;				
			}
			
			/**
			 * Return true if both vectors are identical
			 */
			inline bool operator == (const vector2 &newVec) const
			{
				return ((x == newVec.x) && (y == newVec.y));
			}

			/**
			 * Return true if both vectors are NOT identical.
			 */
			inline bool operator != (const vector2 &newVec) const
			{
				return !((x == newVec.x) && (y == newVec.y));
			}
			
			/**
			 * Returns a negate'd copy of this vector
			 */
			inline vector2 negate() const
			{
				return vector2(-x, -y);
			}

			/**
			 * Negate this vector.
			 */
			inline vector2 negateItself() 
			{
				x = -x;
				y = -y;

				return *this;
			}
			
			/**
			 * Return the length of this vector.
			 */
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
			inline void normalize() 
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
