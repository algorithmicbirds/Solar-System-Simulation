#ifndef COLLISION_DETECTOR
#define COLLISION_DETECTOR

#include <glm/glm.hpp>
#include <raycaster.h>
#include <celestial_body.h>
#include <vector>

class CollisionDetector {
public:
  CollisionDetector(RayCaster *rayCaster);

  // perform collision detection with rays falling down
  bool checkCollisionWithTerrain(float mouseX, float mouseY,
                                 const glm::mat4 &projection,
                                 const glm::mat4 &view,
                                 const std::vector<CelestialBody> &bodies);

  void resolveCameraCollisions(Camera &camera,
                               std::vector<CelestialBody> &bodies);

private:
  RayCaster *rayCaster;
};

#endif
