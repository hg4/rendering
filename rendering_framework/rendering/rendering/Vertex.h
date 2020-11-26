#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

class Vertex {
public:
	Vertex() {}
	Vertex(vec3 _position, vec3 _normal, vec2 _texCoord) :
		position(_position), normal(_normal), texCoords(_texCoord) {}
	vec3 position;
	vec3 normal;
	vec2 texCoords;
};
