#include <collision_detector.h>
#include <iostream>

CollisionDetector::CollisionDetector(RayCaster *rayCaster)
    : rayCaster(rayCaster) {}

bool CollisionDetector::checkCollisionWithTerrain(
    float mouseX, float mouseY, const glm::mat4 &projection,
    const glm::mat4 &view, const std::vector<CelestialBody> &bodies) {
  // get the ray direction for the falling ray (straight down)
  Ray ray =
      rayCaster->getRayFromScreenCoordinates(mouseX, mouseY, projection, view);

  ray.direction =
      glm::vec3(0.0f, -1.0f, 0.0f); // ray falling down along the Y-axis

  // check for collision with body
  for (const auto &body : bodies) {
    if (rayCaster->checkRayIntersection(ray, body)) {
      std::cout << "Collision detected with body at position: "
                << body.position.x << " " << body.position.y << " "
                << body.position.z << std::endl;
      return true; // collision detected
    }
  }

  return false; // no collision
}
