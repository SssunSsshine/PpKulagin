//sin(x)

#include  <mpi.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <string>
#include "L2N20.h"

void input_value(double& value)
{
	std::string str;
	std::cin >> str;
	std::istringstream istr(str);
	if (!(istr >> value)) {
		value = -3;
	}
}

void line(int n)
{
	for (int i = 0; i < 20 + (20 - n) * n; i++)
	{
		std::cout << "_";
	}
	std::cout << std::endl;
}

//вывод в консоль заголовков столбцов таблицы
void print_header(int n)
{
	std::cout
		<< std::left
		<< std::setw(20)
		<< "|";
	for (int i = 1; i <= n; i++) {
		std::cout
			<< std::left
			<< std::setw(10)
			<< "|X" + std::to_string(i);
	}
	std::cout << std::endl;
}

//вывод в консоль строки содержащей значения точек и n и epsilon
void print_x_value(int n, double eps, double* globaldata)
{
	std::cout
		<< std::left
		<< std::setw(20)
		<< "|X Values";
	for (int i = 0; i < n; i++)
		std::cout
		<< std::left
		<< std::setw(10)
		<< "|" + std::to_string(globaldata[i]);
	std::cout << std::endl;
}

//вывод в консоль строки содержащей результат, вычесленный с точностью eps
void print_eps_result(int n, double eps, double* resglobal)
{
	std::cout
		<< std::left
		<< std::setw(20)
		<< "|epsilon results";
	for (int i = 0; i < n; i++)
		std::cout
		<< std::left
		<< std::setw(10)
		<< "|" + std::to_string(resglobal[i]);
	std::cout << std::endl;
}

//вывод в консоль результирующих значений, вычесленных по формуле заданной в условии задачи 
void print_result_values(int n, double eps, double* res)
{
	std::cout
		<< std::left
		<< std::setw(20)
		<< "|Results";
	for (int i = 0; i < n; i++)
		std::cout
		<< std::left
		<< std::setw(10)
		<< "|" + std::to_string(res[i]);
	std::cout << std::endl;
}

//вычисление ряда тэйлора для числа x с точностью eps
double calculate_taylor(double x, double eps)
{
	double cur = x;
	double res = x;
	double sqr_x = pow(x, 2);
	int i = 1;
	while (abs(cur) >= eps) {
		cur *= (-1) * sqr_x / (2 * i * (2 * i + 1));
		res += cur;
		i++;
	}
	return res;
}

void input_x(double& x, std::string message)
{
	std::string str;
	std::cout << message;
	std::cin >> str;
	std::istringstream istr(str);
	bool isNaN = !(istr >> x);
	while (isNaN) {
		std::cout << "Error. " << message;
		std::cin >> str;
		std::istringstream istr(str);
		isNaN = !(istr >> x);
	}
}

int main(int argc, char* argv[])
{
	int procnum;
	int myrank;
	double x1;
	double xn;
	int n;
	double eps;
	double* globaldata = NULL;
	double* localdata = NULL;
	double* reslocal = NULL;
	double* resglobal = NULL;
	double* res = NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &procnum);

	if (myrank == 0) {
		
		input_x(x1, "Input first value: ");

		do {
			std::cout << "\n";
			std::cout << "Input last value: ";
			input_value(xn);
			std::cout << "\n";
		} while (xn < x1);
		

		do {
			std::cout << "Input epsilon: ";
			input_value(eps);
		} while (eps >= 1 || eps <= 0);
		std::cout << "\n";

		do {
			std::cout << "Input n: ";
			std::string str;
			std::cin >> str;
			std::istringstream istr(str);
			if (!(istr >> n)) {
				n = -3;
			}

		} while (n <= 0 || n < procnum);
		std::cout << "\n";

		globaldata = new double[n];
		res = new double[n];
		resglobal = new double[n];

		double step = (xn - x1 + 1) / n;
		double tmp = x1;
		for (int i = 0; i < n; i++) {
			globaldata[i] = tmp;
			res[i] = sin(tmp);
			tmp = tmp + step;
		}
	}

	//широковещание, посылаем всем процессам эпсилон и n
	MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//вычисляем сколько данных посылается каждому из процессов
	double k = ceil((double)n / (double)procnum);
	localdata = new double[k];
	reslocal = new double[k];

	//распределяем иксы по всем процессам 
	MPI_Scatter(globaldata, k, MPI_DOUBLE, localdata, k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	for (int i = 0; i < k; i++) {
		reslocal[i] = calculate_taylor(localdata[i], eps);
	}

	//собираем результирующие данные по всем процессам
	MPI_Gather(reslocal, k, MPI_DOUBLE, resglobal, k, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (myrank == 0) {
		line(n);
		print_header(n);
		line(n);

		print_x_value(n, eps, globaldata);
		line(n);

		print_eps_result(n, eps, resglobal);
		line(n);

		print_result_values(n, eps, res);
		line(n);
	}

	MPI_Finalize();
	return 0;
}
