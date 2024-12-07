//
// Created by gabriel on 11/04/23.
//

#include "PoggerGL.h"

#include <iostream>
#include <map>

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "thunder.hh"

#define UNUSED(variable) (void)(variable)

GLuint VAO, VBO, splitFBO;
GLuint pingpongFBO[2];
GLuint pingpongTexture[2];
GLuint splitTexture[2];
GLuint splitRenderDepthBuffer[2];
std::map<std::string, GLint> uniformMap;
glm::vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

GLuint fakeVAO, fakeVBO;
float fakeScreenCoords[] = {
    1.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f,

    1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f};
bool horizontal = true;
GLuint inputTexture = 2;

float xSensitivity = 0.05f;
float ySensitivity = 0.05f;
float speed = 0.1f;
float verticalAngle = 0.0f;
float horizontalAngle = M_PI;
float epsSensitivity = 0.02f;

glm::vec3 direction =
    glm::vec3(std::cos(verticalAngle) * std::sin(horizontalAngle),
              std::sin(verticalAngle),
              std::cos(verticalAngle) * std::cos(horizontalAngle));
glm::vec3 right = glm::vec3(std::sin(horizontalAngle - M_PI / 2.0f), 0,
                            std::cos(horizontalAngle - M_PI / 2.0f));
glm::vec3 up = glm::cross(right, direction);

glm::vec3 position = glm::vec3(0.2f, 5.0f, 9.2f);

glm::mat4 view = glm::lookAt(position, position + direction, up);
glm::mat4 projection =
    glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
glm::mat4 pV = projection * view;

Node *mainRootNode = nullptr;
Node *secondaryRootNode = nullptr;
std::vector<glm::vec3> vertices;

unsigned frameCount = 0;
unsigned fpsCurrentTime = 0;
unsigned fpsPreviousTime = 0;
unsigned previousTime = 0;
unsigned currentTime = 0;

unsigned maxHeight = 0;
unsigned totalBufferSize = 0;
float percentPerCall = 0.08f;  // 0.08 default
float percentToDisplay = 0.01f;
float colorReduce = 0.01f;  // 0.1f default
float blurSpread = 1.0f;
float blurSpreadIncrease = 0.01f;
float xIncrease = 0.5f;
float gammaLight = 1.0f;
float exposure = 0.8f;
int amountOfBlur = 0;
float epsFactor = 1.f;  // 1.f default

std::default_random_engine gen(time(0));
std::uniform_real_distribution<float> dist(-40.0f, 40.0f);
glm::vec2 begin = {dist(gen), 200.0f};
glm::vec2 end = {dist(gen), 0.0f};
std::uniform_real_distribution<float> dist2(0.1f, 0.01f);

void calculateFPS() {
  frameCount++;
  fpsCurrentTime = glutGet(GLUT_ELAPSED_TIME);

  unsigned timeInterval = fpsCurrentTime - fpsPreviousTime;
  if (timeInterval < 1000) return;

  float fps = static_cast<float>(frameCount) /
              (static_cast<float>(timeInterval) / 1000.0f);
  frameCount = 0;
  fpsPreviousTime = fpsCurrentTime;
  std::string title =
      std::string("Pog | FPS : ") + std::to_string(static_cast<int>(fps));
  glutSetWindowTitle(title.data());
}

void updatePv() {
  direction = glm::vec3(std::cos(verticalAngle) * std::sin(horizontalAngle),
                        std::sin(verticalAngle),
                        std::cos(verticalAngle) * std::cos(horizontalAngle));
  right = glm::vec3(std::sin(horizontalAngle - M_PI / 2.0f), 0,
                    std::cos(horizontalAngle - M_PI / 2.0f));
  up = glm::cross(right, direction);

  view = glm::lookAt(position, position + direction, up);
  projection =
      glm::perspective(glm::radians(60.0f),
                       static_cast<float>(glutGet(GLUT_WINDOW_WIDTH)) /
                           static_cast<float>(glutGet(GLUT_WINDOW_HEIGHT)),
                       0.1f, 100.0f);
  pV = projection * view;
}

