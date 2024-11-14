#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 

class Camera {
public:
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;

  float yaw;
  float pitch;
  float fov;

  float speed;
  float sensitivity;

  Camera(glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 20.0f),
         float startYaw = -90.0f, float startPitch = 0.0f,
         float startFov = 45.0f);

  glm::mat4 getViewMatrix() const; 

  void processKeyboardInput(float deltaTime, bool w, bool s, bool a, bool d,
                            bool space, bool shift);
  void processMouseMovement(float xOffset, float yOffset);

  void setAspectRatio(float aspectRatio);
  glm::mat4 getProjectionMatrix() const { return projection; }

private:
  void updateCameraFront();
  void updateProjection();

  glm::mat4 projection; 
};

#endif
