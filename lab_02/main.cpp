#include<iostream>
#include<stdio.h>
#include<string>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <intrin.h>

#pragma intrinsic(__rdtsc)

using namespace std;

/*
static inline unsigned long long int tick()
{
	unsigned long long int d;
	__asm__ __volatile__("rdtsc" : "=A" (d));
	return d;
}
*/


int** new_m(int n, int m)
{
	int **matrix = (int**)malloc(n* sizeof(int*));
	for (int i = 0; i <= n; i++)
	{
		matrix[i] = (int*)malloc(m * sizeof(int));
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			matrix[i][j] = NULL;
		}
	}

	return matrix;
}

int** mult_m(int** matrix1, int n1, int m1, int** matrix2, int n2, int m2)
{
	int** result = NULL;
	if (matrix1 && matrix2 && m1 == n2)
	{
		result = new_m(n1, m2);
		for (int i = 0; i < n1; i++)
			for (int j = 0; j < m2; j++)
				for (int k = 0; k < m1; k++)
					result[i][j] += matrix1[i][k] * matrix2[k][j];
	}

	return result;
}

int** Vinograd(int** matrix1, int n1, int m1, int** matrix2, int n2, int m2)
{
	int* mulH = (int*)malloc(n1 * sizeof(int));
	for (int i = 0; i < n1; i++)
		mulH[i] = 0;

	int* mulV = (int*)malloc(m2 * sizeof(int));
	for (int i = 0; i < m2; i++)
		mulV[i] = 0;

	int** result = new_m(n1, m2);

	for (int i = 0; i < n1; i++)
	{
		for (int j = 0; j < m1 / 2; j++)
		{
			mulH[i] = mulH[i] + matrix1[i][j * 2] * matrix1[i][j * 2 + 1];
		}
	}

	for (int i = 0; i < m2; i++)
	{
		for (int j = 0; j < n2 / 2; j++)
		{
			mulV[i] = mulV[i] + matrix2[j * 2][i] * matrix2[j * 2 + 1][i];
		}
	}

	for (int i = 0; i < n1; i++)
	{
		for (int j = 0; j < m2; j++)
		{
			result[i][j] = -mulH[i] - mulV[j];
			for (int k = 0; k < m1 / 2; k++)
			{
				result[i][j] = result[i][j] + (matrix1[i][2 * k + 1]+ matrix2[2 * k][j]) * (matrix1[i][2 * k] + matrix2[2 * k + 1][j]);
			}
		}
	}

	if (m1 % 2)
	{
		for (int i = 0; i < n1; i++)
		{
			for (int j = 0; j < m2; j++)
			{
				result[i][j] = result[i][j] + matrix1[i][m1 - 1] * matrix2[m1 - 1][j];
			}
		}
	}


	free(mulH);
	free(mulV);

	return result;
}

int** Vinograd_optim(int** matrix1, int n1, int m1, int** matrix2, int n2, int m2)
{
	int* mulH = (int*)malloc(n1 * sizeof(int));
	for (int i = 0; i < n1; i++)
		mulH[i] = 0;

	int* mulV = (int*)malloc(m2 * sizeof(int));
	for (int i = 0; i < m2; i++)
		mulV[i] = 0;

	int** result = new_m(n1, m2);

	int m1Mod2 = m1 % 2;
	int n2Mod2 = n2 % 2;

	for (int i = 0; i < n1; i++)
	{
		for (int j = 0; j < (m1 - m1Mod2); j += 2)
		{
			mulH[i] += matrix1[i][j] * matrix1[i][j + 1];
		}
	}

	for (int i = 0; i < m2; i++)
	{
		for (int j = 0; j < (n2 - n2Mod2); j += 2)
		{
			mulV[i] += matrix2[j][i] * matrix2[j + 1][i];
		}
	}

	for (int i = 0; i < n1; i++)
	{
		for (int j = 0; j < m2; j++)
		{
			int buff = -(mulH[i] + mulV[j]);
			for (int k = 0; k < (m1 - m1Mod2); k += 2)
			{
				buff += (matrix1[i][k + 1] + matrix2[k][j]) * (matrix1[i][k] + matrix2[k + 1][j]);
			}
			result[i][j] = buff;
		}
	}

	if (m1Mod2)
	{
		int m1Min_1 = m1 - 1;
		for (int i = 0; i < n1; i++)
		{
			for (int j = 0; j < m2; j++)
			{
				result[i][j] += matrix1[i][m1Min_1]	* matrix2[m1Min_1][j];
			}
		}
	}

	free(mulH);
	free(mulV);

	return result;
}


int main()
{
	int n1;
	cout << "Input n1: ";
	cin >> n1;
	cout << "\n";

	int m1;
	cout << "Input m1: ";
	cin >> m1;
	cout << "\n";

	int n2;
	cout << "Input n2: ";
	cin >> n2;
	cout << "\n";

	int m2;
	cout << "Input m2: ";
	cin >> m2;
	cout << "\n";

	int** matrix1 = new_m(n1, m1);
	int** matrix2 = new_m(n2, m2);

	cout << "Input matrix1:\n";
	for (int i = 0; i < n1; i++)
	{
		for (int j = 0; j < m1; j++)
		{
			cin >> matrix1[i][j];
		}
	}

	cout << "\n\nInput matrix1:\n";
	for (int i = 0; i < n2; i++)
	{
		for (int j = 0; j < m2; j++)
		{
			cin >> matrix2[i][j];
		}
	}

	int **res = mult_m(matrix1, n1, m1, matrix2, n2, m2);

	cout << "\n\nResult:\n";
	for (int i = 0; i < n1; i++)
	{
		for (int j = 0; j < m2; j++)
		{
			cout << res[i][j] << " ";
		}
		cout << "\n";
	}

	int **res2 = Vinograd(matrix1, n1, m1, matrix2, n2, m2);

	cout << "\n\nResult:\n";
	for (int i = 0; i < n1; i++)
	{
		for (int j = 0; j < m2; j++)
		{
			cout << res2[i][j] << " ";
		}
		cout << "\n";
	}

	int **res3 = Vinograd_optim(matrix1, n1, m1, matrix2, n2, m2);

	cout << "\n\nResult:\n";
	for (int i = 0; i < n1; i++)
	{
		for (int j = 0; j < m2; j++)
		{
			cout << res3[i][j] << " ";
		}
		cout << "\n";
	}

}