void updateMesh() {
  PoggerGL::programs[0]->activate();
  Node::freeFromRoot(mainRootNode);

  std::pair<Node *, unsigned int> pair = genThunder(begin, end, 4, 0, 3);
  mainRootNode = pair.first;

  addBranches(mainRootNode, begin, end, (unsigned int)maxHeight, 0, epsFactor);

  Node::freeFromRoot(secondaryRootNode);

  secondaryRootNode =
      getThunderFramesDistance(mainRootNode, begin.y, percentToDisplay);
  vertices.resize(0);

  float randomOffset = dist(gen) / 40.0f;

  std::vector<glm::vec3> part1 =
      Node::toVertices(secondaryRootNode, {randomOffset, randomOffset, 0}, 10);
  std::vector<glm::vec3> part2 = Node::toVertices(
      secondaryRootNode, {1.5 + randomOffset, randomOffset, 0}, 10);
  std::vector<glm::vec3> part3 = Node::toVertices(
      secondaryRootNode, {randomOffset, 1.5 + randomOffset, 0}, 10);
  std::vector<glm::vec3> part4 = Node::toVertices(
      secondaryRootNode, {1.5 + randomOffset, 1.5 + randomOffset, 0}, 10);

  for (unsigned i = 0; i < part1.size() - 1; i++) {
    if (part1[i].x == INFINITY || part2[i].x == INFINITY ||
        part3[i].x == INFINITY || part4[i].x == INFINITY)
      i += 1;

    if (i < part1.size() - 1) {
      vertices.push_back(part1[i]);
      vertices.push_back(part2[i]);
      vertices.push_back(part3[i]);

      vertices.push_back(part2[i]);
      vertices.push_back(part3[i]);
      vertices.push_back(part4[i]);

      vertices.push_back(part3[i]);
      vertices.push_back(part4[i]);
      vertices.push_back(part1[i + 1]);

      vertices.push_back(part1[i + 1]);
      vertices.push_back(part2[i + 1]);
      vertices.push_back(part4[i]);
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  TEST_OPENGL_ERROR();
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);
  if (color.w == 0) {
    color.w = 1.0f;
    updateUniforms();
  }
}

void printNonBlackPixels(GLuint textureID, int width, int height) {
  GLubyte *pixels = new GLubyte[width * height * 4];

  glBindTexture(GL_TEXTURE_2D, textureID);

  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      int pixelIndex = (y * width + x) * 4;

      if (pixels[pixelIndex] != 0 || pixels[pixelIndex + 1] != 0 ||
          pixels[pixelIndex + 2] != 0) {
        std::cout << "Pixel (" << x << ", " << y << "): ";
        std::cout << "R=" << static_cast<int>(pixels[pixelIndex]) << " ";
        std::cout << "G=" << static_cast<int>(pixels[pixelIndex + 1]) << " ";
        std::cout << "B=" << static_cast<int>(pixels[pixelIndex + 2])
                  << std::endl;
      }
    }
  }
  delete[] pixels;
  std::cout << "texture ID : " << textureID << std::endl;
}

