#include <raylib.h>

#ifndef CAMERA_H
#define CAMERA_H

typedef struct {
  Camera3D camera;
  float cameraRadius;
	float cameraAlpha;
	float cameraBeta;
	Vector2 previousMousePosition;
} OrbitCamera;

void init_camera(OrbitCamera *c);
void camera_update(OrbitCamera *c);

#endif