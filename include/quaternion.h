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

#ifndef _QUATERNION_H_
#define _QUATERNION_H_ 

#include "prerequisites.h"
#include "vector3.h"
#include "matrix3.h"
#include "matrix4.h"

namespace k
{
	/**
	 * quaternion Class
	 * Used to hold 4d coordinates
	 */
	class quaternion 
	{
		public:
			vec_t x, y, z, w;

		public:
			/**
	 		 * Default Constructor
			 * Start as a blank quaternion(1,0,0,0)
	 		 */
			quaternion()
			{ 
				w = 1;
				x = y = z = 0;
			}
			
			quaternion(vec_t xx, vec_t yy, vec_t zz, vec_t ww)
			{ 
				x = xx;
				y = yy;
				z = zz;
				w = ww;
			}

			/**
			 * This function has been taken from 
			 * Ogre3D - All credit goes to ogre3d team
			 */
			quaternion(matrix3 mat)
			{
				// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
				// article "Quaternion Calculus and Fast Animation".

				vec_t fTrace = mat.m[0][0] + mat.m[1][1] + mat.m[2][2];
				vec_t fRoot;

	        
				if ( fTrace > 0.0 )
				{
					// |w| > 1/2, may as well choose w > 1/2
					fRoot = sqrt(fTrace + 1.0);  // 2w

					w = 0.5 * fRoot;
					fRoot = 0.5/fRoot;  // 1/(4w)
					x = (mat.m[2][1] - mat.m[1][2]) * fRoot;
					y = (mat.m[0][2] - mat.m[2][0]) * fRoot;
					z = (mat.m[1][0] - mat.m[0][1]) * fRoot;
	        }
	        else
	        {
	            // |w| <= 1/2
	            static size_t s_iNext[3] = { 1, 2, 0 };
	            size_t i = 0;

	            if (mat.m[1][1] > mat.m[0][0])
	                i = 1;

	            if (mat.m[2][2] > mat.m[i][i])
	                i = 2;

	            size_t j = s_iNext[i];
	            size_t k = s_iNext[j];
	
	            fRoot = sqrt(mat.m[i][i] - mat.m[j][j] - mat.m[k][k] + 1.0);
	            vec_t* apkQuat[3] = { &x, &y, &z };
	            *apkQuat[i] = 0.5 * fRoot;
	            fRoot = 0.5 / fRoot;
	            w = (mat.m[k][j] - mat.m[j][k]) * fRoot;
	            *apkQuat[j] = (mat.m[j][i] + mat.m[i][j]) * fRoot;
					*apkQuat[k] = (mat.m[k][i] + mat.m[i][k]) * fRoot;
			  }
			}

			quaternion(const vector3& newVec)
			{
				w = 0;
				x = newVec.x;
				y = newVec.y;
				z = newVec.z;
			}
			
			quaternion(const quaternion& newQuat)
			{
				w = newQuat.w;
				x = newQuat.x;
				y = newQuat.y;
				z = newQuat.z;
			}

			quaternion(const vec_t angle, const vector3& axis)
			{
				vec_t r, s;

				r = (angle/180.0f) * M_PI;
				r *= 0.5f;

    			s = sin(r);

    			w = cos(r);
    			x = axis.x * s;
    			y = axis.y * s;
	 			z = axis.z * s;
			}

			void clear()
			{
				w = 1;
				x = y = z = 0;
			}

			quaternion& operator = (const quaternion& newQuat)
			{
				w = newQuat.w;
				x = newQuat.x;
				y = newQuat.y;
				z = newQuat.z;

				return *this;				
			}

			bool operator == (const quaternion& newQuat)
			{
				return (
						x == newQuat.x && 
						y == newQuat.y &&
						z == newQuat.z &&
						w == newQuat.w
						);
			}

			bool operator != (const quaternion& newQuat)
			{
				return !(
						x == newQuat.x && 
						y == newQuat.y &&
						z == newQuat.z &&
						w == newQuat.w
						);
			}

			quaternion operator * (const quaternion& newQuat)
			{
				quaternion output;
	
				output.w = w*newQuat.w - x*newQuat.x - y*newQuat.y - z*newQuat.z;
				output.x = x*newQuat.w + w*newQuat.x + y*newQuat.z - z*newQuat.y;
				output.y = y*newQuat.w + w*newQuat.y + z*newQuat.x - x*newQuat.z;
				output.z = z*newQuat.w + w*newQuat.z + x*newQuat.y - y*newQuat.x;

				return output;
			}

			quaternion operator * (const vector3& newVec)
			{
				quaternion output;

				output.w = -x*newVec.x - y*newVec.y - z*newVec.z;
				output.x =  w*newVec.x + y*newVec.z - z*newVec.y;
				output.y =  w*newVec.y + z*newVec.y - x*newVec.z;
				output.z =  w*newVec.z + x*newVec.y - y*newVec.x;

				return output;
			}

