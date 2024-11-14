#ifndef SPHERE
#define SPHERE

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <shader_loader.h>

class Sphere {
public:
  Sphere(float radius, unsigned int latitudeCount, unsigned int longitudeCount,
         Shader *shader = nullptr);
  void generateSphere();
  void render(glm::mat4 projection, glm::mat4 view, glm::vec3 position);
  void setShader(Shader *shader);

private:
  float radius;
  unsigned int latitudeCount;
  unsigned int longitudeCount;
  GLuint sphereVAO, sphereVBO, sphereEBO;
  std::vector<float> sphereVertices;
  std::vector<unsigned int> sphereIndices;
  Shader *sphereShader;
};

#endif
