#include <starfield.h>
#include <cstdlib>
#include <ctime>
#include <cmath>

Starfield::Starfield(unsigned int numPoints, float distance)
    : numPoints(numPoints), distance(distance) {

  srand(static_cast<unsigned int>(time(0)));

  generateStars();
}

void Starfield::generateStars() {
  vertices.clear();

  for (unsigned int i = 0; i < numPoints; ++i) {
    float theta = rand() % 360;
    float phi = acos(1.0f - 2.0f * static_cast<float>(rand()) / RAND_MAX);

    float thetaRad = glm::radians(static_cast<float>(theta));
    float phiRad = phi;

    float r = distance;

    float x = r * sin(phiRad) * cos(thetaRad);
    float y = r * sin(phiRad) * sin(thetaRad);
    float z = r * cos(phiRad);

    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(1.0f);
    vertices.push_back(1.0f);
    vertices.push_back(1.0f);
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(),
               vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void Starfield::render(Shader &shader, const glm::mat4 &projection,
                       const glm::mat4 &viewMatrix) {
  shader.use();
  shader.setMat4("projection", projection);
  shader.setMat4("view", viewMatrix);

  glBindVertexArray(VAO);
  glDrawArrays(GL_POINTS, 0, numPoints);
}

void Starfield::updateStarPositions() { generateStars(); }
