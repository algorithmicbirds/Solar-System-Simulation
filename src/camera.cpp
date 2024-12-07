#include <camera.h>

Camera::Camera(glm::vec3 startPosition, float startYaw, float startPitch,
               float startFov)
    : position(startPosition), yaw(startYaw), pitch(startPitch), fov(startFov),
      front(glm::vec3(0.0f, 0.0f, -1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)),
      worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), speed(2000.5f), sensitivity(0.05f) {
  updateCameraFront();
  updateProjection();
}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboardInput(float deltaTime, bool w, bool s, bool a,
                                  bool d, bool space, bool shift) {
  float cameraSpeed = speed * deltaTime;
  if (shift) {
    cameraSpeed *= 2; // Faster movement (shift key)
  }

  if (w) {
    position += cameraSpeed * front;
  }
  if (s) {
    position -= cameraSpeed * front;
  }
  if (a) {
    position -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
  }
  if (d) {
    position += glm::normalize(glm::cross(front, up)) * cameraSpeed;
  }
  if (space) {
    position += cameraSpeed * up; // Move up (jump/fly)
  }
  if (shift) {
    position -= cameraSpeed * up; // Move down
  }
}

void Camera::processMouseMovement(float xOffset, float yOffset) {
  xOffset *= sensitivity;
  yOffset *= sensitivity;

  yaw += xOffset;
  pitch -= yOffset; // Invert the y-axis if needed (FPS convention)

  if (pitch > 89.0f)
    pitch = 89.0f;
  if (pitch < -89.0f)
    pitch = -89.0f;

  updateCameraFront();
}

void Camera::updateCameraFront() {
  glm::vec3 newFront;
  newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  newFront.y = sin(glm::radians(pitch));
  newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  front = glm::normalize(newFront);

  // Recalculate the right and up vectors
  right = glm::normalize(glm::cross(front, worldUp)); // Right vector
  up = glm::normalize(glm::cross(right, front));      // Up vector
}

void Camera::setAspectRatio(float aspectRatio) {
  projection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

glm::vec3 Camera::calculateGravitationalForce(const CelestialBody &body) {
  glm::vec3 direction = body.position - position;
  float distance = glm::length(direction);
  if (distance == 0.0f)
    return glm::vec3(0.0f); // Avoid division by zero

  float forceMagnitude =
      gravitationalConstant * (body.mass) / (distance * distance);
  glm::vec3 force = glm::normalize(direction) * forceMagnitude;

  return force;
}

void Camera::updateProjection() {
  projection = glm::perspective(glm::radians(fov), 1.0f, 0.1f, 100.0f);
}
