#ifndef CELESTIAL_BODY_H
#define CELESTIAL_BODY_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CelestialBody {
public:
  glm::vec3 position;
  glm::vec3 velocity;
  float mass;
  float radius;
  CelestialBody(float r, float m, glm::vec3 pos, glm::vec3 vel);
  void updateBody(float deltaTime, const glm::vec3 &force);
  glm::vec3 calculateGravitationalForce(const CelestialBody &other);
  float getDistanceTo(const CelestialBody &other) const;
};

#endif
