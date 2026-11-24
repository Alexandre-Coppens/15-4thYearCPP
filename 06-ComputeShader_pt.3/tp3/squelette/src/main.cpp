#include "gl_utils.hpp"
#include "scene.hpp"

#include <GLFW/glfw3.h>

#include <cstdio>
#include <iostream>
#include <ostream>
#include <string>

struct AABB
{
  glm::vec4 min;
  glm::vec4 max;
};

//Inits
bool cullingInitialization = true;
bool clusteringInitialization = true;

//Buffers
GLuint lightBuffer;
GLuint validationBuffer;
GLuint planeBuffer;
GLuint clusterBuffer;

//Culling
GLuint lightCulling;

//Clustering
glm::vec3 clusterNumber;
glm::vec3 clusterSize;
std::vector<AABB> lightClusters;

void CullLights()
{
  std::vector<glm::vec4> planeNormals = GetCameraPlanes();
  uint32_t lightCount = lights().size();
  std::vector<GLuint> lightValidation;
  lightValidation.resize(lightCount + 1);
  if (cullingInitialization)
  {
    lightBuffer = makeSSBO(0, static_cast<GLsizeiptr>(lightCount * sizeof(PointLight)), lights().data());
    validationBuffer = makeSSBO(1, static_cast<GLsizeiptr>(lightValidation.size() * sizeof(GLuint)), lightValidation.data());
    planeBuffer = makeSSBO(2, static_cast<GLsizeiptr>(planeNormals.size() * sizeof(glm::vec4)), planeNormals.data());
    cullingInitialization = false;
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
  if (clusteringInitialization)
  {
    clusterSize.x = 64.f;
    clusterSize.y = 64.f;
    clusterNumber.z = 64;
    
    clusterNumber.x = static_cast<float>(windowWidth()) / clusterSize.x;
    clusterNumber.y = static_cast<float>(windowHeight()) / clusterSize.y;
    clusterSize.z = (getFarFrustrumDistance() - getNearFrustrumDistance()) / clusterNumber.z;
    
    lightClusters.resize(clusterNumber.x * clusterNumber.y * clusterNumber.z * 3 * 2);
    clusterBuffer = makeSSBO(3, static_cast<GLsizeiptr>(lightClusters.size() * sizeof(glm::vec3)), lightClusters.data());
  }
  
  clearSSBO(clusterBuffer, 0);
  updateSSBO(clusterBuffer, static_cast<GLsizeiptr>(lightClusters.size() * sizeof(glm::vec3)), lightClusters.data());
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  glUseProgram(lightCulling);
  glUniform1ui(glGetUniformLocation(lightCulling, "n"), static_cast<GLuint>(lightCount));
  glDispatchCompute(ceilDiv(static_cast<GLuint>(lightCount), 256), 1, 1);
  glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
  
  readSSBO(validationBuffer, static_cast<GLsizeiptr>(lightValidation.size() * sizeof(GLuint)), lightValidation.data());
  
  std::cout << lightValidation[0] << std::endl;
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
