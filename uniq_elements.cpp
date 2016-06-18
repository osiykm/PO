#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>

#define ARRAY_SIZE 100
#define MAX_NUMBER 10


int mas[ARRAY_SIZE] = { 0 };

void add(int *a, int n , int nMas) {
	for (int i = 0; i < n; ++i) {
		mas[i + nMas] = a[i];
	}
}
int main(int argc, char* argv[]) {
	srand(time(NULL));
	int rank, size;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int root = 0;
	int piece = ARRAY_SIZE / size;
	if (rank == root) {
		for (int i = 0; i < ARRAY_SIZE; ++i)
			mas[i] = rand() % MAX_NUMBER;
		// Узнаем по сколько скидывать кожному процессу
	}
	int *temp = (int*)malloc(piece * sizeof(int));
	MPI_Scatter(mas, piece, MPI_INT, temp, piece, MPI_DOUBLE, root, MPI_COMM_WORLD);
	std::sort(temp, temp + piece);
	int * end = std::unique(temp, temp + piece);
	int n = 0;
	while (true) {
		if (&(temp[n]) == end)
			break;
		n++;
	}
	if (rank == root) {
		int nMas = 0;
		add(temp, n, nMas);
		nMas += n;
		for (int i = 0; i < size - 1; ++i) {
			MPI_Recv(temp, piece, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			add(temp, status.MPI_TAG, nMas);
			nMas += status.MPI_TAG;
		}
		add(&(mas[size*piece]), ARRAY_SIZE - size*piece, nMas);
		nMas += (ARRAY_SIZE - size*piece);
		std::sort(mas, mas + nMas);
		int * end = std::unique(mas, mas + nMas);
		for (int i = 0; &(mas[i]) != end; ++i) {
			printf("%d\n", mas[i]);
		}
	}
	else {
		MPI_Send(temp, n, MPI_INT, root, n, MPI_COMM_WORLD);
	}
	MPI_Finalize();
}