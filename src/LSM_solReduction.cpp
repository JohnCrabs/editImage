#include "LSM_solReduction.h"
#include <stdio.h>
#include <stdlib.h>

#define myABS(x) (((x)<0)?-(x):(x))
#define mySQR(x) ((x)*(x))
#define EPSILON 1.e-20

//Perform LSM to find the Reduction Table
void LSM_Reduction(double* tab_x, double* tab_X, double* tab_A, double* tab_a, int n)
{
	/* Equation:
	 * Ax = L => AtAx = AtL => Nx = AtL => x = N_1AtL
	 * where:
	 * x = (double*) tab_A
	 */
	//Create A & At
	double* A;         // 2*n x 8
	double* At;        //   8 x 2*n
	A  = (double*)malloc(2*n*8*sizeof(double));
	At = (double*)malloc(8*2*n*sizeof(double));
	
	//Create L
	double* L; // 2*n x 1
	L = (double*)malloc(n*2*sizeof(double));
	
	//Create AtL = U
	double* U; // 8 x 1
	U = (double*)malloc(8*1*sizeof(double));
	
	//Create N & N_1
	double* N;      // 8 x 8
	double* N_1;    // 8 x 8
	N = (double*)malloc(8*8*sizeof(double));
	N_1 = (double*)malloc(8*8*sizeof(double));
	
	double* g_XY;
	double* p_xy;
	
	g_XY = (double*)malloc(n*2*sizeof(double));
	p_xy = (double*)malloc(n*2*sizeof(double));
	
	copyTable(tab_X, g_XY, 2*n);
	copyTable(tab_x, p_xy, 2*n);
	
	int loop = 0;
	//double so;
	
	
	while (loop < 100) {
		//Set Values to A and L
		for (int i = 0; i < 2*n; i+=2 ) {
			//first row of table A
			A[i*8 + 0] = g_XY[i];                  //a11 -> xi
			A[i*8 + 1] = g_XY[i + 1];              //a12 -> xi
			A[i*8 + 2] = 1;                         //a13 -> xi
			A[i*8 + 3] = 0.0;                       //a21 -> xi
			A[i*8 + 4] = 0.0;                       //a22 -> xi
			A[i*8 + 5] = 0.0;                       //a23 -> xi
			A[i*8 + 6] = -g_XY[i]*p_xy[i];        //a31 -> xi
			A[i*8 + 7] = -g_XY[i + 1]*p_xy[i];    //a32 -> xi
			
			//second row of table A
			A[i*8 +  8] = 0.0;                        //a11 -> yi
			A[i*8 +  9] = 0.0;                        //a12 -> yi
			A[i*8 + 10] = 0.0;                        //a13 -> yi
			A[i*8 + 11] = g_XY[i];                   //a21 -> yi
			A[i*8 + 12] = g_XY[i + 1];               //a22 -> yi
			A[i*8 + 13] = 1;                          //a23 -> yi
			A[i*8 + 14] = -g_XY[i]*p_xy[i + 1];     //a31 -> yi
			A[i*8 + 15] = -g_XY[i + 1]*p_xy[i + 1]; //a32 -> yi
			
			//table L
			L[i] = p_xy[i];      //xi
			L[i+1] = p_xy[i + 1];  //yi
		}
		
		//Reverse table A ---> At
		reverseTab(A, At, 2*n, 8);
		
		/*Debugging*/
		/*FILE* openFile1;
		openFile1 = fopen("output/reductionFold/tables_A_At_L.csv", "w");
		for(int i = 0; i < 2*n; i++) {
			for (int j = 0; j < 8; j++) {
				fprintf(openFile1, "%f;", A[i*8 + j]);
			}
			fprintf(openFile1, "\n");
		}
		fprintf(openFile1, "\n;\n;\n");
		for(int i = 0; i < 8; i++) {
			for (int j = 0; j < 2*n; j++) {
				fprintf(openFile1, "%f;", At[i*2*n + j]);
			}
			fprintf(openFile1g_XY[2*i], "\n");
		}
		fprintf(openFile1, "\n;\n;\n");
		for(int i = 0; i < 2*n; i++) {
			for (int j = 0; j < 1; j++) {
				fprintf(openFile1, "%f;", L[i*1 + j]);
			}
			fprintf(openFile1, "\n");
		}
		fclose(openFile1);*/
		/********/
		
		//Multiply tables AtA ----> N & AtL ----> U
		mmult(At, A, N, 8, 2*n, 8);
		mmult(At, L, U, 8, 2*n, 1);
		
		/*Debugging*/
		/*FILE* openFile2;
		openFile2 = fopen("output/reductionFold/tables_N_U.csv", "w");
		for(int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				fprintf(openFile2, "%25.20f;", N[i*8 + j]);
			}
			fprintf(openFile2, "\n");
		}
		fprintf(openFile2, "\n;\n;\n");
		for(int i = 0; i < 8; i++) {
			for (int j = 0; j < 1; j++) {
				fprintf(openFile2, "%f;", U[i*1 + j]);
			}
			fprintf(openFile2, "\n");
		}
		fclose(openFile2);*/
		/********/
		
		//Inverse N ----> N_1
		Matrix_inversion(N, N_1, 8);
		
		/*Debugging*/
		/*FILE* openFile3;
		openFile3 = fopen("output/reductionFold/tables_N_1.csv", "w");
		for(int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				fprintf(openFile3, "%25.20f;", N_1[i*8 + j]);
			}
			fprintf(openFile3, "\n");
		}
		fclose(openFile3);*/
		/********/
		
		//Multiply N_1U ----> x
		mmult(N_1, U, tab_A, 8, 8, 1);
		/*Debugging*/
		/*FILE* openFile4;
		openFile4 = fopen("output/reductionFold/table_Α.csv", "w");
		for(int i = 0; i < 8; i++) {
			for (int j = 0; j < 1; j++) {
				fprintf(openFile4, "%f;", tab_A[i*1 + j]);
			}
			fprintf(openFile4, "\n");
		}
		fclose(openFile4);*/
		/********/
		
		calculate_A(tab_A, tab_a);
		/*double* buf_p_xy;
		buf_p_xy = (double*)malloc(2*n*sizeof(double));
		copyTable(p_xy, buf_p_xy, 2*n);*/
		
		for(int i = 0; i < n; i++) {
			//new_XY(p_xy[2*i], p_xy[2*i+1], tab_A, g_XY[2*i], g_XY[2*i+1]);
			new_XY(g_XY[2*i], g_XY[2*i+1], tab_a, p_xy[2*i], p_xy[2*i+1]);
		}
		loop++;
	}
	//Clear Memory
	free(A);
	free(At);
	free(L);
	free(U);
	free(N);
	free(N_1);
	free(p_xy);
	free(g_XY);
}

