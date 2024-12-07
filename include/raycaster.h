#ifndef RAYCASTER
#define RAYCASTER

#include <glm/glm.hpp>
#include <camera.h>
#include <celestial_body.h>

class Ray {
public:
  glm::vec3 origin;
  glm::vec3 direction;

  Ray(const glm::vec3 &origin, const glm::vec3 &direction)
      : origin(origin), direction(direction) {}
};

class RayCaster {
public:
  RayCaster(Camera *camera);
  Ray getRayFromScreenCoordinates(float x, float y, const glm::mat4 &projection,
                                  const glm::mat4 &view);
  glm::vec3 getRayDirectionFromScreenCoordinates(float x, float y,
                                                 const glm::mat4 &projection,
                                                 const glm::mat4 &view);
  bool checkRayIntersection(const Ray &ray, const CelestialBody &body);

private:
  Camera *camera;
};

#endif
