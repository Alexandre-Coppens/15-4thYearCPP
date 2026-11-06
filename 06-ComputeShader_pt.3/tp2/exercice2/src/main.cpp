#include "gl_utils.hpp"
#include "scene.hpp"
#include <vector>
#include <GLFW/glfw3.h>

double oldDt = 0;
float dt = 0.f;
void CalculateDeltaTime()
{
  double t = glfwGetTime();
  dt = t - oldDt;
  oldDt = t;
}

int main() {
  if (!initWindow(800, 800, "TP2 exercice 2")) return 1;
  
  const int N = 9999;
  const int cells = 32 * 32 * 32;
  
  std::vector<Particle> state;
  initParticles(state, N, boxSize());
  GLuint current = makeSSBO(0, static_cast<GLsizeiptr>(state.size() * sizeof(Particle)), state.data());
  GLuint next = makeSSBO(1, static_cast<GLsizeiptr>(state.size() * sizeof(Particle)), 0);
  GLuint heads = makeSSBO(4, static_cast<GLsizeiptr>(cells * sizeof(int)), 0);
  GLuint links = makeSSBO(5, static_cast<GLsizeiptr>(N * sizeof(int)), 0);
  
  GLuint build = loadComputeProgram("build_lists.comp");
  GLuint query = loadComputeProgram("query.comp");
  GLuint particles = loadComputeProgram("particles.comp");
  
  while (!windowShouldClose()) {
    clearSSBO(heads, 0xffffffffu);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(build);
    glUniform1ui(glGetUniformLocation(build, "N"), static_cast<GLuint>(N));
    glDispatchCompute(ceilDiv(static_cast<GLuint>(N), 256), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    glUseProgram(query);
    glUniform1ui(glGetUniformLocation(query, "N"), static_cast<GLuint>(N));
    glUniform1f(glGetUniformLocation(query, "attractionRadius"), 10.f);
    glUniform1f(glGetUniformLocation(query, "deltaTime"), dt);
    glDispatchCompute(ceilDiv(static_cast<GLuint>(N), 256), 1, 1);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    
    glUseProgram(particles);
    glUniform1ui(glGetUniformLocation(particles, "N"), static_cast<GLuint>(N));
    glUniform1f(glGetUniformLocation(particles, "boxSize"), boxSize());
    glDispatchCompute(ceilDiv(static_cast<GLuint>(N), 256), 1, 1);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    drawPoints(next, N);
    GLuint tmp = current;
    current = next;
    next = tmp;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, current);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, next);
    pollAndSwap();
    CalculateDeltaTime();
  }
  shutdownWindow();
  return 0;
}
