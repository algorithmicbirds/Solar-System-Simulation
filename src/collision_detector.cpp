#include <collision_detector.h>
#include <raycaster.h>
#include <iostream>

CollisionDetector::CollisionDetector(RayCaster *rayCaster)
    : rayCaster(rayCaster) {}

void CollisionDetector::resolveCameraCollisions(
    Camera &camera, std::vector<CelestialBody> &bodies) {
  for (auto &body : bodies) {
    // calculate the direction from the celestial body center to the camera
    glm::vec3 cameraToBody = camera.position - body.position;
    float distance = glm::length(cameraToBody);

    /** if the camera is inside the body **/

    if (distance < body.radius) {
      // calculate how much the camera is inside the body
      float overlap = body.radius - distance;

      // move the camera exactly to the surface of the body
      glm::vec3 moveDirection =
          glm::normalize(cameraToBody); // normalize to get the direction
      camera.position =
          body.position +
          moveDirection * body.radius; // place the camera on the surface

      /** ensuring camera doesn't get stuck by slightly adjusting the camera's
       position outside the sphere if it's too close **/
      camera.position += moveDirection * 0.1f; // small push outside the surface

      std::cout << "Camera adjusted to surface at: " << camera.position.x
                << ", " << camera.position.y << ", " << camera.position.z
                << std::endl;
    }
  }
}

// check collision with terrain using raycasting
bool CollisionDetector::checkCollisionWithTerrain(
    float mouseX, float mouseY, const glm::mat4 &projection,
    const glm::mat4 &view, const std::vector<CelestialBody> &bodies) {
  // get the ray direction for the falling ray
  Ray ray =
      rayCaster->getRayFromScreenCoordinates(mouseX, mouseY, projection, view);
  ray.direction =
      glm::vec3(0.0f, -1.0f, 0.0f); // ray falling down along the Y-axis

  // check for collision with bodies
  for (const auto &body : bodies) {
    if (rayCaster->checkRayIntersection(ray, body)) {
      std::cout << "Collision detected with body at position: "
                << body.position.x << " " << body.position.y << " "
                << body.position.z << std::endl;
      return true; // collision detected
    }
  }

  return false; // no collision detected
}
