#ifndef CAMERA_H
#define CAMERA_H

#include "raylib.h"
#include "raymath.h"

void setTarget(Vector3 t);
void setCameraDistance(float cD);
void setCameraPitch(float cP);
void setCameraYaw(float cY);
void UpdateCameraManual(Camera3D *camera);

#endif // CAMERA_H