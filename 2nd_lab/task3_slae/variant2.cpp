#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <omp.h>

static const int MAX_ITER = 100000;
static const double TOLERANCE = 1e-7;

int main(int argc, char *argv[])
{
    int N = 5000;
    int nthreads = 1;

    if (argc > 1) N = atoi(argv[1]);
    if (argc > 2) nthreads = atoi(argv[2]);

    omp_set_num_threads(nthreads);

    double tau = 0.01 / N;

    printf("SLAE Variant 2 (single parallel): N=%d, threads=%d, tau=%.6e\n",
           N, nthreads, tau);

    double *A = (double *)malloc(sizeof(double) * N * N);
    double *b = (double *)malloc(sizeof(double) * N);
    double *x = (double *)malloc(sizeof(double) * N);
    double *x_new = (double *)malloc(sizeof(double) * N);
    if (!A || !b || !x || !x_new) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            A[i * N + j] = (i == j) ? 2.0 : 1.0;
        b[i] = N + 1.0;
        x[i] = 0.0;
    }

    double t_start = omp_get_wtime();

    int converged = 0;
    int iters_done = 0;
    double norm_sq = 0.0;

#pragma omp parallel num_threads(nthreads)
    {
        for (int iter = 0; iter < MAX_ITER; iter++) {
#pragma omp for schedule(runtime)
            for (int i = 0; i < N; i++) {
                double ax_i = 0.0;
                for (int j = 0; j < N; j++)
                    ax_i += A[i * N + j] * x[j];
                x_new[i] = x[i] - tau * (ax_i - b[i]);
            }

#pragma omp for reduction(+:norm_sq) schedule(runtime)
            for (int i = 0; i < N; i++) {
                double d = x_new[i] - x[i];
                norm_sq += d * d;
            }

#pragma omp single
            {
                if (sqrt(norm_sq) < TOLERANCE) {
                    converged = 1;
                    iters_done = iter + 1;
                }
                double *tmp = x;
                x = x_new;
                x_new = tmp;
                norm_sq = 0.0;
            }

            if (converged)
                break;
        }
    }

    double t_elapsed = omp_get_wtime() - t_start;

    if (!converged)
        iters_done = MAX_ITER;

    double max_err = 0.0;
    for (int i = 0; i < N; i++) {
        double err = fabs(x[i] - 1.0);
        if (err > max_err) max_err = err;
    }

    printf("Iterations: %d\n", iters_done);
    printf("Max error:  %.2e\n", max_err);
    printf("Time:       %.6f sec\n", t_elapsed);
    printf("CSV,V2,%d,%d,%.6f,%d,%.2e\n", N, nthreads, t_elapsed, iters_done, max_err);

    free(A);
    free(b);
    free(x);
    free(x_new);
    return 0;
}
