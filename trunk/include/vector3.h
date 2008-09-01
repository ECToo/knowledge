
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

			vector3 operator + (const vector3& newVec)
			{
				vector3 tempVec(newVec.x + x, newVec.y + y, newVec.z + z);
				return tempVec;
			}

			vector3& operator += (const vector3& newVec)
			{
				x += newVec.x;
				y += newVec.y;
				z += newVec.z;

				return *this;
			}
			
			vector3& operator = (const vector3& newVec)
			{
				x = newVec.x;
				y = newVec.y;
				z = newVec.z;

				return *this;				
			}
			
			vector3 operator - (const vector3& newVec)
			{
				vector3 tempVec(x - newVec.x, 
						y - newVec.y,
						z - newVec.z);

				return tempVec;				
			}

			vector3& operator -= (const vector3& newVec)
			{
				x -= newVec.x;
				y -= newVec.y;
				z -= newVec.z;

				return *this;
			}

			/**
			 * Multiply this vector by a rotation matrix
			 * @return Another vector with the multiplication stored
			 */
			inline vector3 operator * (const vec_t matrix[9])
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
			
			vector3 operator * (const vec_t scalar)
			{
				vector3 tempVec(x * scalar, 
						y * scalar,
						z * scalar);

				return tempVec;				
			}
			
			vector3 operator * (const vector3& newVec)
			{
				vector3 tempVec(x * newVec.x, 
						y * newVec.y,
						z * newVec.z);

				return tempVec;				
			}
			
			vector3 operator / (const vec_t scalar)
			{
				assert (scalar != 0.0);
				
				vector3 tempVec(x/scalar, y/scalar, z/scalar);
				return tempVec;				
			}
			
			vector3 operator / (const vector3& newVec)
			{
				vector3 tempVec(x / newVec.x, 
						y / newVec.y,
						z / newVec.z);

				return tempVec;				
			}
			
			bool operator == (const vector3& newVec)
			{
				return (
						x == newVec.x && 
						y == newVec.y &&
						z == newVec.z
						);
			}

			void negate()
			{
				x = -x;
				y = -y;
				z = -z;
			}
			
			const vec_t length()
			{
				return sqrt(x*x + y*y + z*z);
			}
			
			/**
	 		 * Dot product between this vector and another one
	 		 */
			const vec_t dotProduct(const vector3& newVec)
			{
				return ((x * newVec.x) + (y * newVec.y) + (z * newVec.z));
			}

			/**
	 		 * The cross product between this vector and another vector
	 		 */
			vector3 crossProduct(const vector3& newVec)
			{
				vector3 tempVec
				(
					y * newVec.z - z * newVec.y,
					z * newVec.x - x * newVec.z,
					x * newVec.y - y * newVec.x
				);
				
				return tempVec;
			}

			vector3& cross(const vector3& newVec)
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
			void normalise()
			{
				vec_t sq = 1.0 / sqrt(length());

				x *= sq;
				y *= sq;
				z *= sq;
			}
			
			/**
	 		 * Distance between this vector and another vector
	 		 */
			inline const vec_t distance(const vector3& newVec)
			{
				return sqrt(pow((newVec.x-x), 2) + 
						pow((newVec.y-y), 2) + 
						pow((newVec.z-z), 2));
			}
			
			/**
	 		 * Print Vector information
	 		 */
			void cout()
			{
				std::cout << "X: " << x << std::endl;
				std::cout << "Y: " << y << std::endl;
				std::cout << "Z: " << z << std::endl;
			}		
	};	
}

#endif