void display() {
  PoggerGL::programs[0]->activate();
  TEST_OPENGL_ERROR();
  glBindFramebuffer(GL_FRAMEBUFFER, splitFBO);
  TEST_OPENGL_ERROR();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  TEST_OPENGL_ERROR();
  glBindVertexArray(VAO);
  TEST_OPENGL_ERROR();
  glEnable(GL_DEPTH_TEST);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  TEST_OPENGL_ERROR();
  glBindVertexArray(0);
  TEST_OPENGL_ERROR();
  PoggerGL::programs[1]->activate();
  TEST_OPENGL_ERROR();
  horizontal = true;
  bool first_iteration = true;
  for (int i = 0; i < amountOfBlur; i++) {
    inputTexture =
        first_iteration ? splitTexture[1] : pingpongTexture[!horizontal];
    GLuint outputBuffer = pingpongFBO[horizontal];

    glActiveTexture(GL_TEXTURE0);
    TEST_OPENGL_ERROR();
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    TEST_OPENGL_ERROR();
    GLint inputTextureLocation = glGetUniformLocation(
        PoggerGL::programs[1]->program->id, "inputTexture");
    TEST_OPENGL_ERROR();
    glUniform1i(inputTextureLocation, 0);
    TEST_OPENGL_ERROR();
    GLint blurSpreadLocation =
        glGetUniformLocation(PoggerGL::programs[1]->program->id, "blurSpread");
    TEST_OPENGL_ERROR();
    glUniform1f(blurSpreadLocation, blurSpread);
    TEST_OPENGL_ERROR();
    GLint xIncreaseLocation =
        glGetUniformLocation(PoggerGL::programs[1]->program->id, "xIncrease");
    TEST_OPENGL_ERROR();
    glUniform1f(xIncreaseLocation, xIncrease);
    TEST_OPENGL_ERROR();
    glBindFramebuffer(GL_FRAMEBUFFER, outputBuffer);
    TEST_OPENGL_ERROR();
    glProgramUniform1i(PoggerGL::programs[1]->program->id,
                       uniformMap["horizontal"], horizontal);
    TEST_OPENGL_ERROR();

    glBindVertexArray(fakeVAO);
    TEST_OPENGL_ERROR();
    glDisable(GL_DEPTH_TEST);
    TEST_OPENGL_ERROR();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    TEST_OPENGL_ERROR();
    horizontal = !horizontal;
    if (first_iteration) first_iteration = false;
  }

  PoggerGL::programs[2]->activate();
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindVertexArray(fakeVAO);
  glDisable(GL_DEPTH_TEST);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, splitTexture[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, pingpongTexture[!horizontal]);

  GLint normalSceneTextureLocation = glGetUniformLocation(
      PoggerGL::programs[2]->program->id, "normalSceneTexture");
  TEST_OPENGL_ERROR();
  glUniform1i(normalSceneTextureLocation, 0);
  TEST_OPENGL_ERROR();
  GLint bloomTextureLocation =
      glGetUniformLocation(PoggerGL::programs[2]->program->id, "bloomTexture");
  TEST_OPENGL_ERROR();
  glUniform1i(bloomTextureLocation, 1);
  TEST_OPENGL_ERROR();
  GLint gammaLocation =
      glGetUniformLocation(PoggerGL::programs[2]->program->id, "gamma");
  TEST_OPENGL_ERROR();
  glUniform1f(gammaLocation, gammaLight);
  TEST_OPENGL_ERROR();
  GLint exposureLocation =
      glGetUniformLocation(PoggerGL::programs[2]->program->id, "exposure");
  TEST_OPENGL_ERROR();
  glUniform1f(exposureLocation, exposure);
  TEST_OPENGL_ERROR();

  glDrawArrays(GL_TRIANGLES, 0, 6);
  TEST_OPENGL_ERROR();
  glutSwapBuffers();
}

