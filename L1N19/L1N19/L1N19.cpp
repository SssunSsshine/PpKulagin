/*19. Каждый процесс обменивается сообщениями со всеми остальными 
и выводит номера тех процессов,
у которых ни одно значение не совпадает с его собственными значениями.*/

#include <iostream>
#include <mpi.h>
#include <iomanip>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <random>
#include <functional>

using namespace std;
const int m = 5;
int TAG = 0;

//проверка различия двух массивов
bool is_different(int  message[m], int  buffer[m])
{
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < m; j++) {
			if(message[i] == buffer[j])
				return false;
		}
	}
	return true;
}

//заполнение массива рандомом
void fill_array(int* arr, int len)
{
	std::random_device rnd_device;
	std::mt19937 mersenne_engine(rnd_device());
	std::uniform_int_distribution<int> dist(0, 50);
	for (int i = 0; i < len; i++) {
		arr[i] = dist(mersenne_engine);
	}
}

//вывод массива
void output_array(int  message[m])
{
	for (int i = 0; i < m; i++)
		cout << left
		<< setw(10)
		<< message[i] << ' ';
	cout << "\n";
}

int main(int argc, char* argv[])
{
	int procnum;
	int message[m];
	int myrank;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &procnum);
	fill_array(message, m);
	cout << myrank << " process: ";
	output_array(message);
	//рассылка всем процессам сообщения
	for (int i = 0; i < procnum; i++) {
		if (i != myrank) {
			MPI_Send(&message, m, MPI_INT, i, TAG, MPI_COMM_WORLD);
		}
	}
	cout << myrank << " process: ";
	for (int i = 0; i < procnum; i++) {
		if (i != myrank) {
			int buffer[m];
			//принятие сообщений от других процессов
			MPI_Recv(&buffer, m, MPI_INT, i, TAG, MPI_COMM_WORLD, &status);
			if (is_different(message, buffer)) {
				cout << i << " ";
			}
		}		
	}
	cout << "\n\n";
	MPI_Finalize();
}






