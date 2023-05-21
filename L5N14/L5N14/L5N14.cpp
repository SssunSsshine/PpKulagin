// Вариант 14. max(Sum(aik+bjk))
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <random>
#include <iostream>
#include <iomanip>


#define dimension 1//размерность декартовой решетки
using namespace std;

//вычисления суммы
int calс_sum(int* a, int* b, int len)
{
    int s = 0;
    for (int i = 0; i < len; i++) {

        s += a[i] + b[i];
    }
    return s;
}

void random_array(int* arr, int len)
{
    std::random_device rnd_device;
    std::mt19937 mersenne_engine(rnd_device());
    std::uniform_int_distribution<int> dist(1, 50);
    for (int i = 0; i < len; i++) {
        arr[i] = dist(mersenne_engine);
    }
}

void printArr(int* A, int n)
{
    for (int i = 0;i < n;i++)
    {
        cout << A[i] << " ";
    }
    cout << "\n";
}

int main(int argc, char* argv[])
{
    int i, j, n, rank, rank_pred, rank_next, current;
    int dims[dimension], periods[dimension];
    MPI_Comm new_comm;
    MPI_Status st;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Обнуляем массив dims и заполняем массив periods для топологии "кольцо" 
    for (i = 0; i < dimension; i++)
    {
        dims[i] = 0;
        periods[i] = 1;
    }

    MPI_Dims_create(n, dimension, dims);
    MPI_Cart_create(MPI_COMM_WORLD, dimension, dims, periods, 0, &new_comm);
    MPI_Cart_shift(new_comm, 0, -1, &rank_pred, &rank_next);

    int* A, * B;
    A = (int*)(malloc(sizeof(int) * n));
    B = (int*)(malloc(sizeof(int) * n));

    //заполнение строки  А
    random_array(A, n);

    //заполнение строки B
    random_array(B, n);

    cout << "PROCESS "<< rank << "____________________________________\n\n";
    cout << "A " << "\n";
    printArr(A, n);
    cout << "B " << "\n";
    printArr(B, n);

    int maximum = calс_sum(A, B, n);
    cout << "sum 0" << " = " << maximum << " ";
    for (j = 1;j < n;j++)
    {
        //передаем и получаем новую строчку B
        MPI_Sendrecv_replace(B, n, MPI_INT, rank_next, 2, rank_pred, 2, new_comm, &st);
        
        cout << "\n\nA\n";
        printArr(A, n);
        cout << "B\n";
        printArr(B, n);

        current = calс_sum(A, B, n);
        maximum = maximum > current ? maximum : current;

        cout << "sum " << j << " = " << current << " ";
        cout << "\n";
    }
    cout << "\n";
    cout << "MAX = " << maximum << "\n\n";
    MPI_Comm_free(&new_comm);
    free(A);
    free(B);
    MPI_Finalize();
    return 0;
}