#include <stdio.h>
#include <math.h>
#include <fstream>
#include <mpi.h>
#define VARIABLE_A 0.
#define VARIABLE_B 1.
#define STEPS_X 8
#define STEPS_T 950

double stepFunction(double w_t_x_plus, double w_t_x_minus, double w_t_x, double tau, double h);
double correctFunc(double x, double t);


int getRank(int rank, int size, int i) {
	if (rank + i < 0)
		return size - 2;
	return (rank + i) % (size - 1);
}

int main(int argc, char* argv[])
{
	int rank, size;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	const double stepX = 1. / (STEPS_X - 1);
	const double stepT = 1. / (STEPS_T - 1);

	double correctMatrix[STEPS_T][STEPS_X] = { 0 };
	double matrix[STEPS_T][STEPS_X] = { 0 };
	int root = size - 1;
	double currentX = 0.0;
	double currentT = 0.0;
	if (rank == root) {
		//Крайові
		for (int i = 0; i < STEPS_T; i++)
		{
			currentX = 0.0;
			for (int j = 0; j < STEPS_X; j++)
			{
				correctMatrix[i][j] = correctFunc(currentX, currentT);
				currentX += stepX;
			}
			currentT += stepT;
		}
		currentX = 0.0;
		for (int i = 0; i < STEPS_X; i++)
		{
			matrix[0][i] = correctFunc(currentX, 0.0);
			currentX += stepX;
		}

		currentT = 0.0;
		for (int i = 0; i < STEPS_T; i++)
		{
			matrix[i][0] = correctFunc(0, currentT);
			matrix[i][STEPS_X - 1] = correctFunc(1, currentT);
			currentT += stepT;
		}
	}
	MPI_Bcast(matrix[0], STEPS_X, MPI_DOUBLE, root, MPI_COMM_WORLD);
	if (rank == root) {
		for (int i = 1; i < STEPS_T; ++i) {
			MPI_Send(&(matrix[i][0]), 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
			MPI_Send(&(matrix[i][STEPS_X-1]), 1, MPI_DOUBLE, (STEPS_X-3)%(size-1), STEPS_X-1, MPI_COMM_WORLD);
		}
		for (int i = 1; i < STEPS_T; ++i) {
			for (int j = 1; j < STEPS_X-1; ++j) {
				MPI_Recv(&(matrix[i][j]), 1, MPI_DOUBLE, MPI_ANY_SOURCE, j, MPI_COMM_WORLD, &status);
			}
		}
		std::ofstream emplFile("matrix.txt");
		std::ofstream correctFile("correctMatrix.txt");
		for (int i = 0; i < STEPS_T; i++)
		{
			for (int j = 0; j < STEPS_X; j++)
			{
				correctFile << correctMatrix[i][j] << '\t';
				emplFile << matrix[i][j] << '\t';
			}
			correctFile << "\n";
			emplFile << "\n";
		}
		correctFile.close();
		emplFile.close();
	}
	else {
		if(rank < STEPS_X-1)
		for (int j = rank+1; j < STEPS_X-1; j+=(size-1)) {
			matrix[1][j] = stepFunction(matrix[0][j + 1],
				matrix[0][j - 1], matrix[0][j], stepT, stepX);
			MPI_Send(&(matrix[1][j]), 1, MPI_DOUBLE, root, j, MPI_COMM_WORLD);
			if(j!=1)
				MPI_Send(&(matrix[1][j]), 1, MPI_DOUBLE, getRank(rank, size, -1), j, MPI_COMM_WORLD);
			if(j!=STEPS_X-2)
				MPI_Send(&(matrix[1][j]), 1, MPI_DOUBLE, getRank(rank, size, + 1), j, MPI_COMM_WORLD);
		}
		for (int i = 2; i < STEPS_T; ++i) {
			for (int j = rank+1; j < STEPS_X - 1; j += (size - 1)) {
				MPI_Recv(&(matrix[i - 1][j - 1]), 1, MPI_DOUBLE, MPI_ANY_SOURCE, j - 1, MPI_COMM_WORLD, &status);
				MPI_Recv(&(matrix[i - 1][j + 1]), 1, MPI_DOUBLE, MPI_ANY_SOURCE, j + 1, MPI_COMM_WORLD, &status);
				matrix[i][j] = stepFunction(matrix[i-1][j + 1],
					matrix[i-1][j - 1], matrix[i-1][j], stepT, stepX);
				MPI_Send(&(matrix[i][j]), 1, MPI_DOUBLE, root, j, MPI_COMM_WORLD);
				if (j != 1 && i!=STEPS_T-1)
					MPI_Send(&(matrix[1][j]), 1, MPI_DOUBLE, getRank(rank, size, -1), j, MPI_COMM_WORLD);
				if (j != STEPS_X - 2 && i != STEPS_T - 1)
					MPI_Send(&(matrix[1][j]), 1, MPI_DOUBLE, getRank(rank, size, +1), j, MPI_COMM_WORLD);
			}
		}
	}

	for		(int i = 1; i < STEPS_T; i++)
	{
		for (int j = 1; j < STEPS_X - 1; j++)
		{
			matrix[i][j] = stepFunction(matrix[i - 1][j + 1],
				matrix[i - 1][j - 1], matrix[i - 1][j], stepT, stepX);
		}
	}
	MPI_Finalize();
	return 0;
}
double stepFunction(double w_t_x_plus, double w_t_x_minus, double w_t_x, double tau, double h)
{
	return w_t_x + tau * (w_t_x_plus - 2 * w_t_x + w_t_x_minus) /
		(h * h) + tau * w_t_x * (w_t_x_plus - w_t_x_minus) / (2 * h);
}

double correctFunc(double x, double t)
{
	return (VARIABLE_A - x) / (VARIABLE_B + t);
}