#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
using namespace std;
// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	glm::mat4 projectionMatrix;
	// constructor with vectors
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();
	void SetProjection(glm::mat4& projection);

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += Front * velocity;
		if (direction == BACKWARD)
			Position -= Front * velocity;
		if (direction == LEFT)
			Position -= Right * velocity;
		if (direction == RIGHT)
			Position += Right * velocity;
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		Zoom -= (float)yoffset;
		if (Zoom < 1.0f)
			Zoom = 1.0f;
		if (Zoom > 45.0f)
			Zoom = 45.0f;
	}

private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif
//
//#ifndef CAMERA_H
//#define CAMERA_H
//
//#include <glad/glad.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#include <vector>
//
//// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
//enum Camera_Movement {
//	FORWARD,
//	BACKWARD,
//	LEFT,
//	RIGHT,
//	UP,
//	DOWN
//};
//
//
//// Default camera values
//const float YAW = 0.0f;
//const float PITCH = 0.0f;
//const float HEAD = 180.0f;
//const float SPEED = 2.5f;
//const float SENSITIVITY = 0.1f;
//const float ZOOM = 45.0f;
//
//
//// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
//class Camera
//{
//public:
//	// camera Attributes
//	glm::vec3 Position;
//	glm::vec3 Front;
//	glm::vec3 Up;
//	glm::vec3 Right;
//	glm::vec3 WorldUp;
//	// euler Angles
//	float Pitch;		//around x
//	float Head;		//around y
//	//float Yaw;
//
//	float Roll;		//around z
//	// camera options
//	float MovementSpeed;
//	float MouseSensitivity;
//	float Zoom;
//
//	// constructor with vectors
//	//default:默认相机初始朝向正z,pitch=head=roll=0.0f
//	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3 worldup = glm::vec3(0.0f, 1.0f, 0.0f), float head = HEAD, float pitch = PITCH,float roll=0.0) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//	{
//		Position = position;
//		WorldUp = worldup;
//		Head = head;
//		//Yaw = yaw;
//		Pitch = pitch;
//		Roll = roll;
//		updateCameraVectors();
//	}
//	// constructor with scalar values
//	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float head, float pitch,float roll=0.0) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
//	{
//		Position = glm::vec3(posX, posY, posZ);
//		WorldUp = glm::vec3(upX, upY, upZ);
//		//Yaw = yaw;
//		Head = head;
//		Pitch = pitch;
//		Roll = roll;
//		updateCameraVectors();
//	}
//
//	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
//	glm::mat4 GetViewMatrix()
//	{
//		return glm::lookAt(Position, Position + Front, Up);
//	}
//
//	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
//	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
//	{
//		float velocity = MovementSpeed * deltaTime;
//		if (direction == FORWARD)
//			Position += Front * velocity;
//		if (direction == BACKWARD)
//			Position -= Front * velocity;
//		if (direction == LEFT)
//			Position -= Right * velocity;
//		if (direction == RIGHT)
//			Position += Right * velocity;
//		if (direction == UP)
//			Position += Up * velocity;
//		if (direction == DOWN)
//			Position -= Up * velocity;
//	}
//
//	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
//	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
//	{
//		xoffset *= MouseSensitivity;
//		yoffset *= MouseSensitivity;
//
//		Head += xoffset;
//		Pitch += yoffset;
//
//		// make sure that when pitch is out of bounds, screen doesn't get flipped
//		if (constrainPitch)
//		{
//		/*	if (Head > 179.0f)
//				Head = 179.0f;
//			if (Head < -179.0f)
//				Head = -179.0f;*/
//			if (Head > 359.0f)
//				Head = 359.0f;
//			if (Head < 0.0f)
//				Head = 0.0f;
//		}
//
//		// update Front, Right and Up Vectors using the updated Euler angles
//		updateCameraVectors();
//		//updateCameraVectors(xoffset, Up);
//		//updateCameraVectors(yoffset, Right);
//	}
//
//	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
//	void ProcessMouseScroll(float yoffset)
//	{
//		Zoom -= (float)yoffset;
//		if (Zoom < 1.0f)
//			Zoom = 1.0f;
//		if (Zoom > 45.0f)
//			Zoom = 45.0f;
//	}
//	void updateCameraVectors(float theta, vec3 u) {
//		glm::quat q;
//		theta = glm::radians(theta/2);
//		float cos_theta = cos(theta);
//		float sin_theta = sin(theta);
//		u *= sin_theta;
//		q.w = cos_theta;
//		q.x = u.x;
//		q.y = u.y;
//		q.z = u.z;
//		Front = glm::normalize(quat_mul(q, Front));
//		Right = glm::normalize(quat_mul(q, Right));
//		Up = glm::normalize(quat_mul(q, Up));
//		Head = atan2(Right.z, Front.z);
//		Pitch = asin(Up.z);
//		Roll = atan2(-Up.x, -Up.y);
//		printf("head:%.2f, Pitch:%.2f, Roll:%.2f\n", Head, Pitch, Roll);
//	}
//
//private:
//	// calculates the front vector from the Camera's (updated) Euler Angles
//	void updateCameraVectors()
//	{
//		// calculate the new Front vector
//		glm::vec3 front;
//		float cosh = cos(glm::radians(Head));
//		float sinh = sin(glm::radians(Head));
//		float cosp = cos(glm::radians(Pitch));
//		float sinp = sin(glm::radians(Pitch));
//		float cosr = cos(glm::radians(Roll));
//		float sinr = sin(glm::radians(Roll));
//	/*	glm::mat4x4 m(cosr*cosh-sinr*sinp*sinh,-sinr*cosp,cosr*sinh+sinr*sinp*cosh,0,
//									sinr*cosh+cosr*sinp*sinh,cosr*cosp,sinr*sinh-cosr*sinp*cosh,0,
//									-cosp*sinh,							sinp,					cosp*cosh,				 0,
//									0,0,0,1);*/
//		front.x = cosr*sinh + sinr*sinp*cosh;
//		front.y = sinr*sinh - cosr*sinp*cosh;
//		front.z = cosp*cosh;
//	/*	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
//		front.y = sin(glm::radians(Pitch));
//		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));*/
//		Front = glm::normalize(front);
//		// also re-calculate the Right and Up vector
//		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
//		Up = glm::normalize(glm::cross(Right, Front));
//		//printf("head:%.2f, Pitch:%.2f, Roll:%.2f\n", Head, Pitch, Roll);
//		//cout << Position.x << ":" << Position.y << " " << Position.z<<endl;
//		
//	}
//
//	vec3 quat_mul(quat q, vec3 v) {
//		quat q_star = quat(q.w, vec3(-q.x, -q.y, -q.z));
//		quat mq = q*quat(0.0,v)*q_star;
//		return vec3(mq.x, mq.y, mq.z);
//	}
//};
//#endif