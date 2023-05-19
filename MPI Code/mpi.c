#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int pid;
    int noOfProc;
    int n;
    double mean;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &noOfProc);

    if (pid == 0)
    {
        // array size
        printf("Enter array size: ");
        scanf("%d", &n);
        double arr[n];
        int i;

        // array elements input
        printf("Enter elements: ");
        for (i = 0; i < n; ++i)
        {
            scanf("%lf", &arr[i]);
        }

        int idx = 0;
        // number of elements to send
        int elements = n / noOfProc;
        // send number of elements to be received and send the elements
        for (i = 1; i < noOfProc - 1; ++i)
        {
            MPI_Send(&elements,
                     1, MPI_INT, i, 0,
                     MPI_COMM_WORLD);
            MPI_Send(&arr[idx],
                     elements, MPI_DOUBLE, i, 0,
                     MPI_COMM_WORLD);
            idx = i * elements;
        }

        // remaining elements in array
        int rem_elem = n - idx;

        MPI_Send(&rem_elem,
                 1, MPI_INT, i, 0,
                 MPI_COMM_WORLD);
        MPI_Send(&arr[idx],
                 rem_elem, MPI_DOUBLE, i, 0,
                 MPI_COMM_WORLD);

        // recieve partial sums
        double sum = 0, temp;
        for (i = 1; i < noOfProc; ++i)
        {
            MPI_Recv(&temp, 1, MPI_DOUBLE, i, 0,
                     MPI_COMM_WORLD, &status);
            sum += temp;
        }

        // calculate mean
        mean = sum / n;
        for (i = 1; i < noOfProc; ++i)
        {
            MPI_Send(&mean,
                     1, MPI_DOUBLE, i, 0,
                     MPI_COMM_WORLD);
        }

        // recieve sum of deviations
        double dev = 0, temp2;
        for (i = 1; i < noOfProc; ++i)
        {
            MPI_Recv(&temp2, 1, MPI_DOUBLE, i, 0,
                     MPI_COMM_WORLD, &status);
            dev += temp2;
        }

        // calculate variance
        double variance = dev / n;
        // calculate standard deviation
        double stand_dev = sqrt(variance);

        printf("Mean = %lf, Variance = %lf, Standard deviation = %lf\n", mean, variance, stand_dev);
    }
    else
    {
        // recieve array size
        int elem_recv;
        MPI_Recv(&elem_recv,
                 1, MPI_INT, 0, 0,
                 MPI_COMM_WORLD, &status);
        double tmp[elem_recv];
        // recieve elements
        MPI_Recv(&tmp, elem_recv,
                 MPI_DOUBLE, 0, 0,
                 MPI_COMM_WORLD, &status);

        // calculate sum of elements
        double tmp_sum = 0;
        for (int i = 0; i < elem_recv; ++i)
        {
            tmp_sum += tmp[i];
        }

        // send back sum of elements
        MPI_Send(&tmp_sum, 1, MPI_DOUBLE, 0, 0,
                 MPI_COMM_WORLD);

        // recieve mean value
        double tmpMean;
        MPI_Recv(&tmpMean, 1,
                 MPI_DOUBLE, 0, 0,
                 MPI_COMM_WORLD, &status);

        // calculate deviations of each data point
        double deviations = 0;
        for (int i = 0; i < elem_recv; ++i)
        {
            double x = tmp[i] - tmpMean;
            deviations += pow(x, 2);
        }

        // send back deviations and their sum
        MPI_Send(&deviations, 1, MPI_DOUBLE, 0, 0,
                 MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}