
#ifndef LIGHTNINGGL_SHADERPROGRAM_HH
#define LIGHTNINGGL_SHADERPROGRAM_HH

#include <GL/glew.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define TEST_OPENGL_ERROR()                                       \
  do {                                                            \
    GLenum err = glGetError();                                    \
    if (err != GL_NO_ERROR)                                       \
      std::cerr << "OpenGL ERROR!" << __LINE__ << " " << __FILE__ \
                << std::endl;                                     \
  } while (0)

class shaderType {
 public:
  shaderType(const std::string &path, GLenum type);
  shaderType();
  void createShader();

  GLuint id;
  std::string path;
  std::string data;
  char *dataPtr;
  GLenum type;

 private:
  void checkCompilationError();
  std::string loadShaderText(std::string &shaderPath);
};

class shaderProgram {
 public:
  shaderProgram(const std::vector<std::pair<std::string, GLenum>> &shaderInfo);
  virtual ~shaderProgram();

  void compileShaderAndLink();
  void activate();
  void checkLinkError();

  shaderType *program;
  std::vector<shaderType *> shaders;
};

#endif  // LIGHTNINGGL_SHADERPROGRAM_HH
