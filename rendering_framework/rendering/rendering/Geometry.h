#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Mesh.h"
#include <memory>
class Geometry {
public:
	static Mesh createCube();
	static Mesh createQuad();
	static unsigned int renderPlane();
	static Mesh createSphere();
};
