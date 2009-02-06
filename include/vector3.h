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

#ifndef _VECTOR3D_H_ 
#define _VECTOR3D_H_ 

#include "prerequisites.h"

namespace k
{
	/**
	 * vector3 Class
	 * Used to hold 3d coordinates
	 */
	class vector3 
	{
		public:
			vec_t x, y, z;

		public:
			/**
	 		 * Default Constructor
			 * Start as a blank vector(0,0)
	 		 */
			vector3()
			{ 
				x = y = z = 0;
			}

			vector3(const vector3& in)
			{
				*this = in;
			}
			
			/**
	 		 * Constructor
			 * @param xx The Vector X
			 * @param yy The Vector Y
			 * @param zz The Vector Z
	 		 */
			vector3(vec_t xx, vec_t yy, vec_t zz)
			{ 
				x = xx;
				y = yy;
				z = zz;
			}

			vector3 reflect(const vector3& normal)
			{
				vector3 out;
				vec_t dot = -2 * dotProduct(normal);

				out.x = x + dot * normal.x;
				out.y = y + dot * normal.y;
				out.z = z + dot * normal.z;
						
				return out;
			}

			inline vector3 operator + (const vector3& newVec) const
			{
				vector3 tempVec(newVec.x + x, newVec.y + y, newVec.z + z);
				return tempVec;
			}

			inline vector3& operator += (const vector3& newVec)
			{
				x += newVec.x;
				y += newVec.y;
				z += newVec.z;

				return *this;
			}
			
			inline vector3& operator = (const vector3& newVec)
			{
				x = newVec.x;
				y = newVec.y;
				z = newVec.z;

				return *this;				
			}
			
			inline vector3 operator - (const vector3& newVec) const
			{
				vector3 tempVec(x - newVec.x, 
						y - newVec.y,
						z - newVec.z);

				return tempVec;				
			}

			inline vector3& operator -= (const vector3& newVec)
			{
				x -= newVec.x;
				y -= newVec.y;
				z -= newVec.z;

				return *this;
			}

			inline vector3& operator *= (const vec_t newVec)
			{
				x *= newVec;
				y *= newVec;
				z *= newVec;

				return *this;
			}

			/**
			 * Multiply this vector by a rotation matrix
			 * @return Another vector with the multiplication stored
			 */
			inline vector3 operator * (const vec_t matrix[9]) const
			{
				vector3 tempVec
						(
						x * matrix[0] + y * matrix[3] + z * matrix[6],
						x * matrix[1] + y * matrix[4] + z * matrix[7],
						x * matrix[2] + y * matrix[5] + z * matrix[8]
						);
				
				return tempVec;				
			}
			
			/**
			 * Multiply this vector by a rotation matrix
			 * @return This vector multiplied by the matrix
			 */
			inline vector3& operator *= (const vec_t matrix[9])
			{
				x = x * matrix[0] + y * matrix[3] + z * matrix[6];
				y = x * matrix[1] + y * matrix[4] + z * matrix[7];
				z = x * matrix[2] + y * matrix[5] + z * matrix[8];
				
				return *this;				
			}
			
			inline vector3 operator * (const vec_t scalar) const
			{
				vector3 tempVec(x * scalar, 
						y * scalar,
						z * scalar);

				return tempVec;				
			}
			
			inline vector3 operator * (const vector3& newVec) const
			{
				vector3 tempVec(x * newVec.x, 
						y * newVec.y,
						z * newVec.z);

				return tempVec;				
			}
			
			inline vector3 operator / (const vec_t scalar) const
			{
				assert (scalar != 0.0);
				
				vector3 tempVec(x/scalar, y/scalar, z/scalar);
				return tempVec;				
			}
			
			inline vector3 operator / (const vector3& newVec) const
			{
				vector3 tempVec(x / newVec.x, 
						y / newVec.y,
						z / newVec.z);

				return tempVec;				
			}
			
			inline bool operator == (const vector3& newVec) const
			{
				return (
						x == newVec.x && 
						y == newVec.y &&
						z == newVec.z
						);
			}

			inline bool operator != (const vector3& newVec) const
			{
				return !(
						x == newVec.x && 
						y == newVec.y &&
						z == newVec.z
						);
			}

			inline vector3 negate() const
			{
				return vector3(-x, -y, -z);
			}

			inline vector3 negateItself()
			{
				x = -x;
				y = -y;
				z = -z;

				return *this;
			}
			
			inline const vec_t length() const
			{
				return sqrt(x*x + y*y + z*z);
			}
			
			/**
	 		 * Dot product between this vector and another one
	 		 */
			inline const vec_t dotProduct(const vector3& newVec) const
			{
				return ((x * newVec.x) + (y * newVec.y) + (z * newVec.z));
			}

			/**
	 		 * The cross product between this vector and another vector
	 		 */
			inline vector3 crossProduct(const vector3& newVec) const
			{
				vector3 tempVec
				(
					y * newVec.z - z * newVec.y,
					z * newVec.x - x * newVec.z,
					x * newVec.y - y * newVec.x
				);
				
				return tempVec;
			}

			inline vector3& cross(const vector3& newVec)
			{
				vector3 tempVec
				(
					y * newVec.z - z * newVec.y,
					z * newVec.x - x * newVec.z,
					x * newVec.y - y * newVec.x
				);

				x = tempVec.x;
				y = tempVec.y;
				z = tempVec.z;

				return *this;
			}
			
			/**
	 		 * Transform this vector into a unit vector
	 		 */
			inline void normalise()
			{
				vec_t sq = 1.0 / length();

				x *= sq;
				y *= sq;
				z *= sq;
			}
			
			/**
	 		 * Distance between this vector and another vector
	 		 */
			inline const vec_t distance(const vector3& newVec) const
			{
				return sqrt(pow((newVec.x-x), 2) + 
						pow((newVec.y-y), 2) + 
						pow((newVec.z-z), 2));
			}
			
			/**
	 		 * Print Vector information
	 		 */
			inline void cout() const
			{
				std::cout << "X: " << x << std::endl;
				std::cout << "Y: " << y << std::endl;
				std::cout << "Z: " << z << std::endl;
			}		

			static const vector3 zero;
			static const vector3 unit_x;
			static const vector3 unit_y;
			static const vector3 unit_z;
	};	
}

#endif
