#include <string>
#include <time.h> 
#include <mpi.h>
#include <Windows.h>
using namespace std;

#define ARRAY_SIZE 3
#define DEL 40

void printMatrix(double matrix[ARRAY_SIZE][ARRAY_SIZE + 1]);

int main(int argc, char* argv[]) {
	int rank, size;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	srand(time(NULL));
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	double matrix[ARRAY_SIZE][ARRAY_SIZE + 1];
	double buf[ARRAY_SIZE * 2];
	if (rank == 0) {
		//Иницилизация матрицы
		for (int i = 0; i < ARRAY_SIZE; ++i) {
			matrix[i][ARRAY_SIZE] = 0;
			for (int j = 0; j < ARRAY_SIZE; ++j) {
				matrix[i][j] = (rand()% 99 + 1)/10.;
			}
		}
		for (int i = 0; i < ARRAY_SIZE; ++i) {
			double tempX = (rand() % 200 - 100) / 10.;
			printf("x%d = %6.1f\n", i, tempX);
			for (int j = 0; j < ARRAY_SIZE; ++j) {
				matrix[j][ARRAY_SIZE] += matrix[j][i] * tempX;
			}
		}
		//конец забивки
		printMatrix(matrix);
		for (int i = 0; i < ARRAY_SIZE; ++i) {
			for (int j = i + 1; j < ARRAY_SIZE; ++j) {
				double del = matrix[j][i] / matrix[i][i];
				MPI_Send(&del, 1, MPI_DOUBLE, ((i*(ARRAY_SIZE) +j) % (size-1)+1), 1, MPI_COMM_WORLD);
				MPI_Send(&(matrix[i][i]), 
					ARRAY_SIZE - i + 1, 
					MPI_DOUBLE, 
					((i*(ARRAY_SIZE)+j) % (size - 1) + 1), 
					1, 
					MPI_COMM_WORLD);
				MPI_Send(&(matrix[j][i]), 
					ARRAY_SIZE - i + 1,
					MPI_DOUBLE, 
					((i*(ARRAY_SIZE)+j) % (size - 1) + 1), 
					1, 
					MPI_COMM_WORLD);
				/*for (int k = i; k <= ARRAY_SIZE; ++k) {
					matrix[j][k] -= (matrix[i][k] * del);
				}*/
			}
			for (int j = i + 1; j < ARRAY_SIZE; ++j) {
				MPI_Recv(&(matrix[j][i]), ARRAY_SIZE - i + 1, MPI_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
			}
		}
		
		printMatrix(matrix);
		double answer[ARRAY_SIZE];
		for (int i = ARRAY_SIZE - 1; i >= 0; --i) {
			answer[i] = matrix[i][ARRAY_SIZE] / matrix[i][i];
			for (int j = 0; j < i; ++j) {
				matrix[j][ARRAY_SIZE] -= (answer[i] * matrix[j][i]);
			}
		}
		for (int i = 0; i < ARRAY_SIZE; ++i) {
			printf("x%d = %6.3f\n", i, answer[i]);
		}
		for (int i = 1; i < size; ++i) {
			MPI_Send(matrix[0], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
		}
	}
	else {
		while (true) {
			double del;
			MPI_Recv(&del, 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			if (status.MPI_TAG == 0)
				break;
			MPI_Recv(matrix[0], ARRAY_SIZE + 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			int count;
			MPI_Get_count(&status, MPI_DOUBLE, &count);
			MPI_Recv(matrix[1], ARRAY_SIZE + 1, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		
			for (int i = 0; i < count; ++i) {
				matrix[1][i] -= (del*matrix[0][i]);
			}  

			MPI_Send(matrix[1], count, MPI_DOUBLE, 0, status.MPI_TAG, MPI_COMM_WORLD);
		}
	}
	MPI_Finalize();
}

void printMatrix(double matrix[ARRAY_SIZE][ARRAY_SIZE + 1]) {
	for (int i = 0; i < ARRAY_SIZE; ++i) {
		for (int j = 0; j < ARRAY_SIZE; ++j) {
			printf("%6.2f", matrix[i][j]);
		}
		printf(" = %6.2f\n", matrix[i][ARRAY_SIZE]);

	}

	printf("\n");
}