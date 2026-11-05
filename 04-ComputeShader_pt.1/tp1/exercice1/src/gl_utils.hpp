#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdint>
GLuint makeSSBO(GLuint binding, GLsizeiptr bytes, const void* data = 0);
void readSSBO(GLuint buffer, GLsizeiptr bytes, void* dst);
GLuint ceilDiv(GLuint n, GLuint d);
GLuint loadComputeProgram(const char* path);
