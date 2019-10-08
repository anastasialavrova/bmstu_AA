#include<iostream>
#include<stdio.h>
#include<string>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <intrin.h>

#pragma intrinsic(__rdtsc)

using namespace std;

unsigned int ReplaceCost(char a, char b)
{
	return (a != b);
}

int levenshtein_rec(string &s1, int n1, string &s2, int n2)
{
	if (n1 == 0)
		return n2;
	if (n2 == 0)
		return n1;
	unsigned int tmp = min(levenshtein_rec(s1, n1 - 1, s2, n2), 
		levenshtein_rec(s1, n1, s2, n2 - 1)) + 1;
	return min(tmp, levenshtein_rec(s1, n1 - 1, s2, n2 - 1) 
		+ ReplaceCost(s1[n1 - 1], s2[n2 - 1]));
	
}

unsigned int levenshtein_damerau_rec(string &s1, int n1, string &s2, int n2)
{
	if (n1 == 0)
		return n2;
	if (n2 == 0)
		return n1;
	unsigned int tmp = min(levenshtein_damerau_rec(s1, n1 - 1, s2, n2), levenshtein_damerau_rec(s1, n1, s2, n2 - 1)) + 1;
	unsigned int res =  min(tmp, levenshtein_damerau_rec(s1, n1 - 1, s2, n2 - 1) + ReplaceCost(s1[n1 - 1], s2[n2 - 1]));
	if (n1 > 1 && n2 > 1 && s1[n1 - 1] == s2[n2 - 2] && s1[n1 - 2] == s2[n2 - 1])
	{
		res = min(res, levenshtein_damerau_rec(s1, n1 - 2, s2, n2 - 2) + 1);
	}
	// cout << (sizeof(n1) + sizeof(n2) + sizeof(tmp) + sizeof(s1) + sizeof(s2) + sizeof(res))*s1.length() << "\n";
	return res;
}

void swap_int_poiter(int **pointer1, int **pointer2)
{
	int *buf = *pointer1;
	*pointer1 = *pointer2;
	*pointer2 = buf;
}

int levinshtein_fill_number(int left, int diag, int top, bool equal)
{
	int min_number = min(diag, min(left, top));
	int res = min_number;
	if (!equal || min_number != diag)
	{
		res++;
	}
	return res;
}


int levenshtein_matrix(string s1, string s2)
{
	int len_s1 = s1.length();
	int len_s2 = s2.length();
	if (len_s1 == 0)
		return len_s2;
	if (len_s2 == 0)
		return len_s1;

	int **matrix = (int**)malloc((len_s1+1) * sizeof(int*));
	for (int i = 0; i <= len_s1; i++)  
	{
		matrix[i] = (int*)malloc((len_s2 + 1) * sizeof(int));
	}

	for (int i = 0; i <= len_s1; i++)
	{
		for (int j = 0; j <= len_s2; j++)
		{
			if (i == 0)
				matrix[i][j] = j;
			if (j == 0)
				matrix[i][j] = i;
		}
	}

	int cost;	for (int i = 1; i < len_s1 + 1; i++)	{		for (int j = 1; j < len_s2 + 1; j++)		{			cost = s1[i-1] == s2[j-1] ? 0 : 1;			matrix[i][j] = min(min(matrix[i-1][j] + 1,
				matrix[i][j-1] + 1), matrix[i-1][j-1] + cost);		}	}	for (int i = 0; i <= len_s1; i++)
	{
		for (int j = 0; j <= len_s2; j++)
		{
			cout << matrix[i][j] << " ";
		}
		cout << "\n";
	}

	int res = matrix[len_s1][len_s2];

	for (int i = 0; i <= len_s1; i++) 
		free(matrix[i]);  
	free(matrix);
	return res;
}

int levenshtein_damerau_matrix(string s1, string s2)
{
	int len_s1 = s1.length();
	int len_s2 = s2.length();
	if (len_s1 == 0)
		return len_s2;
	if (len_s2 == 0)
		return len_s1;

	int **matrix = (int**)malloc((len_s1 + 1) * sizeof(int*));
	for (int i = 0; i <= len_s1; i++)  
	{
		matrix[i] = (int*)malloc((len_s2 + 1) * sizeof(int));
	}

	for (int i = 0; i <= len_s1; i++)
	{
		for (int j = 0; j <= len_s2; j++)
		{
			if (i == 0)
				matrix[i][j] = j;
			if (j == 0)
				matrix[i][j] = i;
		}
	}

	int cost, buf;	for (int i = 1; i < len_s1 + 1; i++)	{		for (int j = 1; j < len_s2 + 1; j++)		{			cost = s1[i - 1] == s2[j - 1] ? 0 : 1;			if (i > 1 && j > 1 && s1[i - 1] == s2[j - 2] && s1[i - 2] == s2[j - 1])			{				buf = matrix[i - 2][j - 2] + 1;				matrix[i][j] = min(min(min(matrix[i - 1][j] + 1, matrix[i][j - 1] + 1), matrix[i - 1][j - 1] + cost), buf);			}			else			{				matrix[i][j] = min(min(matrix[i - 1][j] + 1,
					matrix[i][j - 1] + 1), matrix[i - 1][j - 1] + cost);			}		}	}	for (int i = 0; i <= len_s1; i++)
	{
		for (int j = 0; j <= len_s2; j++)
		{
			cout << matrix[i][j] << " ";
		}
		cout << "\n";
	}

	int res = matrix[len_s1][len_s2];

	for (int i = 0; i <= len_s1; i++)
		free(matrix[i]);
	free(matrix);
	return res;
}

