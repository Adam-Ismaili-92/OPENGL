
#include "shaderProgram.hh"

std::string shaderType::loadShaderText(std::string &shaderPath) {
  std::ifstream file(shaderPath);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void shaderType::checkCompilationError() {
  GLint isCompiled = 0;
  glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);
    TEST_OPENGL_ERROR();
    glDeleteShader(id);
    TEST_OPENGL_ERROR();

    std::cerr << data << "\n"
              << "Compilation failed !\n"
              << path << "\nError message :\n";
    for (const auto &item : errorLog) std::cerr << item;
    std::cerr << std::endl;
    exit(1);
  }
  std::cout << "Compilation done for " << path << " ! ⛏\uFE0F" << std::endl;
}

void shaderProgram::checkLinkError() {
  GLint isLinked = 0;
  glGetProgramiv(program->id, GL_LINK_STATUS, (int *)&isLinked);
  if (isLinked == GL_FALSE) {
    GLint maxLength = 0;
    glGetProgramiv(program->id, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> errorLog(maxLength);
    glGetProgramInfoLog(program->id, maxLength, &maxLength, &errorLog[0]);
    TEST_OPENGL_ERROR();

    glDeleteProgram(program->id);
    TEST_OPENGL_ERROR();
    for (const auto &item : shaders) {
      glDeleteShader(item->id);
      TEST_OPENGL_ERROR();
    }

    std::cerr << "Link failed !\nProgram number " << program->id << "\n";
    for (const auto &item : shaders) std::cerr << item->path << "\n";
    std::cerr << "\nError message :\n";
    for (const auto &item : errorLog) std::cerr << item;
    std::cerr << std::endl;
    exit(1);
  }
  std::cout << "Program number " << program->id << " linked ! ⛓\uFE0F"
            << std::endl;
}

shaderType::shaderType(const std::string &path, GLenum type)
    : path(path), type(type) {}

shaderType::shaderType() : path(""), type(GL_PROGRAM) {}

void shaderType::createShader() {
  id = glCreateShader(type);
  TEST_OPENGL_ERROR();
  data = loadShaderText(path);
  dataPtr = data.data();
  glShaderSource(id, 1, &dataPtr, nullptr);
  TEST_OPENGL_ERROR();
  glCompileShader(id);
  TEST_OPENGL_ERROR();
  checkCompilationError();
}

shaderProgram::shaderProgram(
    const std::vector<std::pair<std::string, GLenum>> &shaderInfo) {
  shaders = {};
  program = new shaderType();
  for (const auto &item : shaderInfo) {
    shaderType *sh = new shaderType(item.first, item.second);
    sh->createShader();
    shaders.push_back(sh);
  }
}

void shaderProgram::compileShaderAndLink() {
  program->id = glCreateProgram();
  TEST_OPENGL_ERROR();

  for (const auto &item : shaders) {
    glAttachShader(program->id, item->id);
    TEST_OPENGL_ERROR();
  }

  glLinkProgram(program->id);
  TEST_OPENGL_ERROR();

  checkLinkError();

  for (const auto &item : shaders) {
    glDetachShader(program->id, item->id);
    TEST_OPENGL_ERROR();
  }

  // Define uniform ssbo and stuff :)
}

void shaderProgram::activate() { glUseProgram(program->id); }

shaderProgram::~shaderProgram() {
  for (auto &shader : shaders) {
    glDetachShader(program->id, shader->id);
    TEST_OPENGL_ERROR();
    glDeleteShader(shader->id);
    TEST_OPENGL_ERROR();
    delete shader;
  }

  glDeleteProgram(program->id);
  TEST_OPENGL_ERROR();
  delete program;
}
