#pragma once

//Perform LSM to find the Reduction Table
extern void LSM_Reduction(double* tab_x, double* tab_X, double* tab_A, double* tab_a, int n);

//Reverse a table
extern void reverseTab(const double* A, double* At, int row, int col);

//Multiply two tables
extern void mmult(const double* A, const double* B, double* C, int n, int m, int t);

//Matrix Inversion
extern int Matrix_inversion(double *ata, double *ata_inv, int n);

//Cholesky
extern int Cholesky_decomposition(double *a, double *b, int n);

//SQRT
extern double mySQRT(double x);

//Copy Table
extern void copyTable(const double* from, double* to, const int size);

//Calculate new XY
extern void new_XY(double &X, double &Y, double* tab_A, double x, double y);

//Calculate A
void calculate_A(const double* tab_a, double* tab_A);