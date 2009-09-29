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

#ifndef _VECTOR3D_H_ 
#define _VECTOR3D_H_ 

#include "prerequisites.h"

namespace k
{
	/**
	 * \brief Hold 3d coordinates
	 */
	class DLL_EXPORT vector3 
	{
		public:
			// Allow us to access like packed data.
			union
			{
				struct 
				{
					vec_t x, y, z;
				};
				vec_t vec[3];
			};

			/**
	 		 * Default Constructor
			 * Start as a blank vector(0,0)
	 		 */
			vector3()
			{ 
				x = y = z = 0;
			}

			/**
			 * Created a new vector as a copy of source vector.
			 *
			 * @param[in] in Source vector.
			 */
			vector3(const vector3& in)
			{
				x = in.x;
				y = in.y;
				z = in.z;
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

			/**
			 * Creates a vector from a formated string,
			 * where values are separated by white space.
			 */
			vector3(const std::string& str)
			{
				x = y = z = 0;

				if (str.length())
					sscanf(str.c_str(), "%f %f %f", &x, &y, &z);
			}

			/**
			 * Reflect this vector on a normal.
			 */
			vector3 reflect(const vector3& normal)
			{
				vec_t dot = -2 * dotProduct(normal);

				return vector3(
						x + dot * normal.x,
						y + dot * normal.y,
						z + dot * normal.z);
			}

			/**
			 * Sum this vector with another one.
			 */
			inline vector3 operator + (const vector3& newVec) const
			{
				return vector3(newVec.x + x, newVec.y + y, newVec.z + z);
			}

			/**
			 * Sum this vector with another one.
			 */
			inline vector3& operator += (const vector3& newVec)
			{
				x += newVec.x;
				y += newVec.y;
				z += newVec.z;

				return *this;
			}
			
			/**
			 * Copy the source vector to this one.
			 * @param[in] newVec source vector.
			 */
			inline vector3& operator = (const vector3& newVec)
			{
				x = newVec.x;
				y = newVec.y;
				z = newVec.z;

				return *this;				
			}
			
			/**
			 * Subtract a vector from this one.
			 * @param[in] newVec A vector to be subtracted from this one.
			 */
			inline vector3 operator - (const vector3& newVec) const
			{
				vector3 tempVec(x - newVec.x, 
						y - newVec.y,
						z - newVec.z);

				return tempVec;				
			}

			/**
			 * Subtract a vector from this one.
			 * @param[in] newVec A vector to be subtracted from this one.
			 */
			inline vector3& operator -= (const vector3& newVec)
			{
				x -= newVec.x;
				y -= newVec.y;
				z -= newVec.z;

				return *this;
			}

			/**
			 * Multiply this vector by a scalar.
			 */
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
			inline vector3 operator * (const vec_t matrix[3][3]) const
			{
				vector3 tempVec
						(
						x * matrix[0][0] + y * matrix[0][1] + z * matrix[0][2],
						x * matrix[1][0] + y * matrix[4][1] + z * matrix[7][2],
						x * matrix[2][0] + y * matrix[5][1] + z * matrix[8][2]
						);
				
				return tempVec;				
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
			
			/**
			 * Multiply this vector by a scalar.
			 */
			inline vector3 operator * (const vec_t scalar) const
			{
				return vector3(x * scalar, y * scalar, z * scalar);
			}
			
			/**
			 * Multiply this vector by another vector.
			 */
			inline vector3 operator * (const vector3& newVec) const
			{
				return vector3(x * newVec.x, y * newVec.y, z * newVec.z);
			}
			
			/**
			 * Divide this vector by a scalar.
			 */
			inline vector3 operator / (const vec_t scalar) const
			{
				assert(scalar);
				return vector3(x/scalar, y/scalar, z/scalar);
			}
			
			/**
			 * Divide this vector by another vector.
			 */
			inline vector3 operator / (const vector3& newVec) const
			{
				return vector3(x / newVec.x, y / newVec.y, z / newVec.z);
			}
			
			/**
			 * Return true if both vectors are identical.
			 */
			inline bool operator == (const vector3& newVec) const
			{
				return (
						x == newVec.x && 
						y == newVec.y &&
						z == newVec.z
						);
			}

			/**
			 * Return true if both vectors are NOT identical.
			 */
			inline bool operator != (const vector3& newVec) const
			{
				return !(
						x == newVec.x && 
						y == newVec.y &&
						z == newVec.z
						);
			}

			/**
			 * Return a negated copy of this vector.
			 */
			inline vector3 negate() const
			{
				return vector3(-x, -y, -z);
			}

			/**
			 * Negate this vector.
			 */
			inline vector3 negateItself()
			{
				x = -x;
				y = -y;
				z = -z;

				return *this;
			}
			
			/**
			 * Returns the length of this vector.
			 */
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

			/**
			 * Cross product this vector and another and save
			 * the results on this vector.
			 */
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

