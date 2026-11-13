
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

constexpr int DEFAULT_N = 1024;


using Matrix = std::vector<double>;

__attribute__((noinline))
static void matmul_naive(const double *A, const double *B, double *C, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            double s = 0.0;
            for (int k = 0; k < n; k++)
                s += A[std::size_t(i) * n + k] * B[std::size_t(k) * n + j]; 
            C[std::size_t(i) * n + j] = s;
        }
}

__attribute__((noinline))
static void matmul_opt(const double *__restrict A, const double *__restrict B,
                       double *__restrict C, int n) {
    std::fill(C, C + std::size_t(n) * n, 0.0);
    for (int i = 0; i < n; i++)
        for (int k = 0; k < n; k++) {
            const double a = A[std::size_t(i) * n + k];
            for (int j = 0; j < n; j++)
                C[std::size_t(i) * n + j] += a * B[std::size_t(k) * n + j]; 
        }
}

int main(int argc, char **argv) {
    const char *mode = argc > 1 ? argv[1] : "naive";
    const int n = argc > 2 ? std::atoi(argv[2]) : DEFAULT_N;
    const std::size_t total = std::size_t(n) * n;

    Matrix A(total), B(total), C(total);
    std::srand(42);
    for (std::size_t i = 0; i < total; i++) {
        A[i] = std::rand() / static_cast<double>(RAND_MAX);
        B[i] = std::rand() / static_cast<double>(RAND_MAX);
    }

    const auto t0 = std::chrono::steady_clock::now();
    if (std::strcmp(mode, "opt") == 0) matmul_opt(A.data(), B.data(), C.data(), n);
    else                               matmul_naive(A.data(), B.data(), C.data(), n);
    const auto t1 = std::chrono::steady_clock::now();

    double sum = 0;
    for (double x : C) sum += x;
    std::printf("mode=%s n=%d checksum=%.6f time=%.3f s\n", mode, n, sum,
                std::chrono::duration<double>(t1 - t0).count());
    return 0;
}
