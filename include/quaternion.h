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

#ifndef _QUATERNION_H_
#define _QUATERNION_H_ 

#include "prerequisites.h"
#include "vector3.h"
#include "matrix3.h"
#include "matrix4.h"

namespace k
{
	/**
	 * \brief Quaternions are used to save and transform rotations on space.
	 */
	class DLL_EXPORT quaternion 
	{
		private:
			/**
			 * Credit goes to iD Software 
			 * guys, im not really sure
			 * Who. ASM version by Shagkur.
			 */
			inline float ReciprocalSqrt(float x) 
			{
				#ifdef __WII__ // TODO: REMOVE THIS FROM HERE
				register f32 d = 0.0f;
				__asm__ __volatile__(
						"frsqrte	%0,%1\n"
						"frsp		%0,%0"
						: "=f"(d) : "f"(x));

				return d;
				#else
				typedef union
				{
					float f;
					long l;
				} mFloatLong;

				mFloatLong i, xx;
				float y, r;

				xx.f = x;
    			y = x * 0.5f;
    			i.l = xx.l;
    			i.l = 0x5f3759df - ( i.l >> 1 );
    			r = i.f;
    			r = r * ( 1.5f - r * r * y );

    			return r;
				#endif
			}

		public:
			// Allow us to access like packed data
			union
			{
				struct 
				{
					vec_t x, y, z, w;
				};
				vec_t quat[4];
			};

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
			
			/**
			 * Create a quaternion from its parameters.
	 		 */
			quaternion(vec_t xx, vec_t yy, vec_t zz, vec_t ww)
			{ 
				x = xx;
				y = yy;
				z = zz;
				w = ww;
			}

			/**
			 * Create a quaternion from a rotation matrix.
			 */
			quaternion(const matrix3& m)
			{
				if (m.m[0][0] + m.m[1][1] + m.m[2][2] > 0.0f) 
				{
					float t = + m.m[0][0] + m.m[1][1] + m.m[2][2] + 1.0f;
					float s = ReciprocalSqrt(t) * 0.5f;

    				w = s * t;
					z = ( m.m[1][0] - m.m[0][1] ) * s;
					y = ( m.m[0][2] - m.m[2][0] ) * s;
					x = ( m.m[2][1] - m.m[1][2] ) * s;
				} 
				else
				if (m.m[0][0] > m.m[1][1] && m.m[0][0] > m.m[2][2]) 
				{
    				float t = + m.m[0][0] - m.m[1][1] - m.m[2][2] + 1.0f;
    				float s = ReciprocalSqrt(t) * 0.5f;
    				
					x = s * t;
    				y = ( m.m[1][0] + m.m[0][1] ) * s;
					z = ( m.m[0][2] + m.m[2][0] ) * s;
					w = ( m.m[2][1] - m.m[1][2] ) * s;
				}
				else 
				if (m.m[1][1] > m.m[2][2]) 
				{
    				float t = - m.m[0][0] + m.m[1][1] - m.m[2][2] + 1.0f;
					float s = ReciprocalSqrt(t) * 0.5f;

					y = s * t;
					x = ( m.m[1][0] + m.m[0][1] ) * s;
					w = ( m.m[0][2] - m.m[2][0] ) * s;
					z = ( m.m[2][1] + m.m[1][2] ) * s;
				} 
				else 
				{
					float t = - m.m[0][0] - m.m[1][1] + m.m[2][2] + 1.0f;
					float s = ReciprocalSqrt(t) * 0.5f;

					z = s * t;
					w = ( m.m[1][0] - m.m[0][1] ) * s;
					x = ( m.m[0][2] + m.m[2][0] ) * s;
					y = ( m.m[2][1] + m.m[1][2] ) * s;
				}
			}

			/**
			 * Create a quaternion from a vector3 (w = 0, x = vX, y = vY, z = vZ)
			 */
			quaternion(const vector3& newVec)
			{
				w = 0;
				x = newVec.x;
				y = newVec.y;
				z = newVec.z;
			}
			
			/**
			 * Create a quaternion from another quaternion.
			 */
			quaternion(const quaternion& newQuat)
			{
				w = newQuat.w;
				x = newQuat.x;
				y = newQuat.y;
				z = newQuat.z;
			}

			#define DEG_TO_RAD(T) ((T/180.0f) * M_PI)
			#define RAD_TO_DEG(T) ((T/M_PI) * 180.0f)

			/**
			 * Create a quaternion from axis-angle rotations.
			 */
			inline quaternion(const vec_t angle, const vector3& axis)
			{
				vec_t r, s;

				r = DEG_TO_RAD(angle) * 0.5f;
    			s = sin(r);

    			w = cos(r);
    			x = axis.x * s;
    			y = axis.y * s;
	 			z = axis.z * s;

				normalize();
			}

			/**
			 * Reset this quaternion.
			 */
			inline void clear()
			{
				w = 1;
				x = y = z = 0;
			}

			/**
			 * Copy source quaternion to this one.
			 * @param[in] newQuat Source quaternion.
			 */
			inline quaternion& operator = (const quaternion& newQuat)
			{
				w = newQuat.w;
				x = newQuat.x;
				y = newQuat.y;
				z = newQuat.z;

				return *this;				
			}

			/**
			 * Check if two quaternions are equal.
			 */
			inline bool operator == (const quaternion& newQuat) const
			{
				return (
						x == newQuat.x && 
						y == newQuat.y &&
						z == newQuat.z &&
						w == newQuat.w
						);
			}

