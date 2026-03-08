#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void matrix_vector_serial(double *A, double *b, double *c, int m, int n)
{
    for (int i = 0; i < m; i++) {
        c[i] = 0.0;
        for (int j = 0; j < n; j++)
            c[i] += A[i * n + j] * b[j];
    }
}

void matrix_vector_omp(double *A, double *b, double *c, int m, int n)
{
#pragma omp parallel
    {
        int nthreads = omp_get_num_threads();
        int tid = omp_get_thread_num();
        int items_per_thread = m / nthreads;
        int lb = tid * items_per_thread;
        int ub = (tid == nthreads - 1) ? m : (lb + items_per_thread);

        for (int i = lb; i < ub; i++) {
            c[i] = 0.0;
            for (int j = 0; j < n; j++)
                c[i] += A[i * n + j] * b[j];
        }
    }
}

int main(int argc, char *argv[])
{
    int m = 20000;
    int nthreads = 1;

    if (argc > 1) m = atoi(argv[1]);
    if (argc > 2) nthreads = atoi(argv[2]);

    int n = m;
    omp_set_num_threads(nthreads);

    printf("Matrix size: %d x %d, threads: %d\n", m, n, nthreads);

    double *A = (double *)malloc(sizeof(double) * m * n);
    double *b = (double *)malloc(sizeof(double) * n);
    double *c = (double *)malloc(sizeof(double) * m);
    if (!A || !b || !c) {
        fprintf(stderr, "Failed to allocate memory\n");
        return 1;
    }

#pragma omp parallel for schedule(static)
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            A[i * n + j] = i + j;
    }
#pragma omp parallel for schedule(static)
    for (int j = 0; j < n; j++)
        b[j] = j;

    double t0 = omp_get_wtime();
    matrix_vector_serial(A, b, c, m, n);
    double t_serial = omp_get_wtime() - t0;

    t0 = omp_get_wtime();
    matrix_vector_omp(A, b, c, m, n);
    double t_parallel = omp_get_wtime() - t0;

    double speedup = t_serial / t_parallel;

    printf("Serial:   %.6f sec\n", t_serial);
    printf("Parallel: %.6f sec\n", t_parallel);
    printf("Speedup:  %.2f\n", speedup);

    printf("CSV,%d,%d,%.6f,%.6f,%.2f\n", m, nthreads, t_serial, t_parallel, speedup);

    free(A);
    free(b);
    free(c);
    return 0;
}