void keyboardDown(unsigned char key, int x, int y) {
  UNUSED(x);
  UNUSED(y);
  switch (key) {
    case 033:
      glutLeaveMainLoop();
      break;
    case 'a':
      position += up * speed;
      break;
    case 'e':
      position -= up * speed;
      break;
    case 'z':
      position += direction * speed;
      break;
    case 'q':
      position -= right * speed;
      break;
    case 's':
      position -= direction * speed;
      break;
    case 'd':
      position += right * speed;
      break;
    case 'Z':
      verticalAngle += ySensitivity;
      break;
    case 'Q':
      horizontalAngle += xSensitivity;
      break;
    case 'S':
      verticalAngle -= ySensitivity;
      break;
    case 'D':
      horizontalAngle -= xSensitivity;
      break;
    case 'x':
      if (percentToDisplay > 1.0f && color.w == 0) {
        percentToDisplay = 0.01f;
        begin = {dist(gen), 200.0f};
        end = {dist(gen), 0.0f};
        colorReduce = dist2(gen);
      }
      return;
    case 'u':
      epsFactor = std::min(1.0f, epsFactor + epsSensitivity);
      std::cout << "epsFactor : " << epsFactor << std::endl;
      break;
    case 'j':
      epsFactor = std::max(0.5f, epsFactor - epsSensitivity);
      std::cout << "epsFactor : " << epsFactor << std::endl;
      break;
    case 'p':
      blurSpread += blurSpreadIncrease;
      ;
      std::cout << "blurSpread : " << blurSpread << std::endl;
      break;
    case 'm':
      blurSpread = std::max(1.0f, blurSpread - blurSpreadIncrease);
      std::cout << "blurSpread : " << blurSpread << std::endl;
      break;
    case 'o':
      amountOfBlur += 1;
      std::cout << "amountOfBlur : " << amountOfBlur << std::endl;
      break;
    case 'l':
      amountOfBlur = std::max(0, amountOfBlur - 1);
      std::cout << "amountOfBlur : " << amountOfBlur << std::endl;
      break;
    case 'i':
      xIncrease += 0.01f;
      std::cout << "xIncrease : " << xIncrease << std::endl;
      break;
    case 'k':
      xIncrease = std::max(0.0f, xIncrease - 0.01f);
      std::cout << "xIncrease : " << xIncrease << std::endl;
      break;
    case 'y':
      exposure += 0.01f;
      std::cout << "exposure : " << exposure << std::endl;
      break;
    case 'h':
      exposure -= 0.01f;
      std::cout << "exposure : " << exposure << std::endl;
      break;
    case 't':
      gammaLight += 0.01f;
      std::cout << "gamma : " << gammaLight << std::endl;
      break;
    case 'g':
      gammaLight -= 0.01f;
      std::cout << "gamma : " << gammaLight << std::endl;
      break;
    default:
      return;
  }

  updatePv();
  updateUniforms();
}

void scheduler() {
  currentTime = glutGet(GLUT_ELAPSED_TIME);
  unsigned timeInterval = currentTime - previousTime;
  if (timeInterval < 16) return;  // 16 because 1/60 * 1000 = 16.6666...

  previousTime = currentTime;
  calculateFPS();
  if (percentToDisplay <= 1.0f) {
    updateMesh();
    percentToDisplay += percentPerCall;
  } else if (color.w > 0 && percentToDisplay > 1.0f) {
    color.w = (color.w - colorReduce <= 0) ? 0 : color.w - colorReduce;
    updateUniforms();
  }

  glutPostRedisplay();
}

void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  TEST_OPENGL_ERROR();
  PoggerGL::programs[0]->activate();
  TEST_OPENGL_ERROR();
  // Bind splitFBO
  glBindFramebuffer(GL_FRAMEBUFFER, splitFBO);
  TEST_OPENGL_ERROR();

  for (int i = 0; i < 2; i++) {
    // Textures init
    glBindTexture(GL_TEXTURE_2D, splitTexture[i]);

    // Update the splitTexture size when we reshape de window
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    TEST_OPENGL_ERROR();

    // Bind depth buffer
    glBindRenderbuffer(GL_RENDERBUFFER, splitRenderDepthBuffer[i]);
    TEST_OPENGL_ERROR();

    // Update depth buffer size
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          glutGet(GLUT_WINDOW_WIDTH),
                          glutGet(GLUT_WINDOW_HEIGHT));
    TEST_OPENGL_ERROR();
  }
  PoggerGL::programs[1]->activate();
  TEST_OPENGL_ERROR();
  for (int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
    TEST_OPENGL_ERROR();

    // Textures init
    glBindTexture(GL_TEXTURE_2D, pingpongTexture[i]);

    // Update the splitTexture size when we reshape de window
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    TEST_OPENGL_ERROR();
  }
  PoggerGL::programs[0]->activate();
  TEST_OPENGL_ERROR();
  // Binding back default splitFBO
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  TEST_OPENGL_ERROR();

  updatePv();
  updateUniforms();
}

