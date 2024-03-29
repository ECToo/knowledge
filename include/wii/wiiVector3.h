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

#ifdef __WII__

#ifndef _WIIVECTOR3D_H_ 
#define _WIIVECTOR3D_H_ 

#include "prerequisites.h"

namespace k
{
	/**
	 * vector3 Class
	 * Used to hold 3d coordinates
	 */
	class DLL_EXPORT vector3 : public Vector
	{
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

			/**
			 * Creates a vector from a formated string,
			 * where values are separated by white space.
			 */
			vector3(const std::string& str, const std::string& del = " ")
			{
				x = y = z = 0;

				if (str.length())
					sscanf(str.c_str(), "%f %f %f", &x, &y, &z);
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
				vector3 tempVec;
				guVecAdd((Vector*)&(*this), (Vector*)&newVec, (Vector*)&tempVec);

				return tempVec;
			}

			inline vector3& operator += (const vector3& newVec)
			{
				guVecAdd((Vector*)&(*this), (Vector*)&newVec, (Vector*)&(*this));
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
				vector3 tempVec;
				guVecSub((Vector*)&(*this), (Vector*)&newVec, (Vector*)&tempVec);

				return tempVec;				
			}

			inline vector3& operator -= (const vector3& newVec)
			{
				guVecSub((Vector*)&(*this), (Vector*)&newVec, (Vector*)&(*this));
				return *this;
			}

			inline vector3& operator *= (const vec_t newVec)
			{
				guVecScale((Vector*)&(*this), (Vector*)&(*this), newVec);
				return *this;
			}

			/**
			 * Multiply this vector by a rotation matrix
			 * @return Another vector with the multiplication stored
			 */
			inline vector3 operator * (const vec_t matrix[9]) const
			{
				// TODO
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
				// TODO
				x = x * matrix[0] + y * matrix[3] + z * matrix[6];
				y = x * matrix[1] + y * matrix[4] + z * matrix[7];
				z = x * matrix[2] + y * matrix[5] + z * matrix[8];
				
				return *this;				
			}
			
			inline vector3 operator * (const vec_t scalar) const
			{
				vector3 tempVec;
				guVecScale((Vector*)&(*this), (Vector*)&tempVec, scalar);

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
				
				vector3 tempVec;
				guVecScale((Vector*)&(*this), (Vector*)&tempVec, 1.0f / scalar);

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
				return guVecDotProduct((Vector*)&(*this), (Vector*)&newVec);
			}

			/**
	 		 * The cross product between this vector and another vector
	 		 */
			inline vector3 crossProduct(const vector3& newVec) const
			{
				vector3 tempVec;
				guVecCross((Vector*)&(*this), (Vector*)&newVec, (Vector*)&tempVec);

				return tempVec;
			}

			inline vector3& cross(const vector3& newVec)
			{
				vector3 tempVec;
				guVecCross((Vector*)&(*this), (Vector*)&newVec, (Vector*)&tempVec);

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
				guVecNormalize((Vector*)&(*this));
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
#endif

