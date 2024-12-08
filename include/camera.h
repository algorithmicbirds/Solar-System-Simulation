#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
  Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
      : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(200.0f),
        mouseSensitivity(0.05f), zoom(45.0f) {
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
  }

  glm::mat4 getViewMatrix() {
    return glm::lookAt(position, position + front, up);
  }

  glm::mat4 getProjectionMatrix(float width, float height) {
    return glm::perspective(glm::radians(zoom), width / height, 0.1f, 50000.0f);
  }

  void processKeyboard(int direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;

    if (direction == 0) // FORWARD
      position += front * velocity;
    if (direction == 1) // BACKWARD
      position -= front * velocity;
    if (direction == 2) // LEFT
      position -= right * velocity;
    if (direction == 3) // RIGHT
      position += right * velocity;
    if (direction == 4) // JUMP (Up)
      position += worldUp * velocity;
    if (direction == 5) // CROUCH (Down)
      position -= worldUp * velocity;
  }

  void processMouseMovement(float xOffset, float yOffset,
                            GLboolean constrainPitch = true) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch -= yOffset;

    if (constrainPitch) {
      if (pitch > 89.0f)
        pitch = 89.0f;
      if (pitch < -89.0f)
        pitch = -89.0f;
    }

    updateCameraVectors();
  }

  void processMouseScroll(float yOffset) {
    if (zoom >= 1.0f && zoom <= 45.0f)
      zoom -= yOffset;
    if (zoom < 1.0f)
      zoom = 1.0f;
    if (zoom > 45.0f)
      zoom = 45.0f;
  }

  void setPosition(glm::vec3 newPosition) { position = newPosition; }

  glm::vec3 position, front, up, right, worldUp;

private:
  float yaw, pitch;
  float movementSpeed, mouseSensitivity, zoom;

  void updateCameraVectors() {
    glm::vec3 tempFront;
    tempFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    tempFront.y = sin(glm::radians(pitch));
    tempFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(tempFront);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
  }
};

#endif
