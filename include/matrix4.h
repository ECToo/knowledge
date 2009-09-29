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

#ifndef _MATRIX4_H_
#define _MATRIX4_H_ 

#include "prerequisites.h"
#include "vector3.h"

namespace k
{
	/**
	 * \brief Handles 4x4 matrixes. Still WIP
	 * You can access matrix data by calling matrixName.m[x][y] where x is the column
	 * and y the row.
	 */
	class DLL_EXPORT matrix4
	{
		public:
			vec_t m[4][4];

			// Initialize to identity
			matrix4()
			{
				memset(m, 0, sizeof(vec_t) * 16);
				m[0][0] = 1.0f;
				m[1][1] = 1.0f;
				m[2][2] = 1.0f;
				m[3][3] = 1.0f;
			}

			/**
			 * Creates the matrix by each member. Keep in mind that we are
			 * like OpenGL (Column Major).
			 */
			matrix4
					(vec_t m00, vec_t m01, vec_t m02, vec_t m03,
					vec_t m10, vec_t m11, vec_t m12, vec_t m13,
					vec_t m20, vec_t m21, vec_t m22, vec_t m23,
					vec_t m30, vec_t m31, vec_t m32, vec_t m33)
			{
				m[0][0] = m00;
				m[0][1] = m01;
				m[0][2] = m02;
				m[0][3] = m03;

				m[1][0] = m10;
				m[1][1] = m11;
				m[1][2] = m12;
				m[1][3] = m13;

				m[2][0] = m20;
				m[2][1] = m21;
				m[2][2] = m22;
				m[2][3] = m23;

				m[3][0] = m30;
				m[3][1] = m31;
				m[3][2] = m32;
				m[3][3] = m33;
			}

			/**
			 * Reset this matrix to identity
			 */
			void setIdentity()
			{
				memset(m, 0, sizeof(vec_t) * 16);
				m[0][0] = 1.0f;
				m[1][1] = 1.0f;
				m[2][2] = 1.0f;
				m[3][3] = 1.0f;
			}

			/**
			 * Get the transpose of this matrix.
			 */
			matrix4 transpose() const
			{
				matrix4 newM;
				for (unsigned int row = 0; row < 4; row++)
				{
					for (unsigned int col = 0; col < 4; col++)
					{
						newM.m[row][col] = m[col][row];
					}
				}

				return newM;
			}

			/**
			 * Copy source matrix to this one.
			 * @param[in] mat source matrix.
			 */
			matrix4& operator= (matrix4 mat)
			{
				for (unsigned int row = 0; row < 4; row++)
				{
					for (unsigned int col = 0; col < 4; col++)
					{
						m[row][col] = mat.m[row][col];
					}
				}

				return *this;
			}

			/**
			 * Multiply this matrix by another one.
			 * @param[in] mat Matrix to multiply this one by.
			 */
			matrix4& operator*= (matrix4 mat)
			{
				matrix4 temp;
				temp = *this * mat;
				*this = temp;
				
				return *this;
			}
        
			/**
			 * Rotate a vector by multiplying it with this matrix.
			 * This function was borrowed from Ogre3D.
			 */
			inline vector3 operator * (const vector3& v) const
			{
				vector3 r;

				vec_t fInvW = 1.0 / (m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3]);

				r.x = (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) * fInvW;
				r.y = (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) * fInvW;
				r.z = (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) * fInvW;

				return r;
			}

			/**
			 * Return the inverse of this matrix.
			 * This function was borrowed from Ogre3D.
			 */
			matrix4 inverse() const
			{
				vec_t m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
				vec_t m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
				vec_t m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
				vec_t m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];
        
				vec_t v0 = m20 * m31 - m21 * m30;
				vec_t v1 = m20 * m32 - m22 * m30;
				vec_t v2 = m20 * m33 - m23 * m30;
				vec_t v3 = m21 * m32 - m22 * m31;
				vec_t v4 = m21 * m33 - m23 * m31;
				vec_t v5 = m22 * m33 - m23 * m32;
        
				vec_t t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
				vec_t t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
				vec_t t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
				vec_t t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

