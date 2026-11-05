#pragma once
#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
inline void initGLLoader() {
  glewExperimental = GL_TRUE;
  (void)glewInit();
}
#else
#include <epoxy/gl.h>
inline void initGLLoader() {}
#endif
#include <cstdint>
struct GLFWwindow;

GLuint makeSSBO(GLuint binding, GLsizeiptr bytes, const void* data = 0);
void readSSBO(GLuint buffer, GLsizeiptr bytes, void* dst);
GLuint ceilDiv(GLuint n, GLuint d);
GLuint loadComputeProgram(const char* path);
GLuint loadRenderProgram(const char* vert, const char* frag);
GLFWwindow* initWindow(int width, int height, const char* title);