//Reverse a table
void reverseTab(const double* A, double* At, int row, int col)
{
	double* buffer;
	buffer = (double*)malloc(row*col*sizeof(double));
	
	for (int i = 0; i < row*col; i++)
		buffer[i] = A[i];
	
	for (int i = 0; i < row; i++)
		for(int j = 0; j < col; j++)
			At[j*row + i] = buffer[i*col + j];
	free(buffer);
}

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

//Matrix inversion
int Matrix_inversion(double *ata, double *ata_inv, int n) 
{
	double *tmpa, *b;
	int *l;
	int r_value;
	register int i, j, k;

	tmpa = (double*)malloc(n*n*sizeof(double));
	b = (double*)malloc(n*sizeof(double));
	l = (int*)malloc(n*sizeof(int));
	r_value=1;
	for (i=0; i<n; i++) {
	for (j=0; j<n; j++) {
		for (k=0; k<n; k++)
			tmpa[j*n+k]=ata[j*n+k];
		b[j]=0.0;
	}
	b[i]=1.0;
		r_value*=Cholesky_decomposition(tmpa, b, n);
		for (j=0; j<n; j++)
			ata_inv[j*n+i]=b[j];
	}
	
	free(tmpa);
	free(b);
	free(l);
	
	return r_value;
}

//Cholesky
int Cholesky_decomposition(double *a, double *b, int n) 
{
	double s;
	register int i, j, k;

	for (i=0; i<n; i++) {
		s=0.0;
		if (i)
			for (j=0; j<i; j++)
				s+=mySQR(a[i*n+j]);
		a[i*n+i]=mySQRT(a[i*n+i]-s);
		if (myABS(a[i*n+i])<EPSILON)
			return 0;
		if (i<n-1)
			for (j=i+1; j<n; j++) {
				s=0.0;
				if (i)
					for (k=0; k<i; k++)
						s+=a[i*n+k]*a[j*n+k];
				a[j*n+i]=(a[j*n+i]-s)/a[i*n+i];
			}
	}
	b[0]/=a[0*n+0];
	for (k=1; k<n; k++) {
		s=0.0;
		for (j=0; j<k; j++)
			s+=a[k*n+j]*b[j];
		b[k]=(b[k]-s)/a[k*n+k];
	}
	b[n-1]/=a[(n-1)*n+n-1];
	for (i=0; i<n-1; i++) {
		s=0.0;
		for (j=n-i-1; j<n; j++)
            s+=a[j*n+n-i-2]*b[j];
		b[n-i-2]=(b[n-i-2]-s)/a[(n-i-2)*n+n-i-2];
	}
	return 1;
}

//SQRT
double mySQRT(double x) 
{
	double px, a;

	a=px=x;
	x/=2.0;
	while (myABS(x-px)>EPSILON) {
		px=x;
		x=(px+a/px)/2;
	}
	return x;
}

//Copy Table
void copyTable(const double* from, double* to, const int size)
{
	for (int i = 0; i < size; i++)
		to[i] = from[i];
}

//Calculate new XY
void new_XY(double &X, double &Y, double* tab_A, double x, double y)
{
	double A, B, C;
	A = tab_A[0]*x + tab_A[1]*y + tab_A[2];
	B = tab_A[3]*x + tab_A[4]*y + tab_A[5];
	C = tab_A[6]*x + tab_A[7]*y + 1;
	
	X = A/C;
	Y = B/C;
}

//Calculate table A
void calculate_A(const double* tab_a, double* tab_A)
{
	/*
	 * tab_a[0] a11
	 * tab_a[1] a12
	 * tab_a[2] a13
	 * tab_a[3] a21
	 * tab_a[4] a22
	 * tab_a[5] a23
	 * tab_a[6] a31
	 * tab_a[7] a32
	 */
	
	double div = tab_a[0]*tab_a[4]-tab_a[3]*tab_a[1];
	
	tab_A[0] =  (tab_a[4]-tab_a[5]*tab_a[7])/div;
	tab_A[1] = -(tab_a[1]-tab_a[2]*tab_a[7])/div; 
	tab_A[2] =  (tab_a[1]*tab_a[5]-tab_a[2]*tab_a[4])/div;
	tab_A[3] = -(tab_a[3]-tab_a[5]*tab_a[6])/div;
	tab_A[4] =  (tab_a[0]-tab_a[2]*tab_a[6])/div;
	tab_A[5] = -(tab_a[0]*tab_a[5]-tab_a[2]*tab_a[3])/div;
	tab_A[6] =  (tab_a[3]*tab_a[7]-tab_a[4]*tab_a[6])/div;
	tab_A[7] = -(tab_a[0]*tab_a[7]-tab_a[1]*tab_a[6])/div;
}