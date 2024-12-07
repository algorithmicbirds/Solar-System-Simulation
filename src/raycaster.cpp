#include <raycaster.h>
#include <globals.h>

RayCaster::RayCaster(Camera *camera) : camera(camera) {}

Ray RayCaster::getRayFromScreenCoordinates(float x, float y,
                                           const glm::mat4 &projection,
                                           const glm::mat4 &view) {
  // convert screen coordinates to normalized device coordinates (NDC)
  glm::vec4 clipSpacePos =
      glm::vec4((x / SCR_WIDTH) * 2.0f - 1.0f,
                -((y / SCR_HEIGHT) * 2.0f - 1.0f), -1.0f, 1.0f); // near plane

  // inverse of the projection matrix to convert to eye space
  glm::mat4 inverseProj = glm::inverse(projection);
  glm::mat4 inverseView = glm::inverse(view);

  // convert NDC to eye space
  glm::vec4 eyeSpacePos = inverseProj * clipSpacePos;
  eyeSpacePos.z = -1.0f; // near plane
  eyeSpacePos.w = 0.0f;

  // convert eye space to world space
  glm::vec4 worldSpacePos = inverseView * eyeSpacePos;

  // Normalize the direction of the ray
  glm::vec3 rayDir = glm::normalize(glm::vec3(worldSpacePos));

  return Ray(camera->position, rayDir);
}

// Check if the ray intersects
bool RayCaster::checkRayIntersection(const Ray &ray,
                                     const CelestialBody &body) {
  glm::vec3 oc = ray.origin -
                 body.position; // vector from ray origin to the sphere's center
  float a = glm::dot(ray.direction, ray.direction);
  float b = 2.0f * glm::dot(oc, ray.direction);
  float c = glm::dot(oc, oc) - body.radius * body.radius;

  float discriminant = b * b - 4.0f * a * c;

  return discriminant > 0; // ray intersects the sphere if discriminant > 0
}

glm::vec3 RayCaster::getRayDirectionFromScreenCoordinates(
    float x, float y, const glm::mat4 &projection, const glm::mat4 &view) {
  // use the previous logic to calculate ray direction from screen coordinates
  return getRayFromScreenCoordinates(x, y, projection, view).direction;
}
