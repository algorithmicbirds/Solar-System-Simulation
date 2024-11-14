#include <sphere.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Sphere::Sphere(float radius, unsigned int latitudeCount,
               unsigned int longitudeCount, Shader *shader)
    : radius(radius), latitudeCount(latitudeCount),
      longitudeCount(longitudeCount), sphereShader(shader) {
  generateSphere();
}

void Sphere::generateSphere() {
  sphereVertices.clear();
  sphereIndices.clear();

  for (unsigned int lat = 0; lat <= latitudeCount; ++lat) {
    float theta = lat * glm::pi<float>() / latitudeCount;
    for (unsigned int lon = 0; lon <= longitudeCount; ++lon) {
      float phi = lon * 2 * glm::pi<float>() / longitudeCount;

      float x = radius * sin(theta) * cos(phi);
      float y = radius * sin(theta) * sin(phi);
      float z = radius * cos(theta);

      glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));

      sphereVertices.push_back(x);
      sphereVertices.push_back(y);
      sphereVertices.push_back(z);
      sphereVertices.push_back(normal.x);
      sphereVertices.push_back(normal.y);
      sphereVertices.push_back(normal.z);
    }
  }

  for (unsigned int lat = 0; lat < latitudeCount; ++lat) {
    for (unsigned int lon = 0; lon < longitudeCount; ++lon) {
      unsigned int first = lat * (longitudeCount + 1) + lon;
      unsigned int second = first + longitudeCount + 1;

      sphereIndices.push_back(first);
      sphereIndices.push_back(second);
      sphereIndices.push_back(first + 1);

      sphereIndices.push_back(second);
      sphereIndices.push_back(second + 1);
      sphereIndices.push_back(first + 1);
    }
  }

  glGenVertexArrays(1, &sphereVAO);
  glGenBuffers(1, &sphereVBO);
  glGenBuffers(1, &sphereEBO);

  glBindVertexArray(sphereVAO);

  glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sphereVertices.size(),
               sphereVertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(unsigned int) * sphereIndices.size(),
               sphereIndices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

void Sphere::render(glm::mat4 projection, glm::mat4 view, glm::vec3 position) {
  if (sphereShader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(1.0f));

    sphereShader->use();
    sphereShader->setMat4("projection", projection);
    sphereShader->setMat4("view", view);
    sphereShader->setMat4("model", model);

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }
}

void Sphere::setShader(Shader *shader) { sphereShader = shader; }