void init_glut(int &argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitContextVersion(4, 5);
  glutInitContextProfile(GLUT_CORE_PROFILE);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize(1841, 764);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Pog | FPS : 0");
  glEnable(GL_DEPTH_TEST);
  TEST_OPENGL_ERROR();  // z-buffer
  // glEnable(GL_CULL_FACE);
  // TEST_OPENGL_ERROR(); // backface culling
  glEnable(GL_BLEND);
  TEST_OPENGL_ERROR();  // color blending (handle opacity)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  TEST_OPENGL_ERROR();
  glClearColor(0, 0, 0, 1.0f);  // clear color
  glutKeyboardFunc(keyboardDown);
  glutDisplayFunc(display);
  glutIdleFunc(scheduler);
  glutReshapeFunc(reshape);
}

bool initGlew() { return (glewInit() == GLEW_OK); }

void updateUniforms() {
  GLuint programId = PoggerGL::programs[0]->program->id;
  PoggerGL::programs[0]->activate();
  TEST_OPENGL_ERROR();
  glProgramUniform4f(programId, uniformMap["objectColor"], color.x, color.y,
                     color.z, color.w);
  TEST_OPENGL_ERROR();
  glProgramUniformMatrix4fv(programId, uniformMap["projectionMatrix"], 1,
                            GL_FALSE, &pV[0][0]);
  TEST_OPENGL_ERROR();
  glProgramUniform3f(programId, uniformMap["cameraUp"], up.x, up.y, up.z);
  TEST_OPENGL_ERROR();
  glProgramUniform3f(programId, uniformMap["cameraRight"], right.x, right.y,
                     right.z);
  TEST_OPENGL_ERROR();
  programId = PoggerGL::programs[1]->program->id;
  PoggerGL::programs[1]->activate();
  TEST_OPENGL_ERROR();
  glProgramUniform1i(programId, uniformMap["horizontal"], horizontal);
  TEST_OPENGL_ERROR();
}

void defineUniforms() {
  GLuint programId = PoggerGL::programs[0]->program->id;
  PoggerGL::programs[0]->activate();
  TEST_OPENGL_ERROR();
  GLint varLocation = glGetUniformLocation(programId, "objectColor");
  TEST_OPENGL_ERROR();
  uniformMap["objectColor"] = varLocation;

  varLocation = glGetUniformLocation(programId, "projectionMatrix");
  TEST_OPENGL_ERROR();
  uniformMap["projectionMatrix"] = varLocation;

  varLocation = glGetUniformLocation(programId, "cameraUp");
  TEST_OPENGL_ERROR();
  uniformMap["cameraUp"] = varLocation;

  varLocation = glGetUniformLocation(programId, "cameraRight");
  TEST_OPENGL_ERROR();
  uniformMap["cameraRight"] = varLocation;

  programId = PoggerGL::programs[1]->program->id;
  PoggerGL::programs[1]->activate();
  TEST_OPENGL_ERROR();

  varLocation = glGetUniformLocation(programId, "horizontal");
  TEST_OPENGL_ERROR();
  uniformMap["horizontal"] = varLocation;

  updateUniforms();
}

PoggerGL::PoggerGL(int &argc, char *argv[]) {
  init_glut(argc, argv);
  if (!initGlew()) {
    std::cerr << "initGlew failed !" << std::endl;
    exit(1);
  }

  std::vector<std::pair<std::string, GLenum>> splitDesc = {
      {"../src/shaders/splitVertexShader.vsh", GL_VERTEX_SHADER},
      {"../src/shaders/splitFragmentShader.fsh", GL_FRAGMENT_SHADER},
  };

  shaderProgram *splitProgramPtr = new shaderProgram(splitDesc);
  splitProgramPtr->compileShaderAndLink();
  programs.push_back(splitProgramPtr);
  splitProgramPtr->activate();
  TEST_OPENGL_ERROR();

  std::vector<std::pair<std::string, GLenum>> blurDesc = {
      {"../src/shaders/blurringVertexShader.vsh", GL_VERTEX_SHADER},
      {"../src/shaders/blurringFragmentShader.fsh", GL_FRAGMENT_SHADER},
  };
  shaderProgram *blurProgramPtr = new shaderProgram(blurDesc);
  blurProgramPtr->compileShaderAndLink();
  programs.push_back(blurProgramPtr);
  blurProgramPtr->activate();
  TEST_OPENGL_ERROR();

  defineUniforms();

  std::vector<std::pair<std::string, GLenum>> mergeDesc = {
      {"../src/shaders/mergeVertexShader.vsh", GL_VERTEX_SHADER},
      {"../src/shaders/mergeFragmentShader.fsh", GL_FRAGMENT_SHADER},
  };
  shaderProgram *mergeProgramPtr = new shaderProgram(mergeDesc);
  mergeProgramPtr->compileShaderAndLink();
  programs.push_back(mergeProgramPtr);

  initSSBO();
  atexit(cleanProgram);
  glutMainLoop();
}

