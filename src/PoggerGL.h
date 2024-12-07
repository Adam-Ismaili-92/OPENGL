//
// Created by gabriel on 11/04/23.
//

#ifndef OPENGL_PROJET_FOUDRE_POGGERGL_H
#define OPENGL_PROJET_FOUDRE_POGGERGL_H
// clang-format off
#include <GL/glew.h>
#include <GL/freeglut.h>
// clang-format on
#include <string>
#include <vector>

#include "shaderProgram.hh"

#define TEST_OPENGL_ERROR()                                       \
  do {                                                            \
    GLenum err = glGetError();                                    \
    if (err != GL_NO_ERROR)                                       \
      std::cerr << "OpenGL ERROR!" << __LINE__ << " " << __FILE__ \
                << std::endl;                                     \
  } while (0)

void updateUniforms();
void defineUniforms();

class PoggerGL {
 public:
  static void cleanProgram();

  PoggerGL(int &argc, char *argv[]);

  static inline std::vector<shaderProgram *> programs = {};

  static void initSSBO();
};

#endif  // OPENGL_PROJET_FOUDRE_POGGERGL_H