				vec_t invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);
        
				vec_t d00 = t00 * invDet;
				vec_t d10 = t10 * invDet;
				vec_t d20 = t20 * invDet;
				vec_t d30 = t30 * invDet;

				vec_t d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
				vec_t d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
				vec_t d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
				vec_t d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

				v0 = m10 * m31 - m11 * m30;
				v1 = m10 * m32 - m12 * m30;
				v2 = m10 * m33 - m13 * m30;
				v3 = m11 * m32 - m12 * m31;
				v4 = m11 * m33 - m13 * m31;
				v5 = m12 * m33 - m13 * m32;

				vec_t d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
				vec_t d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
				vec_t d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
				vec_t d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

				v0 = m21 * m10 - m20 * m11;
				v1 = m22 * m10 - m20 * m12;
				v2 = m23 * m10 - m20 * m13;
				v3 = m22 * m11 - m21 * m12;
				v4 = m23 * m11 - m21 * m13;
				v5 = m23 * m12 - m22 * m13;

				vec_t d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
				vec_t d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
				vec_t d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
				vec_t d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;
        
				return matrix4(
						d00, d01, d02, d03,
						d10, d11, d12, d13,
						d20, d21, d22, d23,
						d30, d31, d32, d33);
			}

			/**
			 * Multiply this matrix by another 4x4 matrix.
			 * This function was borrowed from Ogre3D.
			 */
			matrix4 operator* (matrix4 m2)
			{
				matrix4 r;
				r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
            r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
            r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
            r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

            r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
            r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
            r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
            r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

            r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
            r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
            r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
            r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

            r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
            r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
            r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
            r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];
            
				return r;
			}

			vec_t	MINOR(const matrix4& m, const size_t r0, const size_t r1, const size_t r2, 
								const size_t c0, const size_t c1, const size_t c2)
			{
        
				return m.m[r0][c0] * (m.m[r1][c1] * m.m[r2][c2] - m.m[r2][c1] * m.m[r1][c2]) -
					m.m[r0][c1] * (m.m[r1][c0] * m.m[r2][c2] - m.m[r2][c0] * m.m[r1][c2]) +
					m.m[r0][c2] * (m.m[r1][c0] * m.m[r2][c1] - m.m[r2][c0] * m.m[r1][c1]);
			}

			/**
			 * Test if this matrix is the identity matrix
			 */
			bool isIdentity() const
			{
				for (unsigned int i = 0; i < 4; i++)
				{
					for (unsigned int j = 0; j < 4; j++)
					{
						if (i == j && m[i][j] != 1.0)
							return false;
						else 
							continue;

						if (m[i][j] != 0)
							return false;
					}
				}

				return true;
			}

			/**
			 * Return the inverse matrix as a transformation matrix,
			 * with the translation components.
			 */
			matrix4 getInverseTranslation(const vector3& position) const
			{
				matrix4 mFinal = transpose().inverse();
				
				vector3 dir(-mFinal.m[0][2], -mFinal.m[1][2], -mFinal.m[2][2]);
				vector3 up(-mFinal.m[0][1], -mFinal.m[1][1], -mFinal.m[2][1]);
				vector3 right(-mFinal.m[0][0], -mFinal.m[1][0], -mFinal.m[2][0]);

				mFinal.m[3][0] = -right.dotProduct(position);
				mFinal.m[3][1] = -up.dotProduct(position);
				mFinal.m[3][2] = dir.dotProduct(position);
				mFinal.m[3][3] = 1.0f;

				return mFinal;
			}

			/**
			 * Return the determinant of this matrix.
			 */
			vec_t determinant()
			{
				return m[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) -
					m[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) +
					m[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) -
					m[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2);
			}

			// TODO: Get this out of here
			#ifdef __WII__
			void getWiiMatrix(Mtx wiiMat)
			{
				wiiMat[0][0] = m[0][0];
				wiiMat[0][1] = m[1][0];
				wiiMat[0][2] = m[2][0];
				wiiMat[0][3] = m[3][0];

				wiiMat[1][0] = m[0][1];
				wiiMat[1][1] = m[1][1];
				wiiMat[1][2] = m[2][1];
				wiiMat[1][3] = m[3][1];

				wiiMat[2][0] = m[0][2];
				wiiMat[2][1] = m[1][2];
				wiiMat[2][2] = m[2][2];
				wiiMat[2][3] = m[3][2];
			}
			#endif

			/**
			 * Output matrix data.
			 */
			void cout() const
			{
				printf("%f %f %f %f\n", m[0][0], m[1][0], m[2][0], m[3][0]);
				printf("%f %f %f %f\n", m[0][1], m[1][1], m[2][1], m[3][1]);
				printf("%f %f %f %f\n", m[0][2], m[1][2], m[2][2], m[3][2]);
				printf("%f %f %f %f\n", m[0][3], m[1][3], m[2][3], m[3][3]);
			}
	};
}

#endif

