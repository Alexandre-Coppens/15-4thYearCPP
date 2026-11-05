#include "gl_utils.hpp"
#include "scene.hpp"
#include <vector>

int GetCellID(float x, float y, float z, float size, float cells)
{
  int cell = 0;
  int ix = static_cast<int>(x / size);
  int iy = static_cast<int>(y / size);
  int iz = static_cast<int>(z / size);
  int n = static_cast<int>(cells);
  return ix + n * (iy + n * iz);
}

void SetupParticleCells(std::vector<Particle>& _particles, std::vector<int>& _cellsList, std::vector<int>& _cellsNext,const int n, const float boxSize)
{
  _cellsList = std::vector<int>(_cellsList.size(), -1);
  _cellsNext = std::vector<int>(_cellsNext.size(), -1);
  for (int i = 0; i < n; i++)
  {
    Particle p = _particles[i];
    int cell = GetCellID(p.posType[0], p.posType[1], p.posType[2], 2, 32);
    
    int temp = _cellsList[cell];
    _cellsList[cell] = i;
    _cellsNext[i] = temp;
  }
}

int main() {
  if (!initWindow(800, 800, "TP2 exercice 1")) return 1;
  const int N = 4096;
  const int cells = 32 * 32 * 32;
  std::vector<Particle> state;
  initBoids(state, N, boxSize());
  GLuint current = makeSSBO(0, static_cast<GLsizeiptr>(state.size() * sizeof(Particle)), state.data());
  GLuint next = makeSSBO(1, static_cast<GLsizeiptr>(state.size() * sizeof(Particle)), 0);
  GLuint heads = makeSSBO(4, static_cast<GLsizeiptr>(cells * sizeof(int)), 0);
  makeSSBO(5, static_cast<GLsizeiptr>(N * sizeof(int)), 0);
  
  
  std::vector<int> cellsList {std::vector<int>(cells, -1)};
  std::vector<int> cellsNextList {std::vector<int>(N, -1)};
  GLuint cellHead = makeSSBO(4, static_cast<GLsizeiptr>(cellsList.size() * sizeof(int)), cellsList.data());
  GLuint cellNext = makeSSBO(5, static_cast<GLsizeiptr>(cellsNextList.size() * sizeof(int)), cellsNextList.data());
  SetupParticleCells(state, cellsList, cellsNextList, N, boxSize());
  
  GLuint build = loadComputeProgram("boids_build.comp");
  GLuint query = loadComputeProgram("boids_query.comp");
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
    glDispatchCompute(ceilDiv(static_cast<GLuint>(N), 256), 1, 1);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    glUseProgram(particles);
    glUniform1ui(glGetUniformLocation(particles, "N"), static_cast<GLuint>(N));
    glUniform1f(glGetUniformLocation(particles, "boxSize"), boxSize());
    glDispatchCompute(ceilDiv(static_cast<GLuint>(N), 256), 1, 1);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    drawPoints(next, N);
    GLuint tmp = current; current = next; next = tmp;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, current);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, next);
    pollAndSwap();
    SetupParticleCells(state, cellsList, cellsNextList, N, 32);
  }
  shutdownWindow();
  return 0;
}
