#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <cmath>
#include <ctime>

using namespace std;

size_t MAX = numeric_limits<size_t>::max();
size_t l_min = MAX;
vector<size_t> route_min;

template<typename T>
void print_arr(vector<T> a) {
	size_t n = a.size();
	for (size_t i = 0; i < n; i++)
		cout << a[i] + 1 << " ";
	cout << endl;
}

template<typename T>
void print_sum_arr(vector<T> a) {
	size_t n = a.size();
	T sum = 0;
	for (size_t i = 0; i < n; i++)
		sum += a[i];
	cout << sum << endl;
}

template<typename T>
vector<T> new_vec_without_last(vector<T> to, T last) {
	size_t n = to.size();
	vector<T> cur;
	for (size_t i = 0; i < n; i++)
		if (to[i] != last)
			cur.push_back(to[i]);
	return cur;
}

void recurs_func(size_t from, vector<size_t> to, vector<vector<size_t>> d, vector<size_t> route, size_t l) {

	size_t n = to.size();
	size_t last;
	if (n == 1) {
		last = to[0];
		route.push_back(last);
		l += d[from][last];
		l += d[last][route[0]];
		route.push_back(route[0]);
		if (l < l_min) {
			l_min = l;
			route_min = route;
		}
		return;
	}

	vector<size_t> cur_to(n - 1);
	vector<size_t> cur_route(n);
	size_t cur_l;
	for (size_t i = 0; i < n; i++) {
		last = to[i];
		cur_to = new_vec_without_last(to, last);
		cur_route = route;
		cur_route.push_back(last);
		cur_l = l + d[route[route.size() - 1]][last];
		recurs_func(last, cur_to, d, cur_route, cur_l);
	}

}

void perebor(size_t n, vector<vector<size_t>> d) {
	l_min = MAX;
	route_min.clear();
	size_t l = 0;
	vector<size_t> route;
	route_min.resize(n + 1);
	vector<size_t> to(n - 1);

	for (size_t i = 0; i < n; i++) {
		route.clear();
		to.clear();
		l = 0;

		route.push_back(i);
		for (size_t j = 0; j < n; j++)
			if (j != i)
				to.push_back(j);
		recurs_func(i, to, d, route, l);
	}

	cout << endl << "ROUTE: ";
	print_arr(route_min);
	cout << "LENGHT: " << l_min << endl << endl;
}

vector<double> get_probability(size_t from, vector<size_t> to, vector<vector<double>> tao, vector<vector<double>> attraction,
	size_t alpha, size_t beta) {

	double znam = 0, chisl = 0;
	size_t n = to.size();
	vector<double> result(n);
	for (size_t i = 0; i < n; i++) {
		znam += pow(tao[from][to[i]], alpha) * pow(attraction[from][to[i]], beta);
	}
	for (size_t j = 0; j < n; j++) {
		chisl = pow(tao[from][to[j]], alpha) * pow(attraction[from][to[j]], beta);
		result[j] = chisl / znam;
	}
	return result;
}

void get_route(vector<size_t> all, size_t start, vector<size_t> &route, size_t &len, vector<vector<size_t>> d,
	vector<vector<double>> tao, vector<vector<double>> attraction,
	size_t alpha, size_t beta) {

	route.resize(0);
	route.push_back(start);
	vector<size_t> to = new_vec_without_last(all, start);
	size_t n_1 = tao.size() - 2;
	size_t from;
	double coin, sum;
	bool flag;

	for (size_t i = 0; i < n_1; i++) {
		sum = 0;
		flag = true;
		from = route[i];
		vector<double> p = get_probability(from, to, tao, attraction, alpha, beta);
		coin = double(rand() % 10000) / 10000;
		for (size_t j = 0; j < p.size() && flag; j++) {
			sum += p[j];
			if (coin < sum) {
				route.push_back(to[j]);
				len += d[from][to[j]];
				to = new_vec_without_last(to, to[j]);
				flag = false;
			}
		}
	}
	len += d[route[route.size() - 1]][to[0]];
	route.push_back(to[0]);
	len += d[route[route.size() - 1]][route[0]];
	route.push_back(route[0]);
}

bool in_route(size_t a, size_t b, vector<size_t> route) {
	bool res = false;
	size_t m = route.size() - 1;
	for (size_t i = 0; i < m; i++) {
		if (a == route[i] && b == route[i + 1])
			res = true;
	}
	return res;
}

void ant(size_t n, vector<vector<size_t>> d, size_t alpha, size_t beta, double q, size_t time_max, ofstream& file) {

	l_min = MAX;
	route_min.clear();

	double tao_min, tao_start, Q;
	vector<size_t> all(n);
	Q = 350;
	tao_min = 0.001;
	tao_start = 0.5;

	vector<vector<size_t>> routes(n);
	vector<size_t> lens(n);

	vector<vector<double>> attraction(n);
	vector<vector<double>> tao(n);

	for (size_t i = 0; i < n; i++) {
		attraction[i].resize(n);
		tao[i].resize(n);
		lens[i] = 0;
		all[i] = i;
		for (size_t j = 0; j < n; j++) {
			if (i != j) {
				attraction[i][j] = 1.0 / d[i][j];
				tao[i][j] = tao_start;
			}
		}
	}

	for (size_t time = 0; time < time_max; time++) {
		for (size_t k = 0; k < n; k++) {
			get_route(all, k, routes[k], lens[k], d, tao, attraction, alpha, beta);
			if (lens[k] < l_min) {
				l_min = lens[k];
				route_min = routes[k];
			}
		}
		for (size_t i = 0; i < n; i++)
			for (size_t j = 0; j < n; j++) {
				double sum = 0;
				for (size_t m = 0; m < n; m++) {
					if (in_route(i, j, routes[m]))
						sum += Q / lens[m];
				}

				tao[i][j] = tao[i][j] * (1 - q) + sum;
				if (tao[i][j] < tao_min)
					tao[i][j] = tao_min;
			}
	}


	file << " & " << time_max << " & " << alpha << " & " << beta << " & " << q << " & " << l_min << " & ";
	for (size_t i = 0; i < route_min.size(); i++)
		file << route_min[i] << " ";
	file << " \\\\ \n" << endl;
}

int main()
{
	srand(time(0));
	size_t n;
	FILE *f = fopen("mtr10_5.txt", "r");
	if (f == NULL) {
		cout << "ERROR_READ_FILE" << endl;
		return 0;
	}
	fscanf(f, "%ld", &n);
	cout << n << endl;
	vector<vector<size_t>> d(n);
	for (size_t i = 0; i < n; i++) {
		d[i].resize(n);
		for (size_t j = 0; j < n; j++)
			fscanf(f, "%ld", &d[i][j]);
	}
	fclose(f);


	for (size_t i = 0; i < n; i++) {
		print_arr(d[i]);
	}

	double beg, end;
	beg = clock();
	perebor(n, d);
	end = clock();
	cout << "Time: " << double(end - beg) / CLOCKS_PER_SEC << endl;

	vector<size_t> alphas = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	vector<double> qs = { 0.1, 0.2, 0.25, 0.3, 0.4, 0.5, 0.6, 0.7, 0.75, 0.8, 0.9 };
	vector<size_t> tmax = { 5, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };

	ofstream result("result.txt");

	for (size_t time = 0; time < tmax.size(); time++)
		for (size_t koef = 0; koef < alphas.size(); koef++)
			for (size_t isp = 0; isp < qs.size(); isp++)
				ant(n, d, alphas[koef], 10 - alphas[koef], qs[isp], tmax[time], result);

	result.close();

	return 0;
}