#include <mpi.h>
#include <stdlib.h>
#include <string>
#include <stdio.h> 
#include <conio.h> 
// Размер массива
#define ARRAY_SIZE 20
using namespace System;

int mas[ARRAY_SIZE][ARRAY_SIZE];

void Print() {
	for (int i = 0; i < ARRAY_SIZE; ++i) {
		for (int j = 0; j < ARRAY_SIZE; ++j) {
			printf("%4d", mas[j][i]);
		}
		printf("\n");
	}
	for (int i = 0; i < ARRAY_SIZE * 4; ++i) {
		printf("_");
	}
	printf("\n");
}
int main(int argc, char* argv[]) {
	int rank, size;
	int temp[ARRAY_SIZE];
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (rank == 0) {
		int piece; //сколько один поток будет обрабатывать столбцов
		if (size > ARRAY_SIZE) //проверяем сколько потоков
			piece = 1; //если потоков больше чем столбцов 
		else
			if (size != 1)
			piece = ARRAY_SIZE / (size - 1);  //если потоков меньше чем столбцов
			else piece = ARRAY_SIZE + 1;

		Console::WriteLine("piece: " + piece );
		for (int i = 0; i < piece; ++i)
			for (int j = 1; j < size; ++j) {
				MPI_Send(mas[i*(size - 1) + j-1], ARRAY_SIZE, MPI_INT, j, piece - i - 1, MPI_COMM_WORLD);
			}
		for (int i = piece*(size - 1); i < ARRAY_SIZE; ++i) {
			for (int j = 0; j < ARRAY_SIZE; ++j)
				mas[i][j] = i-j;
			}
		for (int i = 0; i < piece; ++i)
			for (int j = 1; j < size; ++j) {
				MPI_Recv(temp, ARRAY_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				for(int k = 0; k < ARRAY_SIZE; ++k) {
					mas[(status.MPI_SOURCE - 1) + (size-1)*(piece - 1 - status.MPI_TAG)][k] = temp[k];
				}
			}
		Print();
		for (int i = 0; i < piece; ++i)
			for (int j = 1; j < size; ++j) {
				MPI_Send(mas[i*(size - 1) + j - 1], ARRAY_SIZE, MPI_INT, j, piece - i - 1, MPI_COMM_WORLD);
			}
		for (int i = 0; i < piece; ++i)
			for (int j = 1; j < size; ++j) {
				MPI_Recv(temp, ARRAY_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				for (int k = 0; k < ARRAY_SIZE; ++k) {
					mas[(status.MPI_SOURCE - 1) + (size - 1)*(piece - 1 - status.MPI_TAG)][k] = temp[k];
				}
			}	
		for (int i = piece*(size - 1); i < ARRAY_SIZE; ++i) {
				for (int j = 0; j < ARRAY_SIZE/2; ++j)
					mas[i][ARRAY_SIZE-1-j] = mas[i][j];
			}
		Print();

	}
	else {
		do {
			MPI_Recv(temp, ARRAY_SIZE, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			for (int i = 0; i < ARRAY_SIZE; ++i)
				temp[i] = (rank - 1) + (size - 1)*(ARRAY_SIZE/(size-1) - 1 - status.MPI_TAG)-i;
			MPI_Send(temp, ARRAY_SIZE, MPI_INT, 0, status.MPI_TAG, MPI_COMM_WORLD);
		} while (status.MPI_TAG);
		do {
			MPI_Recv(temp, ARRAY_SIZE, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			for (int i = 0; i < ARRAY_SIZE/2; ++i)
				temp[ARRAY_SIZE-1-i] = temp[i];
			MPI_Send(temp, ARRAY_SIZE, MPI_INT, 0, status.MPI_TAG, MPI_COMM_WORLD);
		} while (status.MPI_TAG);
	}
	MPI_Finalize();
}

