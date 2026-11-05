#include "gl_utils.hpp"

#include <chrono>
#include <iostream>
#include <vector>

void vecAddCPU(const float* A, const float* B, float* C, int n) {
  for (int i = 0; i < n; ++i) {
    C[i] = A[i] + B[i];
  }
}

void vecADD(const float* A, const float* B, float* C, int n) {
  (void)A;
  (void)B;
  (void)C;
  (void)n;
}

template <typename F>
double benchMs(F f, int warmup, int iters) {
  for (int i = 0; i < warmup; ++i) {
    f();
  }
  const std::chrono::steady_clock::time_point t0 = std::chrono::steady_clock::now();
  for (int i = 0; i < iters; ++i) {
    f();
  }
  const std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
  return std::chrono::duration<double, std::milli>(t1 - t0).count() / iters;
}

int mismatches(const std::vector<float>& a, const std::vector<float>& b) {
  int count = 0;
  for (std::size_t i = 0; i < a.size(); ++i) {
    if (a[i] != b[i]) {
      ++count;
    }
  }
  return count;
}

int main() {
  const int sizes[] = {0, 4, 1024, 2049, 100003, 99999999999};
  const int warmup = 10;
  const int iters = 50;
  for (int n : sizes) {
    std::vector<float> A(n), B(n), cpu(n), gpu(n, 0.f);
    for (int i = 0; i < n; ++i) {
      A[i] = float(i % 17);
      B[i] = float((i * 3) % 13);
    }
    vecAddCPU(A.data(), B.data(), cpu.data(), n);
    vecADD(A.data(), B.data(), gpu.data(), n);
    const double cpuMs = benchMs([&] { vecAddCPU(A.data(), B.data(), cpu.data(), n); }, warmup, iters);
    const double gpuMs = benchMs([&] { vecADD(A.data(), B.data(), gpu.data(), n); }, warmup, iters);
    std::cout << "n=" << n << " ecarts=" << mismatches(cpu, gpu)
              << " CPU " << cpuMs << " ms  vecADD " << gpuMs << " ms\n";
  }
  return 0;
}
