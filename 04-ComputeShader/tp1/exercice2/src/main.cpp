#include "gl_utils.hpp"

#include <vector>

int main() {
  const int W = 256;
  const int H = 256;
  GLFWwindow* window = initWindow(W, H, "TP1 exercice 2");
  
  if (!window) {
    return 1;
  }

  glewExperimental = GL_TRUE;

  if (glewInit() != GLEW_OK) {
      glfwDestroyWindow(window);
      glfwTerminate();
      return -1;
  }

  printf("OpenGL : %s\n", glGetString(GL_VERSION));

  std::vector<unsigned> grid(static_cast<size_t>(W * H), 0u);
  for (int i = 0; i < grid.size(); i++)
  {
      grid[i] = rand() % 10 < 3;
  }

  GLuint current = makeSSBO(0, static_cast<GLsizeiptr>(grid.size() * sizeof(unsigned)), grid.data());
  GLuint next = makeSSBO(1, static_cast<GLsizeiptr>(grid.size() * sizeof(unsigned)), grid.data());
  GLuint life = loadComputeProgram("life.comp");
  GLuint display = loadRenderProgram("fullscreen.vert", "display.frag");
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  (void)current;
  (void)next;

  while (!glfwWindowShouldClose(window)) {
    glUseProgram(life);
    // TODO : uniforms width et height.
    // TODO : dispatch, un groupe 16x16 par tuile (ceilDiv).
    // TODO : barriere avant le lecteur suivant (SSBO).
    // TODO : echanger current et next. Ne pas recopier le buffer.
    // TODO : slot 0 = current (lecture), slot 1 = next (ecriture).

    glUniform1ui(glGetUniformLocation(life, "width"), W);
    glUniform1ui(glGetUniformLocation(life, "height"), H);

    glDispatchCompute(ceilDiv(W, 16), ceilDiv(H, 16), 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    std::swap(current, next);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, current);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, next);

    int fbW = 0;
    int fbH = 0;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);
    glUseProgram(display);
    glUniform1ui(glGetUniformLocation(display, "width"), static_cast<GLuint>(W));
    glUniform1ui(glGetUniformLocation(display, "height"), static_cast<GLuint>(H));
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}
