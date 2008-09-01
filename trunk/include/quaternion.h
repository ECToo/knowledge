
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
				quaternion qInv(*this);
				qInv.negate();

				quaternion qPoint(newVec);
				quaternion qTemp = qInv * qPoint;
				quaternion qFinal = qTemp * qInv;

				vector3 output
				(
					qFinal.x,
					qFinal.y,
					qFinal.z
				);

				return output;
			}

			vector3 inverseVector (const vector3& newVec)
			{
				quaternion qInv(*this);
				qInv.negate();

				quaternion qPoint(newVec);
				quaternion qTemp = qInv * qPoint;
				quaternion qFinal = qTemp * *this;

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

			void computeW()
			{
				vec_t temp = 1.0f - (x*x - y*y - z*z);

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
