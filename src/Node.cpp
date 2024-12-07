
#include "Node.hh"

#include <iostream>

Node::Node(const glm::vec2 &position) : position(position), child({}) {}

Node::Node(const glm::vec2 &position, const std::vector<Node *> &child)
    : position(position), child(child) {}

void Node::freeFromRoot(Node *rootNode) {
  if (rootNode == nullptr) return;

  for (const auto &item : rootNode->child) freeFromRoot(item);
  delete rootNode;
}

Node *Node::copyFromRoot(Node *rootNode, Node *rootNodeCpy) {
  rootNodeCpy = new Node(rootNode->position);

  Node *currNode = rootNode;
  Node *currNodeCpy = rootNodeCpy;

  for (size_t i = 0; i < currNode->child.size(); i++) {
    Node *currNodeCpy_child_i = nullptr;
    currNodeCpy_child_i = copyFromRoot(currNode->child[i], currNodeCpy_child_i);
    currNodeCpy->child.push_back(currNodeCpy_child_i);
  }

  return rootNodeCpy;
}

void Node::print(Node *rootNode, bool first) {
  if (rootNode == nullptr) return;

  std::cout << "current : (" << rootNode->position.x << " ,"
            << rootNode->position.y << ")\nchild : ";
  if (rootNode->child.empty()) std::cout << "None";
  for (const auto &item : rootNode->child) print(item, false);

  if (first) std::cout << std::endl;
}

glm::vec2 Node::getLeaf(Node *root) {
  Node *currentNode = root;

  while (currentNode->child.size() > 0) currentNode = currentNode->child[0];

  return currentNode->position;
}

std::vector<glm::vec3> Node::toVertices(Node *rootNode, const glm::vec3 &offSet,
                                        float reduceFactor) {
  std::vector<glm::vec3> ret = {};
  ret.push_back((glm::vec3(rootNode->position, 0) + offSet) / reduceFactor);

  std::vector<glm::vec3> intermediateRet;

  bool moreThanOneChild = false;
  if (rootNode->child.size() > 1) moreThanOneChild = true;

  for (size_t i = 0; i < rootNode->child.size(); i++) {
    const auto &item = rootNode->child[i];

    intermediateRet = toVertices(item, offSet, reduceFactor);
    // if (i > 0 && moreThanOneChild)
    // intermediateRet.insert(intermediateRet.begin(),
    // (glm::vec3(rootNode->position, 0) + offSet) / reduceFactor);

    for (const auto &item2 : intermediateRet) ret.push_back(item2);

    if (moreThanOneChild) {
      intermediateRet.insert(
          intermediateRet.begin(),
          (glm::vec3(rootNode->position, 0) + offSet) / reduceFactor);
      ret.push_back(glm::vec3(INFINITY, 0.f, 0.f));
    }
  }
  return ret;
}
