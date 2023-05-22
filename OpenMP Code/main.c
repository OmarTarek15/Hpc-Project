#include <stdio.h>
#include <math.h>
#include <omp.h>

int main()
{
    int n;
    printf("Enter  the  array size: ");
    scanf("%d", &n);

    int elemnts[n];
    printf("Enter array elements: ");
    for (int i = 0; i < n; i++)
    {
        scanf("%d", &elemnts[i]);
    }

    double start_time, end_time;
    double mean, var, standarddeviation;

    start_time = omp_get_wtime();
    double sum = 0.0;
#pragma omp parallel for reduction(+ : sum)
    for (int i = 0; i < n; i++)
    {
        sum += elemnts[i];
    }
    mean = sum / n;
    end_time = omp_get_wtime();

    printf("Mean = %.1f\n", mean);

    double difference_sum = 0.0;
#pragma omp parallel for reduction(+ : difference_sum)
    for (int i = 0; i < n; i++)
    {
        double difference = elemnts[i] - mean;
        difference_sum += difference * difference;
    }
    var = difference_sum / n;

    printf("Variance = %.4f\n", var);

    standarddeviation = sqrt(var);

    printf("Standard Deviation = %.4f\n", standarddeviation);

    printf("Parallel Time: %.6f seconds\n", end_time - start_time);

    return 0;
}
