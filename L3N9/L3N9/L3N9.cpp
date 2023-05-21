/*Найти максимальную старшую цифру.*/
#include "mpi.h"
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <random>
#include <iomanip>
#include <functional>
#include <string>
#include <sstream>

using namespace std;

//находит старшую цифру числа num
int first_digit(int num) {
    string s = to_string(num);
    const char* nchar = s.c_str();
    return nchar[0] - '0';
}

//функция приведения находит максимальную старшую цифру
void max_first_digit(int* invec, int* inoutvec, int* len, MPI_Datatype* dtype)
{
    int i;
    for (i = 0; i < *len; i++)
    {
        int invec_digit = first_digit(invec[i]);
        int inoutvec_digit = first_digit(inoutvec[i]);
        inoutvec[i] =invec_digit > inoutvec_digit ? invec_digit : inoutvec_digit;
    }
}

void random_array(int* arr, int len)
{
    std::random_device rnd_device;
    std::mt19937 mersenne_engine(rnd_device());
    std::uniform_int_distribution<int> dist(5, 1000);
    for (int i = 0; i < len; i++) {
        arr[i] = dist(mersenne_engine);
    }
}

int main(int argc, char** argv)
{
    const int n = 7;
    int rank, size, i;
    int data[n];
    int result[n];
    MPI_Op op;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    random_array(data, n);
    for (int i = 0;i < n;i++)
    {
        result[i] = 0;
    }

    cout << left
        << setw(20)
        << to_string(rank) + " process: ";

    for (i = 0;i < n;i++)
        cout << left
        << setw(10)
        << data[i] << ' ';
    cout << "\n\n";

    MPI_Op_create((MPI_User_function*)max_first_digit, 0, &op);
    MPI_Reduce(&data, &result, n, MPI_INT, op, 0, MPI_COMM_WORLD);

    MPI_Op_free(&op);
    if (rank == 0) {
        cout << left
            << setw(20)
            << "res ";
        for (i = 0;i < n;i++)
            cout << left
            << setw(10)
            << result[i] << ' ';
        cout << "\n\n";
    }

    MPI_Finalize();
}