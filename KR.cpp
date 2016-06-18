#include <mpi.h>
#include <time.h> 
#include <stdlib.h>
#include <stdio.h>
#define ARRAY_SIZE 10
#define MAX_NUMBER 10

int main(int argc, char* argv[]) {
	int rank, size;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	srand(time(NULL));
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int root = 0;
	int mas[ARRAY_SIZE] = {0};
	if (rank == root) {
		printf("first array\n");
		for (int i = 0; i < ARRAY_SIZE; ++i) {
			mas[i] = rand() % MAX_NUMBER;
			printf("%3d", mas[i]);
		}
		printf("\n");
	}
	MPI_Bcast(mas, ARRAY_SIZE, MPI_INT, root, MPI_COMM_WORLD);
	int piece = ARRAY_SIZE / size;
	int *temp = (int*)malloc(piece * sizeof(int));
	for (int i = 0; i < piece; ++i) {
		temp[i] = 0;
		for (int j = 0; j < ARRAY_SIZE; ++j)
			if (mas[piece*rank + i] == mas[j])
				temp[i]++;
	}
	if (rank == root && (ARRAY_SIZE - piece*size) > 0) {
		int *temp2 = (int*)malloc((ARRAY_SIZE-piece*size) * sizeof(int));
		for (int i = 0; i < (ARRAY_SIZE - piece*size); ++i) {
			temp2[i] = 0;
			for (int j = 0; j < ARRAY_SIZE; ++j) 
				if (mas[(ARRAY_SIZE - piece*size) + i] == mas[j])
					temp2[i]++;
		}
		for (int i = piece*size; i < ARRAY_SIZE; ++i) {
			mas[i] = temp2[i - piece*size];
		}
		delete temp2;
	}
	MPI_Gather(temp, piece, MPI_INT, mas, piece, MPI_INT, root, MPI_COMM_WORLD);
	delete temp;
	if (rank == root) {
		printf("second array\n");
		for (int i = 0; i < ARRAY_SIZE; ++i) {
			printf("%3d", mas[i]);
		}
		printf("\n");
	}
	MPI_Finalize();
}
