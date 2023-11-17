#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"

#include <vector>

enum class Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

// Default camera values
const float SPEED = 100.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
  glm::vec3 position;
  glm::vec3 u;
  glm::vec3 v;
  glm::vec3 front;
  glm::vec3 worldUp;

  float movementSpeed;
  float mouseSensitivity;
  float zoom;

  Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f))
      : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED),
        mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
    position = _position;
    worldUp = _up;
    updateCameraVectors();
  }

  glm::mat4 getViewMatrix();

  void processKeyboard(Camera_Movement direction, float deltaTime);

  void processMouseMovement(float xoffset, float yoffset,
                            GLboolean constrainPitch = true);

  void processMouseScroll(float yoffset);

private:
  void updateCameraVectors();
};

#endif
