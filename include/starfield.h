#ifndef STARFIELD
#define STARFIELD

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include <shader_loader.h>

class Starfield {
public:
  Starfield(unsigned int numPoints, float distance);

  void render(Shader &shader, const glm::mat4 &projection,
              const glm::mat4 &viewMatrix);
  void updateStarPositions();

private:
  unsigned int numPoints;
  float distance;
  std::vector<float> vertices;
  unsigned int VBO, VAO;

  void generateStars();
};

#endif
