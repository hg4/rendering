#include "Camera.h"

void Camera::SetProjection(glm::mat4 & projection)
{
	projectionMatrix = projection;
}
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}