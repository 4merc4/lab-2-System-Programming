// Лаба: профілювання та оптимізація.
// Множення матриць N x N (double). Режими:
//   ./matmul naive  [N]  - класичний цикл i-j-k (кеш-неефективний доступ до B)
//   ./matmul opt    [N]  - цикл i-k-j (послідовний доступ, автовекторизація/SIMD)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEFAULT_N 1024

static double *alloc_matrix(int n) {
    double *m = aligned_alloc(64, (size_t)n * n * sizeof(double));
    if (!m) { perror("alloc"); exit(1); }
    return m;
}

__attribute__((noinline))
static void matmul_naive(const double *A, const double *B, double *C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            double s = 0.0;
            for (int k = 0; k < n; k++)
                s += A[(size_t)i * n + k] * B[(size_t)k * n + j]; // крок n по B -> cache miss
            C[(size_t)i * n + j] = s;
        }
}

__attribute__((noinline))
static void matmul_opt(const double *restrict A, const double *restrict B,
                       double *restrict C, int n) {
    memset(C, 0, (size_t)n * n * sizeof(double));
    for (int i = 0; i < n; i++)
        for (int k = 0; k < n; k++) {
            double a = A[(size_t)i * n + k];
            for (int j = 0; j < n; j++)
                C[(size_t)i * n + j] += a * B[(size_t)k * n + j]; // послідовний доступ -> SIMD
        }
}

int main(int argc, char **argv) {
    const char *mode = argc > 1 ? argv[1] : "naive";
    int n = argc > 2 ? atoi(argv[2]) : DEFAULT_N;
    double *A = alloc_matrix(n), *B = alloc_matrix(n), *C = alloc_matrix(n);
    srand(42);
    for (size_t i = 0; i < (size_t)n * n; i++) {
        A[i] = rand() / (double)RAND_MAX;
        B[i] = rand() / (double)RAND_MAX;
    }

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    if (strcmp(mode, "opt") == 0) matmul_opt(A, B, C, n);
    else                          matmul_naive(A, B, C, n);
    clock_gettime(CLOCK_MONOTONIC, &t1);

    double sum = 0;
    for (size_t i = 0; i < (size_t)n * n; i++) sum += C[i];
    printf("mode=%s n=%d checksum=%.6f time=%.3f s\n", mode, n, sum,
           (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9);
    free(A); free(B); free(C);
    return 0;
}
