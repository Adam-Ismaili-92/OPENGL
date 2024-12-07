
#ifndef LIGHTNINGGL_NODE_HH
#define LIGHTNINGGL_NODE_HH

#include <GL/glew.h>

#include <vector>

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

#ifndef INFINITY
#define INFINITY ((float)(1e+300 * 1e+300))
#endif

class Node {
 public:
  explicit Node(const glm::vec2 &position);
  Node(const glm::vec2 &position, const std::vector<Node *> &child);

  static void freeFromRoot(Node *rootNode);
  static void print(Node *rootNode, bool first);
  static std::vector<glm::vec3> toVertices(Node *rootNode,
                                           const glm::vec3 &offSet,
                                           float reduceFactor);

  static Node *copyFromRoot(Node *rootNode, Node *rootNodeCpy);

  static glm::vec2 getLeaf(Node *root);
  glm::vec2 position;
  std::vector<Node *> child;
};

#endif  // LIGHTNINGGL_NODE_HH
