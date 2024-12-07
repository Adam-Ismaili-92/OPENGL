
#ifndef LIGHTNINGGL_THUNDER_HH
#define LIGHTNINGGL_THUNDER_HH

#include <deque>
#include <iostream>
#include <random>
#include <utility>

#include "Node.hh"
#define UNUSED(variable) (void)(variable)

std::pair<Node *, unsigned int> genThunder(glm::vec2 start, glm::vec2 end,
                                           unsigned nbMidPoint, unsigned depth,
                                           unsigned maxDepth);

Node *getThunderFramesDistance(Node *root, float heightPos,
                               float frameSizePercentage);

void addBranches(Node *root, glm::vec2 start, glm::vec2 end,
                 unsigned int max_height, unsigned int height,
                 float eps_factor);

#endif  // LIGHTNINGGL_THUNDER_HH
