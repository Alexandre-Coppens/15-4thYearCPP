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

    GLint vecAdd = loadComputeProgram("vecadd.comp");

    GLuint ssboA = makeSSBO(0, n * sizeof(float), A);
    GLuint ssboB = makeSSBO(1, n * sizeof(float), B);
    GLuint ssboC = makeSSBO(2, n * sizeof(float), C);

    //Can also use:     
    //glNamedBufferData(ssboA, n * sizeof(float), A, GL_DYNAMIC_COPY);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboA);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboB);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboC);

    glUseProgram(vecAdd);
    glDispatchCompute(n, 1, 1);

    glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboC);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, n * sizeof(float), C);
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
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL 4.4", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    printf("OpenGL : %s\n", glGetString(GL_VERSION));

  const int sizes[] = {0, 4, 1024, 2049, 100003, 99999};
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
  glfwTerminate();
  return 0;
}