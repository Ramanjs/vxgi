#include "camera.h"

glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(position, position + front, worldUp);
}

void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
  float velocity = movementSpeed * deltaTime;
  if (direction == Camera_Movement::FORWARD)
    position += front * velocity;
  if (direction == Camera_Movement::BACKWARD)
    position -= front * velocity;
  if (direction == Camera_Movement::LEFT)
    position -= u * velocity;
  if (direction == Camera_Movement::RIGHT)
    position += u * velocity;
  if (direction == Camera_Movement::UP)
    position += v * velocity;
  if (direction == Camera_Movement::DOWN)
    position -= v * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset,
                                  GLboolean constrainPitch) {
  xoffset *= mouseSensitivity;
  yoffset *= mouseSensitivity;

  glm::mat4 rotatex = glm::rotate(glm::mat4(1.0f), glm::radians(-xoffset), v);
  glm::mat4 rotatey = glm::rotate(glm::mat4(1.0f), glm::radians(yoffset), u);

  glm::vec3 newfront = rotatey * rotatex * glm::vec4(front, 0.0f);

  if (fabs(glm::dot(newfront, worldUp)) > 0.98f) {
    return;
  }

  front = newfront;

  updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
  zoom -= (float)yoffset;
  if (zoom < 1.0f)
    zoom = 1.0f;
  if (zoom > 45.0f)
    zoom = 45.0f;
}

void Camera::updateCameraVectors() {
  glm::vec3 w = glm::normalize(-front);
  u = glm::normalize(glm::cross(worldUp, w));
  v = glm::cross(w, u);
}
