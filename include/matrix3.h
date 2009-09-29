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

#ifndef _MATRIX3_H_
#define _MATRIX3_H_ 

#include "prerequisites.h"
#include "vector3.h"

namespace k
{
	/**
	 * \brief Handles 3x3 matrixes. Still WIP.
	 * You can access matrix data by calling matrixName.m[x][y] where x is the column
	 * and y the row.
	 */
	class DLL_EXPORT matrix3
	{
		public:
			vec_t m[3][3];

			// Initialize to identity
			matrix3()
			{
				memset(m, 0, sizeof(vec_t) * 9);
				m[0][0] = 1.0f;
				m[1][1] = 1.0f;
				m[2][2] = 1.0f;
			}

			/**
			 * Construct our matrix from vectors. The final 
			 * matrix will be a row major (different than the standard column major)
			 * matrix from the vectors contents. The matrix will look like:
			 *
			 * | x.x x.y x.z |
			 * | y.x y.y y.z |
			 * | z.x z.y z.z |
			 *
			 */
			matrix3(vector3 x, vector3 y, vector3 z)
			{
				m[0][0] = x.x;	
				m[0][1] = y.x;	
				m[0][2] = z.x;	

				m[1][0] = x.y;	
				m[1][1] = y.y;	
				m[1][2] = z.y;	

				m[2][0] = x.z;	
				m[2][1] = y.z;	
				m[2][2] = z.z;	
			}

			/**
			 * Get the transpose of this matrix.
			 */
			matrix3 transpose() const
			{
				matrix3 newM;
				for (unsigned int row = 0; row < 3; row++)
				{
					for (unsigned int col = 0; col < 3; col++)
					{
						newM.m[row][col] = m[col][row];
					}
				}

				return newM;
			}

			/**
			 * Sum this matrix with another one
			 * and return the sum.
			 */
			matrix3 operator + (const matrix3& mat) const
			{
				matrix3 result;
				for (unsigned int row = 0; row < 3; row++)
				{
					for (unsigned int col = 0; col < 3; col++)
					{
						result.m[row][col] = mat.m[row][col] + m[row][col];
					}
				}

				return result;
			}

			/**
			 * Sum this matrix with another one and keep the result
			 * in this matrix.
			 */
			matrix3& operator += (const matrix3& mat)
			{
				for (unsigned int row = 0; row < 3; row++)
				{
					for (unsigned int col = 0; col < 3; col++)
					{
						m[row][col] += mat.m[row][col];
					}
				}

				return *this;
			}

			/**
			 * Subtract another matrix from this one
			 * and returna the resultant matrix.
			 */
			matrix3 operator - (const matrix3& mat) const
			{
				matrix3 result;
				for (unsigned int row = 0; row < 3; row++)
				{
					for (unsigned int col = 0; col < 3; col++)
					{
						result.m[row][col] = m[row][col] - mat.m[row][col];
					}
				}

				return result;
			}

			/**
			 * Multiply this matrix by a vector3
			 */
			vector3 operator * (const vector3& vec)
			{
				vector3 result;
				vector3 row1 = vector3(m[0][0], m[1][0], m[2][0]);
				vector3 row2 = vector3(m[0][1], m[1][1], m[2][1]);
				vector3 row3 = vector3(m[0][2], m[1][2], m[2][2]);

				result.x = row1.dotProduct(vec);
				result.y = row2.dotProduct(vec);
				result.z = row3.dotProduct(vec);

				return result;
			}

			/**
			 * Subtract another matrix from this one and keep the
			 * results on it.
			 */
			matrix3& operator -= (const matrix3& mat)
			{
				for (unsigned int row = 0; row < 3; row++)
				{
					for (unsigned int col = 0; col < 3; col++)
					{
						m[row][col] -= mat.m[row][col];
					}
				}

				return *this;
			}

			/**
			 * Copy Source matrix to this one.
			 * @param[in] mat Source matrix.
			 */
			matrix3& operator = (const matrix3& mat)
			{
				for (unsigned int row = 0; row < 3; row++)
				{
					for (unsigned int col = 0; col < 3; col++)
					{
						m[row][col] = mat.m[row][col];
					}
				}

				return *this;
			}

