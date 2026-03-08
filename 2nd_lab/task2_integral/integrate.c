#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double func(double x)
{
    return exp(-x * x);
}

double integrate_serial(double (*f)(double), double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += f(a + h * (i + 0.5));
    return sum * h;
}

double integrate_omp(double (*f)(double), double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.0;

#pragma omp parallel
    {
        int nthreads = omp_get_num_threads();
        int tid = omp_get_thread_num();
        int items_per_thread = n / nthreads;
        int lb = tid * items_per_thread;
        int ub = (tid == nthreads - 1) ? n : (lb + items_per_thread);

        double local_sum = 0.0;
        for (int i = lb; i < ub; i++)
            local_sum += f(a + h * (i + 0.5));

#pragma omp atomic
        sum += local_sum;
    }

    return sum * h;
}

int main(int argc, char *argv[])
{
    int nsteps = 40000000;
    int nthreads = 1;

    if (argc > 1) nsteps = atoi(argv[1]);
    if (argc > 2) nthreads = atoi(argv[2]);

    omp_set_num_threads(nthreads);

    double a = -4.0;
    double b = 4.0;
    double exact = sqrt(M_PI);

    printf("Integration of exp(-x^2) on [%.1f, %.1f], nsteps=%d, threads=%d\n",
           a, b, nsteps, nthreads);

    double t0 = omp_get_wtime();
    double res_serial = integrate_serial(func, a, b, nsteps);
    double t_serial = omp_get_wtime() - t0;

    t0 = omp_get_wtime();
    double res_parallel = integrate_omp(func, a, b, nsteps);
    double t_parallel = omp_get_wtime() - t0;

    double speedup = t_serial / t_parallel;

    printf("Serial:   result=%.16f  error=%.2e  time=%.6f sec\n",
           res_serial, fabs(res_serial - exact), t_serial);
    printf("Parallel: result=%.16f  error=%.2e  time=%.6f sec\n",
           res_parallel, fabs(res_parallel - exact), t_parallel);
    printf("Speedup:  %.2f\n", speedup);

    printf("CSV,%d,%d,%.6f,%.6f,%.2f\n", nsteps, nthreads, t_serial, t_parallel, speedup);

    return 0;
}
