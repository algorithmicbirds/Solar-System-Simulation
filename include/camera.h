#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <celestial_body.h>
#include <globals.h>

class Camera {
public:
  glm::vec3 position; // Camera position in space
  glm::vec3 front;    // Camera's front vector
  glm::vec3 up;       // Camera's up vector
  glm::vec3 right;    // Camera's right vector
  glm::vec3 worldUp;  // The world "up" vector (constant)

  float yaw;   // Camera's yaw (rotation around y-axis)
  float pitch; // Camera's pitch (rotation around x-axis)
  float fov;   // Field of view

  float speed;       // Movement speed
  float sensitivity; // Mouse sensitivity
  glm::mat4 projection;

  Camera(glm::vec3 startPosition, float startYaw, float startPitch,
         float startFov);

  glm::mat4 getViewMatrix() const;
  void processKeyboardInput(float deltaTime, bool w, bool s, bool a, bool d,
                            bool space, bool shift);
  void processMouseMovement(float xOffset, float yOffset);
  void updateCameraFront();
  void setAspectRatio(float aspectRatio);
  void updateProjection();
  glm::vec3 calculateGravitationalForce(const CelestialBody &body);
};

#endif
