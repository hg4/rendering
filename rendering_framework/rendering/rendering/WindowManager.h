#pragma once
#include <GLFW/glfw3.h>
#include <iostream>
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
using namespace std;

#define	WINDOW_ERROR -1
#define	WINDOW_OK	1

class WindowManager {
public:
	static Camera * camera_;
	static bool firstMouse_;
	static float lastX_, lastY_;
	WindowManager() {
		windowInit(1024, 1024, "", 3, 3);
		camera_ = new Camera(vec3(0.0, 0.0, -1.0));
		callbackRegister();
	}
	WindowManager(int height, int width, vec3 cam_pos, char * title = "MyViewer", unsigned int major_version = 3, unsigned int minor_version = 3) {
		windowInit(height, width, title, major_version, minor_version);
		camera_ = new Camera(cam_pos);
		callbackRegister();
	}
	typedef void(*mainloop)(void);
	void WindowLoop(mainloop) {
		while (!glfwWindowShouldClose(window_)) {
			mainloop();
		}
	}
	GLFWwindow * getWindow() { return window_; }
	Camera * getCamera() { return camera_; }
private:
	GLFWwindow * window_;

	unsigned int scr_height_, scr_width_, major_version_, minor_version_;
	char * window_title_;
	int windowInit(unsigned int _height, unsigned int _width, char * _title,
		unsigned int _major, unsigned int _minor) {
		// glfw: initialize and configure
		// ------------------------------
		lastX_ = _width / 2.0;
		lastY_ = _height / 2.0;
		firstMouse_ = true;
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _minor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		window_ = glfwCreateWindow(_height, _width, _title, NULL, NULL);
		if (window_ == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return WINDOW_ERROR;
		}
		glfwMakeContextCurrent(window_);
	}
	

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and 
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
	}


	// glfw: whenever the mouse moves, this callback is called
	// -------------------------------------------------------
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
			if (firstMouse_)
			{
				lastX_ = xpos;
				lastY_ = ypos;
				firstMouse_ = false;
			}

			float xoffset = xpos - lastX_;
			float yoffset = lastY_ - ypos; // reversed since y-coordinates go from bottom to top

			lastX_ = xpos;
			lastY_ = ypos;

			camera_->ProcessMouseMovement(xoffset, yoffset);
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE) firstMouse_ = true;
	}

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	// ----------------------------------------------------------------------
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera_->ProcessMouseScroll(yoffset);
	}

	void callbackRegister() {
		glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
		glfwSetCursorPosCallback(window_, mouse_callback);
		glfwSetScrollCallback(window_, scroll_callback);
	}
};
Camera* WindowManager::camera_ = NULL;
bool WindowManager::firstMouse_=true;
float WindowManager::lastX_ = 0.0;
float WindowManager::lastY_ = 0.0;