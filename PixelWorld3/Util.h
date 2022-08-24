#pragma once

#include <glm/glm.hpp>
#include <vector>



typedef struct _node {
    std::vector<int> domain;
    glm::vec3 color;
    int entropy;
    glm::vec2 pos;
    bool queued;
} node;



namespace Util {
    double lerp(double a, double b, double t);
    glm::vec3 lerp3D(glm::vec3 a, glm::vec3 b, double t);
    bool NodeCompare(node* a, node* b);
}
