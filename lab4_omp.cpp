#include <string>
#include <omp.h>    
#include <time.h>  
using namespace std;

#define MATRIX_SIZE 3
#define DEL 40

void printMatrix(double matrix[MATRIX_SIZE][MATRIX_SIZE+1]);

int main() {
	srand(time(NULL));
	double matrix[MATRIX_SIZE][MATRIX_SIZE + 1];
#pragma omp parallel for 
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		matrix[i][MATRIX_SIZE] = 0;
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			matrix[i][j] = (double)((rand() % DEL + (rand() % (i + 1) * (j + 1))/10. + (double)(i + 1) / (j + 1)*10.)) / 10.;
		}
	}
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		double tempX = (rand() % 200 - 100) / 10.;
		printf("x%d = %6.2f\n", i, tempX);
	#pragma omp parallel for 
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			matrix[j][MATRIX_SIZE] += matrix[j][i] * tempX;
		}
	}
	printMatrix(matrix);
	
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		for (int j = i+1; j < MATRIX_SIZE; ++j) {
			double del = matrix[j][i] / matrix[i][i];
			#pragma omp parallel for 
			for (int k = i; k <= MATRIX_SIZE; ++k) {
				matrix[j][k] -= (matrix[i][k]*del);
			}
		}
	}
	printMatrix(matrix);
	double answer[MATRIX_SIZE];
	
	for (int i = MATRIX_SIZE-1; i >=0 ; --i) {
		answer[i] = matrix[i][MATRIX_SIZE] / matrix[i][i];
		#pragma omp parallel for 
		for (int j = 0; j < i; ++j) {
			matrix[j][MATRIX_SIZE] -= (answer[i]*matrix[j][i]);
		}
	}
	for (int i = 0; i < MATRIX_SIZE; ++i) {
		printf("x%d = %6.3f\n", i, answer[i]);
	}
	getchar();
}


void printMatrix(double matrix[MATRIX_SIZE][MATRIX_SIZE+1]) {

	for (int i = 0; i < MATRIX_SIZE; ++i) {
		for (int j = 0; j < MATRIX_SIZE; ++j) {
			printf("%6.2f", matrix[i][j]);
		}
		printf(" = %6.2f\n", matrix[i][MATRIX_SIZE]);

	}

	printf("\n");
}