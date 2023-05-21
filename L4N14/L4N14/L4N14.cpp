#include "mpi.h"
#include <iostream>
#include <random>
#include <iomanip>

using namespace std;

int main(int argc, char** argv)
{
    const int n = 7;
    if (n < 4) {
        cout << "N cannot be less then 4";
        return 0;
    }

    const int k = n > 5 ? 4 * (n / 2 - 1) : 4;
    const int m = k * 2;
    float a[n][n], b[m];
    int xpose, rank;
    int array_of_displacements[k], array_of_blocklengths[k];

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            a[i][j] = j + 10 * i;
        }
    }

    if (rank == 0) {
        cout << "A\n";
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                cout << left
                    << setw(5)
                    << a[i][j]
                    << " ";
            }
            cout << "\n\n";
        }
    }

    //середина входной матрицы
    int mid = n / 2;
    //середина массивов array_of_blocklengths и array_of_displacements
    int mid_k = k / 2;

    //заполнение массива длин блоков
    array_of_blocklengths[0] = mid;
    for (int i = 1; i < k; i++) {
        array_of_blocklengths[i] = 1;
    }
    array_of_blocklengths[mid_k - 1] = mid;
    array_of_blocklengths[mid_k] = mid;
    array_of_blocklengths[k - 1] = mid;

    //заполнение массив смещений
    array_of_displacements[0] = 0;
    int j = 1;
    //заполнение первой половины
    for(int i = 1; i < mid_k; i++){
        array_of_displacements[i] = n * j;
        i++;
        array_of_displacements[i] = array_of_displacements[i - 1] + mid - 1;
        j++;
    }
    //в зависимости от того четная ли длина матрицы мы либо будем сдвигать смещение на 1 либо нет
    int add_num = 0;
    if (n % 2 != 0) {
        add_num = 1;
        j++;
    }
    array_of_displacements[mid_k] = n * j + mid + add_num;
    j++;  
    //заполнение второй половины
    for (int i = mid_k + 1; i < k - 1; i++) {
        array_of_displacements[i] = n * j + mid + add_num;
        i++;
        array_of_displacements[i] = array_of_displacements[i - 1] + mid - 1;
        j++;
    }
    array_of_displacements[k - 1] = n * (n - 1) + mid + add_num;

    MPI_Type_indexed(k, array_of_blocklengths, array_of_displacements, MPI_FLOAT, &xpose);
    MPI_Type_commit(&xpose);
    MPI_Sendrecv(a, 1, xpose, rank, 0, b, m, MPI_FLOAT, rank, 0, MPI_COMM_WORLD, &status);
    if (rank == 0) {
        cout << "B\n";
        for (int i = 0; i < m; i++) {

            cout << left
                << setw(5)
                << b[i]
                << " ";

        }
    }
    MPI_Type_free(&xpose);
    MPI_Finalize();
}