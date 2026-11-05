#include "gl_utils.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
std::string readFile(const std::string& path) {
  std::ifstream in(path.c_str());
  if (!in) {
    throw std::runtime_error("lecture impossible : " + path);
  }
  std::stringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

GLuint compile(GLenum type, const std::string& src, const std::string& name) {
  GLuint shader = glCreateShader(type);
  const char* c = src.c_str();
  glShaderSource(shader, 1, &c, 0);
  glCompileShader(shader);
  GLint ok = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[2048];
    glGetShaderInfoLog(shader, sizeof(log), 0, log);
    throw std::runtime_error(name + " : " + log);
  }
  return shader;
}

GLuint link(GLuint a, GLuint b) {
  GLuint program = glCreateProgram();
  glAttachShader(program, a);
  if (b) {
    glAttachShader(program, b);
  }
  glLinkProgram(program);
  GLint ok = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &ok);
  if (!ok) {
    char log[2048];
    glGetProgramInfoLog(program, sizeof(log), 0, log);
    throw std::runtime_error(std::string("link : ") + log);
  }
  glDeleteShader(a);
  if (b) {
    glDeleteShader(b);
  }
  return program;
}

std::string shaderPath(const char* path) {
  std::ifstream direct(path);
  if (direct) {
    return path;
  }
  return std::string(SHADER_DIR) + "/" + path;
}
}

GLuint makeSSBO(GLuint binding, GLsizeiptr bytes, const void* data) {
  GLuint buffer = 0;
  return buffer;
}

void readSSBO(GLuint buffer, GLsizeiptr bytes, void* dst) {
}

GLuint ceilDiv(GLuint n, GLuint d) { return d == 0 ? 0 : (n + d - 1) / d; }

GLuint loadComputeProgram(const char* path) {
  return link(compile(GL_COMPUTE_SHADER, readFile(shaderPath(path)), path), 0);
}
