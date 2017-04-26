## Synopsis

A tool to make simple image editing from a photogrametric perspective.

## Code Example

//Multiply two tables A & B and creates C
void mmult(const double* A, const double* B, double* C, int n, int m, int t)
{
	/* (n x m) X (m x t) = n x t
	 * 
	 *                       <--- t  --->
	 *     <---- m ---->   | | 1  5   9 |
	 *  | | 1  2  3  4 | X | | 2  6  10 |
	 *  n | 5  6  7  8 |   m | 3  7  11 |
	 *  | | 9 10 11 12 |   | | 4  8  12 |
	 * 
	 */
	 
	for (int i = 0; i < n; i++)
		for (int j = 0; j < t; j++) {
			C[i*t + j] = 0.0;
			for (int k = 0; k < m; k++)
				C[i*t + j] += A[i*m + k] * B[k*t + j];
		}
}

## Motivation

This software solves all photogrametric exercises for the seventh semester of http://www.survey.ntua.gr/en/

## Installation

1)Open a terminal in the folder with the makefile.
2)Run the command make.

## Tests

You can find all result at "voithima_logismikoy_editImage.pdf" in greek.

## Contributors

Currently only me.

## License
Copyright (C) Dec 2016 John Crabs <kavouras.ioannis@gmail.com>

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




