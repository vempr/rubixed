#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include "camera.h"

void init_camera(OrbitCamera *c) {
	c->camera.position = (Vector3){ 0.0f, 6.5f, 6.5f };
	c->camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
	c->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
	c->camera.fovy = 45.0f;
	c->camera.projection = CAMERA_PERSPECTIVE;

	c->cameraRadius = sqrt(pow(6.5, 2.0) + pow(6.5, 2.0));
	c->cameraAlpha = 0.0f;
	c->cameraBeta = PI / 4;
	c->previousMousePosition = (Vector2){0.0f, 0.0f};
}

void camera_update(OrbitCamera *c) {
  Vector2 currentMousePosition = GetMousePosition();

  if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
    float deltaX = currentMousePosition.x - c->previousMousePosition.x;
    float deltaY = currentMousePosition.y - c->previousMousePosition.y;

    c->cameraAlpha -= deltaX * 0.004f;
    c->cameraBeta += deltaY * 0.004f;

    float lim = PI / 2 - 0.01;
    if (c->cameraBeta > lim) c->cameraBeta = lim;
    if (c->cameraBeta < -lim) c->cameraBeta = -lim;
  }

  c->previousMousePosition = currentMousePosition;

  // cartesian to spherical conversion
  c->camera.position.x = c->cameraRadius * cosf(c->cameraBeta) * sinf(c->cameraAlpha);
  c->camera.position.y = c->cameraRadius * sinf(c->cameraBeta);
  c->camera.position.z = c->cameraRadius * cosf(c->cameraAlpha) * cosf(c->cameraBeta);

  float wheelMove = GetMouseWheelMove();
  if (wheelMove != 0.0f) {
    c->cameraRadius -= wheelMove * 0.4f;
    if (c->cameraRadius < 4.0f) c->cameraRadius = 4.0f;
    if (c->cameraRadius > 16.0f) c->cameraRadius = 16.0f;
  }
}