void PoggerGL::cleanProgram() {
  for (const auto &item : programs) delete item;

  glDeleteVertexArrays(1, &VAO);
  TEST_OPENGL_ERROR();
  glDeleteBuffers(1, &VBO);
  TEST_OPENGL_ERROR();
  glDeleteFramebuffers(1, &splitFBO);
  TEST_OPENGL_ERROR();
  glDeleteFramebuffers(2, pingpongFBO);
  TEST_OPENGL_ERROR();
  glDeleteRenderbuffers(2, splitRenderDepthBuffer);
  TEST_OPENGL_ERROR();
  glDeleteTextures(2, splitTexture);
  TEST_OPENGL_ERROR();
  glDeleteTextures(2, pingpongTexture);
  TEST_OPENGL_ERROR();
  glDeleteVertexArrays(1, &fakeVAO);
  TEST_OPENGL_ERROR();
  glDeleteBuffers(1, &fakeVBO);
  TEST_OPENGL_ERROR();

  Node::freeFromRoot(mainRootNode);
  Node::freeFromRoot(secondaryRootNode);
}

void PoggerGL::initSSBO() {
  // fakeScreenInit
  programs[1]->activate();
  TEST_OPENGL_ERROR();
  glGenVertexArrays(1, &fakeVAO);
  TEST_OPENGL_ERROR();
  glBindVertexArray(fakeVAO);
  TEST_OPENGL_ERROR();
  glGenBuffers(1, &fakeVBO);
  TEST_OPENGL_ERROR();
  glBindBuffer(GL_ARRAY_BUFFER, fakeVBO);
  TEST_OPENGL_ERROR();
  glBufferData(GL_ARRAY_BUFFER, sizeof(fakeScreenCoords), &fakeScreenCoords,
               GL_STATIC_DRAW);
  TEST_OPENGL_ERROR();
  glEnableVertexAttribArray(0);
  TEST_OPENGL_ERROR();
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
  TEST_OPENGL_ERROR();
  glEnableVertexAttribArray(1);
  TEST_OPENGL_ERROR();
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  TEST_OPENGL_ERROR();

  programs[0]->activate();
  TEST_OPENGL_ERROR();

  std::pair<Node *, unsigned int> pair = genThunder(begin, end, 4, 0, 3);
  mainRootNode = pair.first;
  maxHeight = pair.second;

  addBranches(mainRootNode, begin, end, (unsigned int)maxHeight, 0, epsFactor);

  std::vector<glm::vec3> part1 = Node::toVertices(mainRootNode, {0, 0, 0}, 10);
  std::vector<glm::vec3> part2 = Node::toVertices(mainRootNode, {1, 0, 0}, 10);
  std::vector<glm::vec3> part3 = Node::toVertices(mainRootNode, {0, 1, 0}, 10);
  std::vector<glm::vec3> part4 = Node::toVertices(mainRootNode, {1, 1, 0}, 10);

  for (unsigned i = 0; i < part1.size() - 1; i++) {
    if (part1[i].x == INFINITY || part2[i].x == INFINITY ||
        part3[i].x == INFINITY || part4[i].x == INFINITY)
      i += 1;

    if (i < part1.size() - 1) {
      vertices.push_back(part1[i]);
      vertices.push_back(part2[i]);
      vertices.push_back(part3[i]);

      vertices.push_back(part2[i]);
      vertices.push_back(part3[i]);
      vertices.push_back(part4[i]);

      vertices.push_back(part3[i]);
      vertices.push_back(part4[i]);
      vertices.push_back(part1[i + 1]);

      vertices.push_back(part1[i + 1]);
      vertices.push_back(part2[i + 1]);
      vertices.push_back(part4[i]);
    }
  }

  glGenVertexArrays(1, &VAO);
  TEST_OPENGL_ERROR();
  glBindVertexArray(VAO);
  TEST_OPENGL_ERROR();

  glGenBuffers(1, &VBO);
  TEST_OPENGL_ERROR();
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  TEST_OPENGL_ERROR();
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);
  totalBufferSize = sizeof(glm::vec3) * vertices.size();
  TEST_OPENGL_ERROR();
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
  TEST_OPENGL_ERROR();
  glEnableVertexAttribArray(0);
  TEST_OPENGL_ERROR();

  // splitFBO creation & binding
  glGenFramebuffers(1, &splitFBO);
  TEST_OPENGL_ERROR();
  glBindFramebuffer(GL_FRAMEBUFFER, splitFBO);
  TEST_OPENGL_ERROR();

  // Init depth render buffers
  glGenRenderbuffers(2, splitRenderDepthBuffer);
  TEST_OPENGL_ERROR();
  // Init textures
  glGenTextures(2, splitTexture);
  TEST_OPENGL_ERROR();

  for (int i = 0; i < 2; i++) {
    // Textures init
    glBindTexture(GL_TEXTURE_2D, splitTexture[i]);
    TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, glutGet(GLUT_WINDOW_WIDTH),
                 glutGet(GLUT_WINDOW_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 nullptr);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    TEST_OPENGL_ERROR();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D, splitTexture[i], 0);
    TEST_OPENGL_ERROR();

    // Depth buffer inits
    TEST_OPENGL_ERROR();
    glBindRenderbuffer(GL_RENDERBUFFER, splitRenderDepthBuffer[i]);
    TEST_OPENGL_ERROR();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          glutGet(GLUT_WINDOW_WIDTH),
                          glutGet(GLUT_WINDOW_HEIGHT));
    TEST_OPENGL_ERROR();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, splitRenderDepthBuffer[i]);
    TEST_OPENGL_ERROR();
  }

  unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, attachments);
  TEST_OPENGL_ERROR();

  // Checking that the splitFBO created is correctly initialized
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Frame buffer object for split rendering correctly "
                 "initialized ! ✔\uFE0F"
              << std::endl;

  PoggerGL::programs[1]->activate();
  TEST_OPENGL_ERROR();
  // pingpongFBO creation
  glGenFramebuffers(2, pingpongFBO);
  TEST_OPENGL_ERROR();
  // Init pingpongTextures
  glGenTextures(2, pingpongTexture);
  TEST_OPENGL_ERROR();

  for (int i = 0; i < 2; i++) {
    // pingpongFBO binding
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
    TEST_OPENGL_ERROR();

    // Textures init
    glBindTexture(GL_TEXTURE_2D, pingpongTexture[i]);
    TEST_OPENGL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, glutGet(GLUT_WINDOW_WIDTH),
                 glutGet(GLUT_WINDOW_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 nullptr);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    TEST_OPENGL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    TEST_OPENGL_ERROR();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           pingpongTexture[i], 0);
    TEST_OPENGL_ERROR();

    // Checking that the splitFBO created is correctly initialized
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
      std::cout << "Frame buffer object " << i
                << " for pingpongging correctly initialized ! ✔\uFE0F"
                << std::endl;

    // No depth buffer here since it's for pingpongging
  }
  PoggerGL::programs[0]->activate();
  TEST_OPENGL_ERROR();
  // Binding back default splitFBO
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  TEST_OPENGL_ERROR();
}
