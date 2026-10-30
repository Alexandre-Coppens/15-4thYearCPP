#include "gl_utils.hpp"

#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

GLuint shader = 0;

void sumCPU(const float* v, float* sum, int n) {
  float s = 0.f;
  for (int i = 0; i < n; ++i) {
    s += v[i];
  }
  *sum = s;
}

// Pas 1, 2, 4, ... : elements espaces d'un intervalle regulier.
void reduceEntrelace(const float* v, float* sum, int n) {
  std::vector<float> x(v, v + n);
  for (int stride = 1; stride < n; stride *= 2) {
    for (int i = 0; i < n; i += stride * 2) {
      x[i] += x[i + stride];
    }
  }
  *sum = n > 0 ? x[0] : 0.f;
}

// Pas n/2, n/4, ... : l'intervalle actif est divise par deux.
void reduceMoities(const float* v, float* sum, int n) {
  std::vector<float> x(v, v + n);
  for (int stride = n / 2; stride >= 1; stride /= 2) {
    for (int i = 0; i < stride; ++i) {
      x[i] += x[i + stride];
    }
  }
  *sum = n > 0 ? x[0] : 0.f;
}

void reduce(const float* vec, float* sum, int size) {
  glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(float), vec, GL_DYNAMIC_COPY);
  
  glDispatchCompute(ceilDiv(size,1024), 1, 1);
  glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
  Make Multipass Here
  
  glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float), sum);
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

bool compare(float v1, float v2)
{
  std::cout << "[ " << v1
            << std::setw(8) << " | " << v2 
            << std::setw(8)<< "] = ";
  return v1 == v2;
}

unsigned int getEmpty(float* values, int size)
{
  unsigned int result = 0;
  for (int i = 0; i < size; ++i)
  {
    if (values[i] != 0.f) result++;
  }
  return result;
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
  
    shader = loadComputeProgram("reduce.comp");
    glUseProgram(shader);

    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

  const int sizes[] = {1024, 16384, 65536, 262144, 1048576};
  const int warmup = 10;
  const int iters = 50;

  std::cout << std::fixed << std::setprecision(4);
  std::cout << std::setw(8) << "n"
            << std::setw(14) << "|e-entrelace|"
            << std::setw(12) << "|e-moities|"
            << std::setw(10) << "|e-gpu|"
            << std::setw(12) << "CPU ms"
            << std::setw(14) << "entrelace ms"
            << std::setw(12) << "moities ms"
            << std::setw(12) << "reduce ms"
            << std::setw(24) << "comparison" << "\n";

  for (int n : sizes) {
    std::vector<float> values(static_cast<size_t>(n));
    glUniform1ui(glGetUniformLocation(shader, "size"), n);

    for (int i = 0; i < n; ++i) {
      values[i] = float(i % 17) - 8.f;
    }
    
    float cpu = 0.f;
    float entrelace = 0.f;
    float moities = 0.f;
    float gpu = 0.f;

    sumCPU(values.data(), &cpu, n);
    reduceEntrelace(values.data(), &entrelace, n);
    reduceMoities(values.data(), &moities, n);
    reduce(values.data(), &gpu, n);

    const double cpuMs = benchMs([&] { sumCPU(values.data(), &cpu, n); }, warmup, iters);
    const double entrelaceMs = benchMs([&] { reduceEntrelace(values.data(), &entrelace, n); }, warmup, iters);
    const double moitiesMs = benchMs([&] { reduceMoities(values.data(), &moities, n); }, warmup, iters);
    const double gpuMs = benchMs([&] { reduce(values.data(), &gpu, n); }, warmup, iters);
    
    std::cout << std::setw(8) << n
              << std::setw(14) << std::fabs(cpu - entrelace)
              << std::setw(12) << std::fabs(cpu - moities)
              << std::setw(10) << std::fabs(cpu - gpu)
              << std::setw(12) << cpuMs
              << std::setw(14) << entrelaceMs
              << std::setw(12) << moitiesMs
              << std::setw(12) << gpuMs
              << std::setw(8) << compare(cpu, gpu) << "\n";
  }

  return 0;
}
