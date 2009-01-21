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

#ifndef _MATRIX3_H_
#define _MATRIX3_H_ 

#include "prerequisites.h"
#include "vector3.h"

namespace k
{
	class matrix3
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

			// Initialise our matrix. Keep in mind
			// that we are using right hand coordinate
			// system and therefore our matrix is column
			// major.
			matrix3(vector3 x, vector3 y, vector3 z)
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
			}

			// Get the transpose of this matrix
			matrix3 transpose()
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

			matrix3& operator*= (matrix3 mat)
			{
				matrix3 temp;
				temp = *this * mat;
				*this = temp;
				
				return *this;
			}

			matrix3 operator* (matrix3 mat)
			{
				matrix3 result;
				for (unsigned int row = 0; row < 3; row++)
				{
					for (unsigned int col = 0; col < 3; col++)
					{
						result.m[row][col] = m[row][0]*mat.m[0][col] +
							m[row][1]*mat.m[1][col] +
							m[row][2]*mat.m[2][col];
					}
				}

				return result;
			}

			vec_t determinant()
			{
				vec_t fCofactor00 = m[1][1]*m[2][2] - m[1][2]*m[2][1];
				vec_t fCofactor10 = m[1][2]*m[2][0] - m[1][0]*m[2][2];
				vec_t fCofactor20 = m[1][0]*m[2][1] - m[1][1]*m[2][0];

        		return m[0][0]*fCofactor00 + m[0][1]*fCofactor10 + m[0][2]*fCofactor20;
			}

			void cout() const
			{
				std::cout << m[0][0] << "\t" << m[1][0] << "\t" << m[2][0] << std::endl;
				std::cout << m[0][1] << "\t" << m[1][1] << "\t" << m[2][1] << std::endl;
				std::cout << m[0][2] << "\t" << m[1][2] << "\t" << m[2][2] << std::endl;
				std::cout << m[0][3] << "\t" << m[1][3] << "\t" << m[2][3] << std::endl;
			}
	};
}

#endif