			vector3 rotateVector (const vector3& newVec)
			{
				quaternion qInv = duplicate().negate();
				quaternion copy = duplicate();

				quaternion qPoint(newVec);
				quaternion qTemp = copy * qPoint;
				quaternion qFinal = qTemp * qInv;

				return vector3(qFinal.x, qFinal.y, qFinal.z);
			}

			vector3 inverseVector (const vector3& newVec)
			{
				quaternion qInv = duplicate().negate();
				quaternion copy = duplicate();

				quaternion qPoint(newVec);
				quaternion qTemp = qInv * qPoint;
				quaternion qFinal = qTemp * copy;

				return vector3(qFinal.x, qFinal.y, qFinal.z);
			}

			void normalise()
			{
				vec_t magnitude = sqrt(w*w + x*x + y*y + z*z);
				if (magnitude > 0.0f)
				{
					vec_t oneOverMagnitude = 1.0f/magnitude;

					w *= oneOverMagnitude;
					x *= oneOverMagnitude;
					y *= oneOverMagnitude;
					z *= oneOverMagnitude;
				}
			}

			matrix4 toMatrix()
			{
				matrix4 mat;

				vec_t xx, yy, zz, xy, wz, xz, wy, yz, wx;

				xx = x * x;
    			yy = y * y;
    			zz = z * z;
    			xy = x * y;
				wz = w * z;
				xz = x * z;
				wy = w * y;
				yz = y * z;
				wx = w * x;

				mat.m[0][0] = 1.0f - 2.0f * (yy + zz);
				mat.m[0][1] = 2.0f * (xy + wz);
				mat.m[0][2] = 2.0f * (xz - wy);
				mat.m[0][3] = 0.0f;

				mat.m[1][0] = 2.0f * (xy - wz);
				mat.m[1][1] = 1.0f - 2.0f * (xx + zz);
				mat.m[1][2] = 2.0f * (yz + wx);
				mat.m[1][3] = 0.0f;

				mat.m[2][0] = 2.0f * (xz + wy);
				mat.m[2][1] = 2.0f * (yz - wx);
				mat.m[2][2] = 1.0f - 2.0f * (xx + yy);
				mat.m[2][3] = 0.0f;

				mat.m[3][0] = 0.0f;
				mat.m[3][1] = 0.0f;
				mat.m[3][2] = 0.0f;
				mat.m[3][3] = 1.0f;

				return mat;
			}

			void toMatrix(vec_t matrix[])
			{
				assert(matrix != NULL);

				vec_t xx, yy, zz, xy, wz, xz, wy, yz, wx;

				xx = x * x;
    			yy = y * y;
    			zz = z * z;
    			xy = x * y;
				wz = w * z;
				xz = x * z;
				wy = w * y;
				yz = y * z;
				wx = w * x;

				matrix[0] = 1.0f - 2.0f * (yy + zz);
				matrix[1] = 2.0f * (xy + wz);
				matrix[2] = 2.0f * (xz - wy);
				matrix[3] = 0.0f;
				matrix[4] = 2.0f * (xy - wz);
				matrix[5] = 1.0f - 2.0f * (xx + zz);
				matrix[6] = 2.0f * (yz + wx);
				matrix[7] = 0.0f;
				matrix[8] = 2.0f * (xz + wy);
				matrix[9] = 2.0f * (yz - wx);
				matrix[10] = 1.0f - 2.0f * (xx + yy);
				matrix[11] = 0.0f;
				matrix[12] = 0.0f;
				matrix[13] = 0.0f;
				matrix[14] = 0.0f;
				matrix[15] = 1.0f;
			}

			void toAxisAngle(vec_t& angle, vector3& axis)
			{
				if (w > 1)
					normalise();

				angle = 2 * acos(w);
				angle *= 180.0/M_PI;

				vec_t s = sqrt(1 - w*w);
				if (s < 0.001)
				{
					axis.x = x;
					axis.y = y;
					axis.z = z;
				}
				else
				{
					axis.x = x / s;
					axis.y = y / s;
					axis.z = z / s;
				}
			}

			void computeW()
			{
				vec_t temp = 1.0f - x*x - y*y - z*z;

				if (temp < 0.0f)
					w = 0.0f;
				else
					w = -(vec_t)sqrt(temp);
			}

			quaternion& negate()
			{
				x = -x;
				y = -y;
				z = -z;

				return *this;
			}

			vector3 getDirection()
			{
				// Lets Consider here the
				// direction of the default camera
				// is -Z (0, 0, -1)
				return rotateVector(vector3(0, 0, -1));
			}

			quaternion duplicate()
			{
				return quaternion(x, y, z, w);
			}
			
			/**
	 		 * Print Quaternion information
	 		 */
			void cout()
			{
				std::cout << "W: " << w << std::endl;
				std::cout << "X: " << x << std::endl;
				std::cout << "Y: " << y << std::endl;
				std::cout << "Z: " << z << std::endl;
			}		
	};	
}

#endif
