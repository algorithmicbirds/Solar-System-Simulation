#include <celestial_body.h>

CelestialBody::CelestialBody(float r, float m, glm::vec3 pos, glm::vec3 vel)
    : radius(r), mass(m), position(pos), velocity(vel) {}

void CelestialBody::updateBody(float deltaTime, const glm::vec3 &force) {
  glm::vec3 acceleration = force / mass;
  velocity += acceleration * deltaTime;
  position += velocity * deltaTime;
}

// gravitational force between two celestial bodies
glm::vec3
CelestialBody::calculateGravitationalForce(const CelestialBody &other) {
  glm::vec3 direction =
      other.position - position; // vector from this body to the other body
  float distanceSquared = glm::dot(direction, direction);

  // avoid division by zero by checking if the bodies are too close
  float minDistance = radius + other.radius + 1.0f;
  if (distanceSquared < minDistance * minDistance) {
    distanceSquared = minDistance * minDistance;
  }

  // avoid divide by zero
  if (distanceSquared == 0.0f)
    return glm::vec3(0.0f);

  float forceMagnitude = (0.1f) * (mass * other.mass) / distanceSquared;
  return glm::normalize(direction) *
         forceMagnitude; // normalize direction and multiply by force magnitude
}

float CelestialBody::getDistanceTo(const CelestialBody &other) const {
  return glm::length(
      other.position -
      position); // returning the Euclidean distance between the two bodies
}