unsigned long long count_res_tic(unsigned long long time_in, unsigned long long time_out, unsigned long long delta)
{
	unsigned long long time;
	if (time_out - time_in >= delta)
	{
		time = time_out - time_in - delta;
	}
	else
	{
		time = time_out - time_in;
	}
	return time;
}

void count_time(string s1, string s2)
{
	unsigned long long t1 = 0, t2 = 0, t3 = 0, t4 = 0;
	for (int i = 0; i < 10; i++)
	{
		unsigned long long time1_in = __rdtsc();
		unsigned long long delta1 = __rdtsc() - time1_in;
		time1_in = __rdtsc();
		int steps1 = levenshtein_rec(s1, s1.length(), s2, s2.length());
		unsigned long long time1_out = __rdtsc();
		cout << "\n" << "Time (levenshtein_rec): " << count_res_tic(time1_in, time1_out, delta1) << "\n";
		t1 += count_res_tic(time1_in, time1_out, delta1);

		unsigned long long time2_in = __rdtsc();
		unsigned long long delta2 = __rdtsc() - time2_in;
		time2_in = __rdtsc();
		int steps2 = levenshtein_matrix(s1, s2);
		unsigned long long time2_out = __rdtsc();
		cout << "\n" << "Time (matrix): " << count_res_tic(time2_in, time2_out, delta2) << "\n";
		t2 += count_res_tic(time2_in, time2_out, delta2);

		unsigned long long time3_in = __rdtsc();
		unsigned long long delta3 = __rdtsc() - time3_in;
		time3_in = __rdtsc();
		int steps3 = levenshtein_damerau_rec(s1, s1.length(), s2, s2.length());
		unsigned long long time3_out = __rdtsc();
		cout << "\n" << "Time (levenshtein_damerau_rec):" << count_res_tic(time3_in, time3_out, delta3) << "\n";
		t3 += count_res_tic(time3_in, time3_out, delta3);

		unsigned long long time4_in = __rdtsc();
		unsigned long long delta4 = __rdtsc() - time4_in;
		time4_in = __rdtsc();
		int steps4 = levenshtein_damerau_matrix(s1, s2);
		unsigned long long time4_out = __rdtsc();
		cout << "\n" << "Time (levenshtein_damerau_matrix):" << count_res_tic(time4_in, time4_out, delta4) << "\n";
		t4 += count_res_tic(time4_in, time4_out, delta4);
	}
	cout << "\n";
	cout << "T1: " << t1 / 10 << "\n";
	cout << "T2: " << t2 / 10 << "\n";
	cout << "T3: " << t3 / 10 << "\n";
	cout << "T4: " << t4 / 10 << "\n";
}

int main()
{
	string s1, s2;
	cout << "Vvedite 1 slovo: ";
	getline(cin, s1);
	cout << "\n";
	cout << "Vvedite 2 slovo: ";
	getline(cin, s2);
	cout << "\n";

	cout << "Levenshtein (recursive):" << "\n";
	int steps1 = levenshtein_rec(s1, s1.length(), s2, s2.length());
	cout << "Answer = " << steps1;
	cout << "\n\n";

	cout << "Levenshtein (matrix):" << "\n";
	int steps2 = levenshtein_matrix(s1, s2);
	cout << "Answer = " << steps2;
	cout << "\n\n";

	cout << "Damerau-Levenshtein (recursive):" << "\n";
	unsigned int steps3 = levenshtein_damerau_rec(s1, s1.length(), s2, s2.length());
	cout << "Answer = " << steps3;
	cout << "\n\n";

	cout << "Damerau-Levenshtein (matrix):" << "\n";
	unsigned int steps4 = levenshtein_damerau_matrix(s1, s2);
	cout << "Answer = " << steps4;
	cout << "\n\n";

	//count_time(s1, s2);
}