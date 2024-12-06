#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.h>
#include <celestial_body.h>
#include <globals.h>
// for ray representation
struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;

  Ray(const glm::vec3 &origin, const glm::vec3 &direction)
      : origin(origin), direction(glm::normalize(direction)) {}
};

class RayCaster {
public:
  RayCaster(Camera *camera);

  // calculate the ray from the camera through a given screen position (mouse
  // position)
  Ray getRayFromScreenCoordinates(float x, float y, const glm::mat4 &projection,
                                  const glm::mat4 &view);

  // check if a ray intersects a celestial body 3d object
  bool checkRayIntersection(const Ray &ray, const CelestialBody &body);
  glm::vec3 getRayDirectionFromScreenCoordinates(float x, float y,
                                                 const glm::mat4 &projection,
                                                 const glm::mat4 &view);

private:
  Camera *camera;
};

#endif
