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

#ifndef _MATRIX4_H_
#define _MATRIX4_H_ 

#include "prerequisites.h"
#include "vector3.h"

namespace k
{
	class matrix4
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

			// Initialise our matrix. Keep in mind
			// that we are using right hand coordinate
			// system and therefore our matrix is column
			// major.
			matrix4(vector3 x, vector3 y, vector3 z, vector3 w)
			{
				m[0][0] = x.x;	
				m[0][1] = x.y;	
				m[0][2] = x.z;	

				m[1][0] = y.x;	
				m[1][1] = y.y;	
				m[1][2] = y.z;	

				m[2][0] = z.x;	
				m[2][1] = z.y;	
				m[2][2] = z.z;	

				m[3][0] = w.x;	
				m[3][1] = w.y;	
				m[3][2] = w.z;	
			}

			// Get the transpose of this matrix
			matrix4 transpose()
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

			matrix4& operator*= (matrix4 mat)
			{
				matrix4 temp;
				temp = *this * mat;
				*this = temp;
				
				return *this;
			}

			matrix4 operator* (matrix4 mat)
			{
				matrix4 result;
				for (unsigned int row = 0; row < 4; row++)
				{
					for (unsigned int col = 0; col < 4; col++)
					{
						result.m[row][col] = m[row][0]*mat.m[0][col] +
							m[row][1]*mat.m[1][col] +
							m[row][2]*mat.m[2][col];
					}
				}

				return result;
			}

			// This Also came from Ogre3D
			vec_t	MINOR(const matrix4& m, const size_t r0, const size_t r1, const size_t r2, 
								const size_t c0, const size_t c1, const size_t c2)
			{
        
				return m.m[r0][c0] * (m.m[r1][c1] * m.m[r2][c2] - m.m[r2][c1] * m.m[r1][c2]) -
					m.m[r0][c1] * (m.m[r1][c0] * m.m[r2][c2] - m.m[r2][c0] * m.m[r1][c2]) +
					m.m[r0][c2] * (m.m[r1][c0] * m.m[r2][c1] - m.m[r2][c0] * m.m[r1][c1]);
			}

			vec_t determinant()
			{
				return m[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) -
					m[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) +
					m[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) -
					m[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2);
			}
	};
}

#endif

