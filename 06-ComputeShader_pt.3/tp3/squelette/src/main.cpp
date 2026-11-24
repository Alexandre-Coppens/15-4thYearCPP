#include "gl_utils.hpp"
#include "scene.hpp"

#include <GLFW/glfw3.h>

#include <cstdio>
#include <iostream>
#include <ostream>
#include <string>

bool initialization = true;
GLuint lightBuffer;
GLuint validationBuffer;
GLuint planeBuffer;

GLuint lightCulling;

void CullLights()
{
  std::vector<glm::vec4> planeNormals = GetCameraPlanes();
  uint32_t lightCount = lights().size();
  std::vector<GLuint> lightValidation;
  lightValidation.resize(lightCount + 1);
  if (initialization)
  {
    lightBuffer = makeSSBO(0, static_cast<GLsizeiptr>(lightCount * sizeof(PointLight)), lights().data());
    validationBuffer = makeSSBO(1, static_cast<GLsizeiptr>(lightValidation.size() * sizeof(GLuint)), lightValidation.data());
    planeBuffer = makeSSBO(2, static_cast<GLsizeiptr>(planeNormals.size() * sizeof(glm::vec4)), planeNormals.data());
    initialization = false;
  }
  clearSSBO(validationBuffer, 0);
  updateSSBO(lightBuffer, static_cast<GLsizeiptr>(lightCount * sizeof(PointLight)), lights().data());
  updateSSBO(planeBuffer, static_cast<GLsizeiptr>(planeNormals.size() * sizeof(glm::vec4)), planeNormals.data());
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  glUseProgram(lightCulling);
  glUniform1ui(glGetUniformLocation(lightCulling, "n"), static_cast<GLuint>(lightCount));
  glDispatchCompute(ceilDiv(static_cast<GLuint>(lightCount), 256), 1, 1);
  glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
  
  readSSBO(validationBuffer, static_cast<GLsizeiptr>(lightValidation.size() * sizeof(GLuint)), lightValidation.data());
  
  std::cout << lightValidation[0] << std::endl;
}

void ClusterLights()
{
  float clusterSize = 64;
  std::vector<glm::vec3> numberOfClusters = glm::vec3{gwidth};
}

int main(int argc, char** argv) {
  const char* map = "./assets/EvilCathedral.pk3";
  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "--maps" && i + 1 < argc) {
      map = argv[++i];
    }
  }
  if (!map) {
    std::fprintf(stderr, "usage: %s --maps fichier.bsp\n", argv[0]);
    return 1;
  }
  if (!initWindow(1280, 720, "Compute shaders")) {
    return 1;
  }
  if (!loadMap(map)) {
    return 1;
  }
  
  lightCulling = loadComputeProgram("LightCulling.comp");
  
  while (!windowShouldClose()) {
    updateLights(static_cast<float>(glfwGetTime()));
    CullLights();
    renderGBuffer();
    displayPreview();
    pollAndSwap();
  }
  shutdownWindow();
  return 0;
}
