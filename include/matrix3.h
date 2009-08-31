/*
    Copyright (C) 2008-2009 Rômulo Fernandes Machado <romulo@castorgroup.net>

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

#ifndef _MATRIX3_H_
#define _MATRIX3_H_ 

#include "prerequisites.h"
#include "vector3.h"

namespace k
{
	/**
	 * \brief Handles 3x3 matrixes. Still WIP
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
			 * Initialise our matrix. Keep in mind
			 * that we are using right hand coordinate
			 * system and therefore our matrix is column
			 * major.
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

			matrix3& operator= (matrix3 mat)
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
			matrix3& operator*= (matrix3 mat)
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
			matrix3 operator* (matrix3 mat)
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
			void toAxisAngle(vec_t& angle, vector3& axis)
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
			vec_t determinant()
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

