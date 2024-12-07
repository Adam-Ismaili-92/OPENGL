
#include "thunder.hh"

#include <iostream>

std::default_random_engine generator(time(0));
std::uniform_real_distribution<float> distribution(0.01f, 1.0f);
std::uniform_real_distribution<float> distribution2(10.f, 15.f);
std::uniform_real_distribution<float> distribution3(0.1f, 0.5f);

std::pair<Node *, unsigned int> genThunder(glm::vec2 start, glm::vec2 end,
                                           unsigned nbMidPoint, unsigned depth,
                                           unsigned maxDepth) {
  if (depth == maxDepth)
    return (end.y == 0) ? std::pair(new Node(start, {new Node(end)}), 2)
                        : std::pair(new Node(start), 1);

  glm::vec2 vec = (end - start) / (static_cast<float>(nbMidPoint) + 1.0f);

  std::vector<glm::vec2> pos = {};
  for (unsigned i = 0; i < nbMidPoint + 2; i++) {
    glm::vec2 randomOffset =
        (i == 0 || i == nbMidPoint + 1)
            ? glm::vec2(0, 0)
            : glm::vec2((distribution(generator) * 2.0f - 1.0f) * vec.x *
                            (static_cast<float>(maxDepth) /
                             (static_cast<float>(std::sqrt(depth)) + 1.0f)),
                        (distribution(generator) - 0.2f) * vec.y);
    pos.push_back(start + randomOffset + static_cast<float>(i) * vec);
  }

  Node *ret = nullptr;
  unsigned int size = 0;

  for (unsigned i = 0; i < pos.size() - 1; i++) {
    std::pair<Node *, unsigned int> p =
        genThunder(pos[i], pos[i + 1], nbMidPoint, depth + 1, maxDepth);
    Node *chunk = p.first;
    unsigned int currSize = p.second;

    size += currSize;

    if (ret == nullptr)
      ret = chunk;
    else {
      Node *tmp = ret;
      while (!tmp->child.empty()) tmp = tmp->child[0];
      tmp->child.push_back(chunk);
    }
  }
  return std::pair(ret, size);
}

void addBranches(Node *root, glm::vec2 start, glm::vec2 end,
                 unsigned int max_height, unsigned int height,
                 float eps_factor) {
  if (height >= max_height || root->child.size() == 0) return;

  float normalized_height = static_cast<float>(static_cast<float>(height) /
                                               static_cast<float>(max_height)) *
                            5.f;

  std::uniform_real_distribution<float> distributionEps(normalized_height, 5.f);
  float eps = distributionEps(generator) * eps_factor;

  float proba_1 = std::exp(-1. * (normalized_height + eps));

  if (proba_1 >= 0.2f) {
    glm::vec2 vec = end - start;

    float adj_length = std::sqrt(std::pow(vec.x, 2) + std::pow(vec.y, 2));

    float angle = 3.14f / distribution2(generator);

    float hyp_length = adj_length / std::cos(angle);

    float opp_length = std::sin(angle) * hyp_length;

    float new_x = end.x;
    float new_y = start.y * distribution3(generator);

    float proba_2 = distribution(generator);

    if (proba_2 >= 0.5f)
      new_x -= opp_length;

    else
      new_x += opp_length;

    glm::vec2 new_start = root->position;

    glm::vec2 new_end = glm::vec2(new_x, new_y);

    std::pair<Node *, unsigned int> p = genThunder(new_start, new_end, 4, 0, 3);

    Node *child_root = p.first;

    if (child_root->child.size() > 0)
      root->child.push_back(child_root->child[0]);
  }

  addBranches(root->child[0], root->child[0]->position, end, max_height,
              height + 1, eps_factor);

  return;
}

Node *getThunderFramesDistance(Node *root, float heightPos,
                               float frameSizePercentage) {
  Node *newRoot = nullptr;

  newRoot = Node::copyFromRoot(root, newRoot);

  std::deque<Node *> queue = {newRoot};

  float heightLimit = (heightPos * (1 - frameSizePercentage));

  while (queue.size() > 0) {
    Node *currentNode = queue.front();
    queue.pop_front();

    size_t i = 0;
    while (i < currentNode->child.size()) {
      if (currentNode->child[i]->position.y < heightLimit) {
        Node::freeFromRoot(currentNode->child[i]);

        currentNode->child.erase(currentNode->child.begin() + i);

        i -= 1;
      } else
        queue.push_back(currentNode->child[i]);

      i += 1;
    }
  }

  return newRoot;
}
