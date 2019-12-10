#include <iostream>
#include <string>
#include <vector>
# include <limits.h>
# include <string.h>
# include <stdio.h>

using namespace std;

# define NO_OF_CHARS 256

int max(int a, int b) { return (a > b) ? a : b; }

void badchars(char *str, int size,
	int badchar[NO_OF_CHARS]);

// Функция поиска по шаблону, использующая эвристику плохого характера алгоритма Бойера-Мура
int bm_search(char *txt, char *pat)
{
	int m = (int)strlen(pat);
	int n = (int)strlen(txt);

	int result;

	int badchar[NO_OF_CHARS];

	//Заполнение массива плохих символов, вызвав функцию предварительной обработки badchars() для данного шаблона
	badchars(pat, m, badchar);

	int s = 0;  // s - смещение шаблона относительно текста
	while (s <= (n - m))
	{
		int j = m - 1;

		// пока совпадают
		while (j >= 0 && pat[j] == txt[s + j])
			j--;

		if (j < 0)
		{
			result = s;

			// сдвиг
			// s + m < n на случай, если паттерн появляется в конце текста
			s += (s + m < n) ? m - badchar[txt[s + m]] : 1;

		}
		else
			s += max(1, j - badchar[txt[s + j]]);
	}

	return result;
}

// Функция предварительной обработки для плохого характера Бойера-Мура
void badchars(char *str, int size,
	int badchar[NO_OF_CHARS])
{
	int i;

	// Инициализировать все вхождения как -1
	for (i = 0; i < NO_OF_CHARS; i++)
		badchar[i] = -1;

	// Заполнение фактического значения последнего вхождения символа
	for (i = 0; i < size; i++)
		badchar[(int)str[i]] = i;

}

void compute_prefix(char* pat, int M, int* lps);

int kmp_search(char* pat, char* txt)
{
	int M = (int)strlen(pat);
	int N = (int)strlen(txt);

	int result = -1;

	// для самого длинного префикса
	int *lps = (int*)calloc(M, sizeof(int));

	compute_prefix(pat, M, lps);

	int i = 0; // индекс txt[]
	int j = 0; // индекс pat[]
	while (i < N)
	{
		if (pat[j] == txt[i])
		{
			j++;
			i++;
		}

		if (j == M)
		{
			result = i - j;
			j = lps[j - 1];
		}

		// несовпадение
		else if (i < N && pat[j] != txt[i])
		{
			if (j != 0)
				j = lps[j - 1];
			else
				i++;
		}
	}
	return result;
}

// заполняет префикс pat[0..M-1]
void compute_prefix(char* pat, int M, int* lps)
{
	// длина прошлого самого длинного префикса
	int len = 0;

	lps[0] = 0; // lps[0] is always 0

				// префикс for i = 1 to M-1
	int i = 1;
	while (i < M)
	{
		if (pat[i] == pat[len])
		{
			len++;
			lps[i] = len;
			i++;
		}

		else
		{
			if (len != 0)
			{
				len = lps[len - 1];
			}
			else
			{
				lps[i] = 0;
				i++;
			}
		}
	}
}


int main()
{
	char txt[] = "there they are";
	char pat[] = "they";

	//char txt[] = "abeccacbadbabbad";
	//char pat[] = "abbad";

	cout << txt << "\n" << pat << endl << endl;

	cout << "KMP:" << endl;
	int ind_kmp = kmp_search(pat, txt);
	cout << "Pattern found at index: " << ind_kmp << endl << endl;

	cout << "BM:" << endl;
	int ind_bm = bm_search(txt, pat);
	cout << "Pattern found at index: " << ind_bm << endl;

	return 0;
}