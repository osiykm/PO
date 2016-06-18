#include <stdio.h>
#include <math.h>
#include <fstream>

#define VARIABLE_A 0.
#define VARIABLE_B 1.
#define STEPS_X 8
#define STEPS_T 950

double stepFunction(double w_t_x_plus, double w_t_x_minus, double w_t_x, double tau, double h);
double correctFunc(double x, double t);

int main()
{
	const double stepX = 1. / (STEPS_X - 1);
	const double stepT = 1. / (STEPS_T - 1);

	double correctMatrix[STEPS_T][STEPS_X] = { 0 };
	double matrix[STEPS_T][STEPS_X] = { 0 };

	double currentX = 0.0;
	double currentT = 0.0;
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

	for (int i = 1; i < STEPS_T; i++)
	{
#pragma omp parallel for
		for (int j = 1; j < STEPS_X - 1; j++)
		{
			matrix[i][j] = stepFunction(matrix[i - 1][j + 1],
				matrix[i - 1][j - 1], matrix[i - 1][j], stepT, stepX);
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
	system("pause");
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