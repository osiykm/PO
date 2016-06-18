#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define VECTOR_SIZE 10
#define MAX_NUMBER 100
int main(int argc, char* argv[]) {
	int rank, size;
	srand(time(NULL));
	int A[VECTOR_SIZE] = { 0 };
	int B[VECTOR_SIZE] = { 0 };
	int C[VECTOR_SIZE] = { 0 };
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (rank == 0) {
		for (int i = 0; i < VECTOR_SIZE; ++i) {
			A[i] = rand() % MAX_NUMBER;
			B[i] = rand() % MAX_NUMBER;
		}
	}
	MPI_Bcast(A, VECTOR_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(B, VECTOR_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
	for (int i = rank; i < VECTOR_SIZE; i += size) {
		C[i] = A[i] + B[i];
		if (rank != 0)
			MPI_Send(&(C[i]), 1, MPI_INT, 0, i, MPI_COMM_WORLD);
	}
	if (rank == 0) {
		int n = 0;
		for (int i = 0; i < VECTOR_SIZE; ++i)
			if (i != n)
				MPI_Recv(&(C[i]), 1, MPI_INT, MPI_ANY_SOURCE, i, MPI_COMM_WORLD, &status);
			else
				n += size;
		printf("%5s%5s%5s\n", "A", "B", "C");
		for (int i = 0; i < VECTOR_SIZE; ++i) {
			printf("%5d%5d%5d\n", A[i], B[i], C[i]);
		}
	}
	MPI_Finalize();
}