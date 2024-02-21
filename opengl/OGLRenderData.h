#pragma once
#include <glm/glm.hpp>
#include <vector>

/* Due to the usage of GLM, the data is organized in system memory
 * the same way it would be in GPU memory, allowing a simple copy to transfer vertex data.
 */
struct OGLVertex {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec2 uv;
};

struct OGLMesh {
  std::vector<OGLVertex> vertices;
};