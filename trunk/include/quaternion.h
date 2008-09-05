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

			quaternion(vec_t angle, const vector3& axis)
			{
				vec_t r, s;
				r = (angle/180.0f) * M_PI;
    			s = sin(r * 0.5f);

    			w = cos(r * 0.5f);
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
				vector3 direction;

				direction.x = 2.0f*(x * z - w * y);
				direction.y = 2.0f*(y * z + w * x);
				direction.z = 2.0f*(x * x + y * y);

				return direction;
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
