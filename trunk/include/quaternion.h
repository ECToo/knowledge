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
				#ifdef __WII__
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
			
			quaternion(vec_t xx, vec_t yy, vec_t zz, vec_t ww)
			{ 
				x = xx;
				y = yy;
				z = zz;
				w = ww;
			}

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

			#define DEG_TO_RAD(T) ((T/180.0f) * M_PI)
			#define RAD_TO_DEG(T) ((T/M_PI) * 180.0f)

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

			inline void clear()
			{
				w = 1;
				x = y = z = 0;
			}

			inline quaternion& operator = (const quaternion& newQuat)
			{
				w = newQuat.w;
				x = newQuat.x;
				y = newQuat.y;
				z = newQuat.z;

				return *this;				
			}

			inline bool operator == (const quaternion& newQuat) const
			{
				return (
						x == newQuat.x && 
						y == newQuat.y &&
						z == newQuat.z &&
						w == newQuat.w
						);
			}

			inline bool operator != (const quaternion& newQuat) const
			{
				return !(
						x == newQuat.x && 
						y == newQuat.y &&
						z == newQuat.z &&
						w == newQuat.w
						);
			}

			inline quaternion operator * (const quaternion& newQuat) const
			{
				quaternion output;
	
				output.w = w*newQuat.w - x*newQuat.x - y*newQuat.y - z*newQuat.z;
				output.x = x*newQuat.w + w*newQuat.x + y*newQuat.z - z*newQuat.y;
				output.y = y*newQuat.w + w*newQuat.y + z*newQuat.x - x*newQuat.z;
				output.z = z*newQuat.w + w*newQuat.z + x*newQuat.y - y*newQuat.x;

				return output;
			}

			inline quaternion operator * (const vector3& newVec) const
			{
				quaternion output;

				#ifdef __WII__

				Vector tmp;
				tmp.x = newVec.x;
				tmp.y = newVec.y;
				tmp.z = newVec.z;
			
				register f32 vXY,vZZ,qXY,qZW;
				register f32 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
				register char* a = (char*)&tmp;
				register char* b = (char*)quat;
				register char* ab = (char*)output.quat;

				__asm__ __volatile__ (
				"psq_l		%0,0(%12),0,0\n"		// [vx][vy]
				"lfs			%1,8(%12)\n"			// [vz][vz]
				"psq_l		%2,0(%13),0,0\n"		// [qx][qy]
				"ps_neg		%4,%0\n"					// [-vx][-vy]
				"psq_l		%3,8(%13),0,0\n"		// [qz][qw]
				"ps_neg		%5,%1\n"					// [-vz][-vz]
				"ps_merge01	%6,%5,%1\n"				// [-vz][vz]
				"ps_mul		%8,%3,%6\n"				// [qz*-vz][qw*vz]
				"ps_muls1	%9,%4,%2\n"				// [-vx*qy][-vy*qy]
				"ps_merge01	%11,%4,%0\n"			// [-vx][vy]
				"ps_madds0	%7,%11,%2,%8\n"		// [-vx*qx+qz*-vz][vy*qx+qw*vz]
				"ps_sum0	%10,%7,%7,%9\n"			// [-vx*qx+qz*-vz+-vy*qy][vy*qx+qw*vz]
				"ps_sum1	%10,%9,%10,%7\n"			// [-vx*qx+qz*-vz+-vy*qy][-vx*qy+vy*qx+qw*vz]
				"ps_merge10	%10,%10,%10\n"			// [-vx*qy+vy*qx+qw*vz][-vx*qx+qz*-vz+-vy*qy]
				"psq_st		%10,8(%14),0,0\n"		// Z = [-vx*qy+vy*qx+qw*vz], W = [-vx*qx+qz*-vz+-vy*qy]
				"ps_merge10	%8,%2,%2\n"				// [qy][qx]
				"ps_muls0	%9,%8,%5\n"				// [qy*-vz][qx*-vz]
				"ps_muls0	%7,%0,%3\n"				// [vx*qz][vy*qz]
				"ps_merge10	%7,%7,%7\n"				// [vy*qz][vx*qz]
				"ps_madds1	%10,%0,%3,%7\n"		// [vx*qw+vy*qz][vy*qw+vx*qz]
				"ps_sum0	%8,%10,%10,%9\n"			// [vx*qw+vy*qz+qy*-vz][vy*qw+vx*qz]
				"ps_sum1	%8,%9,%8,%10\n"			// [vx*qw+vy*qz+qy*-vz][qx*-vz+vy*qw+vx*qz]
				"psq_st		%8,0(%14),0,0"			// X = [vx*qw+vy*qz+qy*-vz], Y = [qx*-vz+vy*qw+vx*qz]

				// Output
				: "=&f"(vXY),"=&f"(vZZ),"=&f"(qXY),
				"=&f"(qZW),"=&f"(tmp0),"=&f"(tmp1),
				"=&f"(tmp2),"=&f"(tmp3),"=&f"(tmp4),
				"=&f"(tmp5),"=&f"(tmp6),"=&f"(tmp7)

				// Input
				: "r"(a),"r"(b),"r"(ab)
				);

				#else

				output.w = -x*newVec.x - y*newVec.y - z*newVec.z;
				output.x =  w*newVec.x + y*newVec.z - z*newVec.y;
				output.y =  w*newVec.y + z*newVec.y - x*newVec.z;
				output.z =  w*newVec.z + x*newVec.y - y*newVec.x;

				#endif

				return output;
			}

			inline vector3 rotateVector (const vector3& newVec) const
			{
				quaternion qInv = duplicate().negate();
				quaternion copy = duplicate();

				quaternion qPoint(newVec);
				quaternion qTemp = copy * qPoint;
				quaternion qFinal = qTemp * qInv;

				return vector3(qFinal.x, qFinal.y, qFinal.z);
			}

			inline vector3 inverseVector (const vector3& newVec) const
			{
				quaternion qInv = duplicate().negate();
				quaternion copy = duplicate();

				quaternion qPoint(newVec);
				quaternion qTemp = qInv * qPoint;
				quaternion qFinal = qTemp * copy;

				return vector3(qFinal.x, qFinal.y, qFinal.z);
			}

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

			inline void computeW()
			{
				vec_t temp = 1.0f - x*x - y*y - z*z;

				if (temp < 0.0f)
					w = 0.0f;
				else
					w = -(vec_t)sqrt(temp);
			}

			inline quaternion& negate()
			{
				x = -x;
				y = -y;
				z = -z;

				return *this;
			}

			inline vector3 getDirection() const
			{
				// Lets Consider here the
				// direction of the default camera
				// is -Z (0, 0, -1)
				return rotateVector(vector3(0, 0, -1));
			}

			inline quaternion duplicate() const
			{
				return quaternion(x, y, z, w);
			}
			
			/**
	 		 * Print Quaternion information
	 		 */
			inline void cout() const
			{
				printf("X: %f - Y: %f - Z: %f - W: %f\n", x, y, z, w);
			}		
	};	
}

#endif
