#include <iostream>
#include <random>

using namespace std;

int main()
{
	int n = 4;										//1
	int *mas = (int*)malloc(n * sizeof(int));		//2
	int left = 0;									//3
	int right = n - 1;								//4
	for (int i = 0; i < n; i++)						//5
	{
		mas[i] = n - i;								//6
	}
	while (left <= right)							//7
	{
		for (int i = left; i < right; i++)			//8
		{
			if (mas[i] > mas[i + 1])				//9
			{
				int buf = mas[i];					//10
				mas[i] = mas[i + 1];				//11
				mas[i + 1] = buf;					//12
			}
		}
		right--;									//13
		for (int j = right; j >= left; j--)			//14
		{
			if (mas[j - 1] > mas[j])				//15
			{
				int buf = mas[j];					//16
				mas[j] = mas[j - 1];				//17
				mas[j - 1] = buf;					//18
			}
		}
		left++;										//19
	}
}