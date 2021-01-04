#include "camera.h"

Camera::Camera()
{
    position        = Vec3d(0.f, 0.f, 0.f);
    direction       = Vec3d(0.f, 0.f, 1.f);
    forwardVelocity = Vec3d(0.f, 0.f, 0.f);
    yaw             = 0.f;
    pitch           = 0.f;
}

void Camera::rotateYaw(const float& angle)
{
    this->yaw += angle;
}

void Camera::rotatePitch(const float& angle)
{
    this->pitch += angle;
}

Vec3d Camera::lookAtTarget()
{
    Mat4 cameraPitchRotation = Mat4::rotateX(this->pitch);
    Mat4 cameraYawRotation = Mat4::rotateY(this->yaw);

    // create a camera rotation matrix based on Yaw and Pitch
    Mat4 cameraRotation = Mat4::eye();
    cameraRotation = cameraPitchRotation * cameraRotation;
    cameraRotation = cameraYawRotation * cameraRotation;

    // update camera direction based on the rotation
    Vec3d target(0, 0, 1); // target is looking at the positive Z-axis
    this->direction = Vec4d::toVec3d( Vec3d::toVec4d(target) * cameraRotation );

    // offset the camera position in the direction where the camera is looking at
    target = this->position + this->direction;
    return target;
}
