#include<iostream>
#include<stdio.h>
#include<string>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <intrin.h>
#include <fstream>

#pragma intrinsic(__rdtsc)

using namespace std;

void sortInsertion(int* mas, int n) 
{ 
	for (int i = 1; i < n; i++) 
	{ 
		int key = mas[i];         
		int j = i - 1;         
		for (; j >= 0 && mas[j] > key; j--)             
			mas[j + 1] = mas[j];         
		mas[j + 1] = key; 
	} 
}

void sortBubble(int* mas, int n) 
{ 
	for (int i = 0; i < n; i++)         
		for (int j = 0; j < n - i - 1; j++)             
			if (mas[j] > mas[j + 1])
			{
				int buf = mas[j];
				mas[j] = mas[j + 1];
				mas[j + 1] = buf;
			}
}

void sortShaker(int *mas, int n)
{
	int left = 0, right = n - 1;
	while (left <= right)
	{
		for (int i = left; i < right; i++)
		{
			if (mas[i] > mas[i + 1])
			{
				int buf = mas[i];
				mas[i] = mas[i + 1];
				mas[i + 1] = buf;
			}
		}
		right--;
		for (int j = right; j >= left; j--)
		{
			if (mas[j - 1] > mas[j])
			{
				int buf = mas[j];
				mas[j] = mas[j - 1];
				mas[j - 1] = buf;
			}
		}
		left++;
	}
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

void count_time(int *mas1, int *mas2, int *mas3, int n)
{
	unsigned long long t1 = 0, t2 = 0, t3 = 0, t4 = 0;
		unsigned long long time1_in = __rdtsc();
		unsigned long long delta1 = __rdtsc() - time1_in;
		time1_in = __rdtsc();
		sortBubble(mas2, n);
		unsigned long long time1_out = __rdtsc();
		t1 += count_res_tic(time1_in, time1_out, delta1);

		unsigned long long time2_in = __rdtsc();
		unsigned long long delta2 = __rdtsc() - time2_in;
		time2_in = __rdtsc();
		sortInsertion(mas1, n);
		unsigned long long time2_out = __rdtsc();
		t2 += count_res_tic(time2_in, time2_out, delta2);

		unsigned long long time3_in = __rdtsc();
		unsigned long long delta3 = __rdtsc() - time3_in;
		time3_in = __rdtsc();
		sortShaker(mas3, n);
		unsigned long long time3_out = __rdtsc();
		t3 += count_res_tic(time3_in, time3_out, delta3);

	cout << "\n";
	cout << "T1: " << t1 / 10 << "\n";
	cout << "T2: " << t2 / 10 << "\n";
	cout << "T3: " << t3 / 10 << "\n";
}

void generate_tests(int n)
{
	FILE *f;
	string a;
	f = fopen("test.txt", "w");
	ofstream fout("test.txt");
	//fout << "3455646";
	srand(time(NULL));
	for (int i = n - 1; i >= 0; i--)
	{
		fout << i;
		if (i != 99)
			fout << " ";
	}
	fclose(f);
}

void read_test(int *mas1, int *mas2, int* mas3, int n) 
{
	FILE *f;
	string a;
	f = fopen("test.txt", "r");
	ifstream fin("test.txt");
	//fout << "3455646";
	srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		int x;
		fin >> x;
		mas1[i] = x;
		mas2[i] = x;
		mas3[i] = x;
	}
	fclose(f);
}

int main()
{

	int n, x;

	//cout << "Input size: ";

	//cin >> n;
	//cout << "\n";

	n = 900;

	generate_tests(n);

	cout << "Input mas: ";
	int *mas1 = (int*)malloc(n * sizeof(int));
	int *mas2 = (int*)malloc(n * sizeof(int));
	int *mas3 = (int*)malloc(n * sizeof(int));

	read_test(mas1, mas2, mas3, n);

	for (int i = 0; i < n; i++)
		cout << mas2[i] << " ";

	/*
	for (int i = 0; i < n; i++)
	{
		cin >> x;
		mas1[i] = x;
		mas2[i] = x;
		mas3[i] = x;
	}
	*/

	count_time(mas1, mas2, mas3, n);

	/*
	cout << "sortBubble:\n";

	sortBubble(mas2, n);
	for (int i = 0; i < n; i++)
		cout << mas2[i] << " ";

	cout << "\n";

	cout << "sortInsertion:\n";

	sortInsertion(mas1, n);

	for (int i = 0; i < n; i++)
		cout << mas1[i] << " ";

	cout << "\n";

	cout << "sortShaker:\n";

	sortShaker(mas3, n);
	for (int i = 0; i < n; i++)
		cout << mas3[i] << " ";

	cout << "\n";
	*/
}