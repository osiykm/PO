#include <stdio.h>
#include <math.h>
#include <fstream>
#define VARIABLE_A 0.
#define VARIABLE_B 1.
#define STEPS_X 8
#define STEPS_T 950

double f(double w_t_x, double w_t1_x_plus, double w_t1_x_minus, double w_t1_x, double tau, double h);

double df_dw_minus(double w_t1_x, double tau, double h);
double df_dw(double w_t_x_plus, double w_t_x_minus, double tau, double h);
double df_dw_plus(double w_t1_x, double tau, double h);

double correctFunc(double x, double t);

int main()
{
	const double stepX = 1. / (STEPS_X - 1);
	const double stepT = 1. / (STEPS_T - 1);

	double correctMatrix[STEPS_T][STEPS_X] = { 0 };
	double matrix[STEPS_T][STEPS_X] = { 0 };
	double yakobi[STEPS_X - 2][STEPS_X - 1] = { 0 };
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
	//
	double w_t_x, w_t1_x, w_t1_minus, w_t1_plus;

	//#pragma omp paralel
	for (int i = 1; i < STEPS_T; i++)
	{
		for (int j = 1; j<STEPS_X - 1; j++)
		{
			w_t1_x = matrix[i - 1][j];
			w_t1_plus = matrix[i - 1][j + 1];
			w_t1_minus = matrix[i - 1][j - 1];
			if (i == 1)
				w_t_x = w_t1_x;
			else
				w_t_x = matrix[i - 2][j];

			yakobi[j - 1][STEPS_X - 2] = f(w_t_x, w_t1_plus, w_t1_minus, w_t1_x, stepT, stepX);

			if (j>0)
				yakobi[j - 1][j - 2] = df_dw_minus(w_t1_x, stepT, stepX);
			else
				yakobi[j - 1][STEPS_X - 2] -= df_dw_minus(w_t1_x, stepT, stepX) * w_t1_minus;

			yakobi[j - 1][j - 1] = df_dw(w_t1_plus, w_t1_minus, stepT, stepX);

			if (j<STEPS_X - 2)
				yakobi[j - 1][j] = df_dw_plus(w_t1_x, stepT, stepX);
			else
				yakobi[j - 1][STEPS_X - 2] -= df_dw_plus(w_t1_x, stepT, stepX) * w_t1_plus;
		}
		for (int i = 0; i < STEPS_X - 2; i++)
		{
			for (int j = STEPS_X - 2; j >= i; j--)
			{
				yakobi[i][j] /= yakobi[i][i];

				for (int k = 0; k < STEPS_X - 2; k++)
					if (i != k)
						yakobi[k][j] -= (yakobi[i][j] * yakobi[k][i]);
			}
		}

		for (int j = 1; j<STEPS_X - 1; j++)  //нельзя так как код берет предыдущие значение
			matrix[i][j] = matrix[i - 1][j] + yakobi[j - 1][STEPS_X - 2];
	}
	//
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

double f(double w_t_x, double w_t1_x_plus, double w_t1_x_minus, double w_t1_x, double tau, double h)
{
	return (w_t1_x - w_t_x) / tau - (w_t1_x_plus - 2.*w_t1_x + w_t1_x_minus) / (h*h) - w_t1_x*(w_t1_x_plus - w_t1_x_minus) / (2 * h);
}

double df_dw_minus(double w_t1_x, double tau, double h) {
	return -1. / (h*h) + w_t1_x / (2. * h);
}
double df_dw(double w_t_x_plus, double w_t_x_minus, double tau, double h) {
	return 1. / tau + 2. / (h*h) - (w_t_x_plus - w_t_x_minus) / (2.*h);
}
double df_dw_plus(double w_t1_x, double tau, double h) {
	return -1. / (h*h) - w_t1_x / (2. * h);
}

double correctFunc(double x, double t)
{
	return (VARIABLE_A - x) / (VARIABLE_B + t);
};