			/**
			 * Multiply this matrix by another and save the
			 * results on it.
			 */
			matrix3& operator *= (const matrix3& mat)
			{
				matrix3 temp;
				temp = *this * mat;
				*this = temp;
				
				return *this;
			}

			/**
			 * Multiply this matrix by another and return a result
			 * matrix.
			 */
			matrix3 operator * (const matrix3& mat) const
			{
				matrix3 result;
				for (unsigned int row = 0; row < 3; row++)
				{
					for (unsigned int col = 0; col < 3; col++)
					{
						result.m[col][row] = m[0][row]*mat.m[col][0] +
							m[1][row]*mat.m[col][1] +
							m[2][row]*mat.m[col][2];
					}
				}

				return result;
			}

			/**
			 * Convert matrix to axis angle.
			 * Thanks to martin baker site for the formula and pseudo java code
			 * wich this one is a port of.
			 */
			void toAxisAngle(vec_t& angle, vector3& axis) const
			{
				double epsilon = 0.01; 
				double epsilon2 = 0.1;
	
				if ((fabs(m[0][1] - m[1][0]) < epsilon) &&
					(fabs(m[0][2] - m[2][0]) < epsilon) &&
					(fabs(m[1][2] - m[2][1]) < epsilon)) 
				{
					// singularity found
					// first check for identity matrix which must have +1 for all terms
					//  in leading diagonaland zero in other terms
					if ((fabs(m[0][1] + m[1][0]) < epsilon2) &&
						 (fabs(m[0][2] + m[2][0]) < epsilon2) &&
						 (fabs(m[1][2] + m[2][1]) < epsilon2) &&
						 (fabs(m[0][0] + m[1][1]+m[2][2]-3) < epsilon2))
					{
						// this singularity is identity matrix so angle = 0
						angle = 0;
						axis.x = 1;
						axis.y = axis.z = 0;

						return;
					}
		
					// otherwise this singularity is angle = 180
					angle = M_PI;

					double xx = (m[0][0] + 1) / 2;
					double yy = (m[1][1] + 1) / 2;
					double zz = (m[2][2] + 1) / 2;
					double xy = (m[0][1] + m[1][0]) / 4;
					double xz = (m[0][2] + m[2][0]) / 4;
					double yz = (m[1][2] + m[2][1]) / 4;

					if ((xx > yy) && (xx > zz)) 
					{ 
						// m[0][0] is the largest diagonal term
						if (xx < epsilon) 
						{
							axis.x = 0;
							axis.y = 0.7071;
							axis.z = 0.7071;
						} 
						else 
						{
							axis.x = sqrt(xx);
							axis.y = xy / axis.x;
							axis.z = xz / axis.x;
						}
					} 
					else 
					if (yy > zz) 
					{ 
						// m[1][1] is the largest diagonal term
						if (yy < epsilon) 
						{
							axis.x = 0.7071;
							axis.y = 0;
							axis.z = 0.7071;
						} 
						else 
						{
							axis.y = sqrt(yy);
							axis.x = xy / axis.y;
							axis.z = yz / axis.y;
						}	
					} 
					else 
					{ 
						// m[2][2] is the largest diagonal term so base result on this
						if (zz < epsilon) 
						{
							axis.x = 0.7071;
							axis.y = 0.7071;
							axis.z = 0;
						} 
						else 
						{
							axis.z = sqrt(zz);
							axis.x = xz / axis.z;
							axis.y = yz / axis.z;
						}
					}
				}
			}

			/**
			 * Get the determinant of this matrix.
			 */
			vec_t determinant() const
			{
				vec_t fCofactor00 = m[1][1]*m[2][2] - m[1][2]*m[2][1];
				vec_t fCofactor10 = m[1][2]*m[2][0] - m[1][0]*m[2][2];
				vec_t fCofactor20 = m[1][0]*m[2][1] - m[1][1]*m[2][0];

        		return m[0][0]*fCofactor00 + m[0][1]*fCofactor10 + m[0][2]*fCofactor20;
			}

			/**
			 * Output matrix data.
			 */
			void cout() const
			{
				 printf("%f %f %f\n", m[0][0], m[1][0], m[2][0]);
				 printf("%f %f %f\n", m[0][1], m[1][1], m[2][1]);
				 printf("%f %f %f\n", m[0][2], m[1][2], m[2][2]);
			}
	};
}

#endif

