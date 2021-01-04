#pragma once
#include "vec3d.h"


class Camera
{
public:
    Camera();

    Vec3d lookAtTarget();
    void rotateYaw(const float& angle);
    void rotatePitch(const float& angle);

    Vec3d position;         // the location of the camera in World Space
    Vec3d direction;        // points to the target
    Vec3d forwardVelocity;  // direction and magnitude of the camera speed movement
    float yaw;              // angle (radians) for yaw rotation (around Y-axis)
    float pitch;            // angle (radians) for pitch rotation (around X-axis)
};
