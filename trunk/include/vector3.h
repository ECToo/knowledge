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

#ifndef _VECTOR3D_H_ 
#define _VECTOR3D_H_ 

#include "prerequisites.h"

namespace k
{
	/**
	 * vector3 Class
	 * Used to hold 3d coordinates
	 */
	class DLL_EXPORT vector3 
	{
		public:
			// Allow us to access like packed data.
			union
			{
				#ifdef __HAVE_SSE3__
				struct 
				{
					vec_t x, y, z, w;
				};
				vec_t vec[4];
				#else
				struct 
				{
					vec_t x, y, z;
				};
				vec_t vec[3];
				#endif
			};

			/**
	 		 * Default Constructor
			 * Start as a blank vector(0,0)
	 		 */
			vector3()
			{ 
				x = y = z = 0;

				#ifdef __HAVE_SSE3__
				w = 0;
				#endif
			}

			vector3(const vector3& in)
			{
				x = in.x;
				y = in.y;
				z = in.z;

				#ifdef __HAVE_SSE3__
				w = 0;
				#endif
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

				#ifdef __HAVE_SSE3__
				w = 0;
				#endif
			}

			/**
			 * Creates a vector from a formated string,
			 * where values are separated by white space.
			 */
			vector3(const std::string& str)
			{
				x = y = z = 0;

				#ifdef __HAVE_SSE3__
				w = 0;
				#endif

				if (str.length())
					sscanf(str.c_str(), "%f %f %f", &x, &y, &z);
			}

			vector3 reflect(const vector3& normal)
			{
				vec_t dot = -2 * dotProduct(normal);

				return vector3(
						x + dot * normal.x,
						y + dot * normal.y,
						z + dot * normal.z);
			}

			inline vector3 operator + (const vector3& newVec) const
			{
				return vector3(newVec.x + x, newVec.y + y, newVec.z + z);
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
				return vector3(x * scalar, y * scalar, z * scalar);
			}
			
			inline vector3 operator * (const vector3& newVec) const
			{
				return vector3(x * newVec.x, y * newVec.y, z * newVec.z);
			}
			
			inline vector3 operator / (const vec_t scalar) const
			{
				assert(scalar);
				return vector3(x/scalar, y/scalar, z/scalar);
			}
			
			inline vector3 operator / (const vector3& newVec) const
			{
				return vector3(x / newVec.x, y / newVec.y, z / newVec.z);
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
				#ifdef __HAVE_SSE3__
				float d = 0.0f;
				__asm__ __volatile__( 
				"movups (%[vec1]), %%xmm0\n"
				"movups (%[vec2]), %%xmm1\n"
				"mulps %%xmm0, %%xmm1\n"
				"haddps %%xmm1, %%xmm1\n"
				"haddps %%xmm1, %%xmm1\n"
				"movss %%xmm1, %[output]"
				: [output] "=m"(d)
				: [vec1] "r" (vec), [vec2] "r" (newVec.vec)
				: "memory", "%xmm0", "%xmm1");
				return d;
				#else
				return ((x * newVec.x) + (y * newVec.y) + (z * newVec.z));
				#endif
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
			inline void normalize()
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
				printf("X: %f - Y: %f - Z: %f\n", x, y, z);
			}		

			static const vector3 zero;
			static const vector3 unit_x;
			static const vector3 unit_y;
			static const vector3 unit_z;
	};	
}

#endif
// #endif

