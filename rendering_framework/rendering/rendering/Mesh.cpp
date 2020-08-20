#include "Mesh.h"
#include <GLFW\glfw3.h>
#include <glad\glad.h>

void Mesh::init(){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VEO);
	glBindVertexArray(VAO);
	//set data buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	//set indices buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VEO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);
	//set data attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	//finish VAO binding
	glBindVertexArray(0);
}