			/**
			 * Check if two quaternions are different.
			 */
			inline bool operator != (const quaternion& newQuat) const
			{
				return !(
						x == newQuat.x && 
						y == newQuat.y &&
						z == newQuat.z &&
						w == newQuat.w
						);
			}

			/**
			 * Multiply this quaternion by another one.
			 * @param[in] newQuat The quaternion to multiply this one by.
			 */
			inline quaternion operator * (const quaternion& newQuat) const
			{
				quaternion output;
	
				output.w = w*newQuat.w - x*newQuat.x - y*newQuat.y - z*newQuat.z;
				output.x = x*newQuat.w + w*newQuat.x + y*newQuat.z - z*newQuat.y;
				output.y = y*newQuat.w + w*newQuat.y + z*newQuat.x - x*newQuat.z;
				output.z = z*newQuat.w + w*newQuat.z + x*newQuat.y - y*newQuat.x;

				return output;
			}

			/**
			 * Multiply this quaternion by a 3D vector.
			 * @param[in] newVec The 3D vector to multiply this one by.
			 */
			inline quaternion operator * (const vector3& newVec) const
			{
				quaternion output;

				output.w = -x*newVec.x - y*newVec.y - z*newVec.z;
				output.x =  w*newVec.x + y*newVec.z - z*newVec.y;
				output.y =  w*newVec.y + z*newVec.y - x*newVec.z;
				output.z =  w*newVec.z + x*newVec.y - y*newVec.x;

				return output;
			}

			/**
			 * Rotate a vector using this quaternion.
			 * @param newVec A vector to be rotated.
			 */
			inline vector3 rotateVector (const vector3& newVec) const
			{
				quaternion qInv = duplicate().negate();
				quaternion copy = duplicate();

				quaternion qPoint(newVec);
				quaternion qTemp = copy * qPoint;
				quaternion qFinal = qTemp * qInv;

				return vector3(qFinal.x, qFinal.y, qFinal.z);
			}

			/**
			 * Un-Rotate a vector rotated by this quaternion. (Get the inverse rotation)
			 */
			inline vector3 inverseVector (const vector3& newVec) const
			{
				quaternion qInv = duplicate().negate();
				quaternion copy = duplicate();

				quaternion qPoint(newVec);
				quaternion qTemp = qInv * qPoint;
				quaternion qFinal = qTemp * copy;

				return vector3(qFinal.x, qFinal.y, qFinal.z);
			}

			/**
			 * Normalize the quaternion.
			 */
			inline void normalize()
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

			/**
			 * Get a 4x4 matrix out of this quaternion.
			 */
			matrix4 toMatrix() const
			{
				matrix4 mat;

				float x2 = x + x;
				float y2 = y + y;
				float z2 = z + z;
				{
					float xx2 = x * x2;
					float yy2 = y * y2;
					float zz2 = z * z2;

					mat.m[0][0] = 1.0f - yy2 - zz2;
					mat.m[1][1] = 1.0f - xx2 - zz2;
					mat.m[2][2] = 1.0f - xx2 - yy2;
				}
				{
					float yz2 = y * z2;
					float wx2 = w * x2;

					mat.m[1][2] = yz2 - wx2;
					mat.m[2][1] = yz2 + wx2;
				}
				{
					float xy2 = x * y2;
					float wz2 = w * z2;

					mat.m[0][1] = xy2 - wz2;
					mat.m[1][0] = xy2 + wz2;
				}
				{
					float xz2 = x * z2;
					float wy2 = w * y2;

					mat.m[2][0] = xz2 - wy2;
					mat.m[0][2] = xz2 + wy2;
				}

				mat.m[0][3] = 0.0f;
				mat.m[1][3] = 0.0f;
				mat.m[2][3] = 0.0f;
				mat.m[3][0] = 0.0f;
				mat.m[3][1] = 0.0f;
				mat.m[3][2] = 0.0f;
				mat.m[3][3] = 1.0f;

				return mat;
			}

			/**
			 * Get a 4x4 matrix out of this quaternion.
			 * @param[out] matrix An array of 16 floats to get the resulting matrix.
			 */
			void toMatrix(vec_t matrix[]) const
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

			/**
			 * Extract axis-angle information from this quaternion.
			 * @param[out] angle Result Angle (output).
			 * @param[out] axis Result Axis (output).
			 */
			void toAxisAngle(vec_t& angle, vector3& axis) 
			{
				if (w > 1)
					normalize();

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

			/**
			 * Calculate the W of this quaternion, from its x, y and z.
			 */
			inline void computeW()
			{
				vec_t temp = 1.0f - x*x - y*y - z*z;

				if (temp < 0.0f)
					w = 0.0f;
				else
					w = -(vec_t)sqrt(temp);
			}

			/**
			 * Negate the quaternion.
			 */
			inline quaternion& negate()
			{
				x = -x;
				y = -y;
				z = -z;

				return *this;
			}

			/**
			 * Get the quaternion direction considering that the
			 * default direction is -Z -> (0, 0, -1)
			 */
			inline vector3 getDirection() const
			{
				// Lets Consider here the
				// direction of the default camera
				// is -Z (0, 0, -1)
				return rotateVector(vector3(0, 0, -1));
			}

			/**
			 * Duplicate the quaternion.
			 */
			inline quaternion duplicate() const
			{
				return quaternion(x, y, z, w);
			}
			
			/**
	 		 * Output Quaternion data
	 		 */
			inline void cout() const
			{
				printf("X: %f - Y: %f - Z: %f - W: %f\n", x, y, z, w);
			}		

			/**
			 * Identity quaternion.
			 */
			static const quaternion identity;
	};	
}

#endif
