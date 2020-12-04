#include "Mesh.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <string>

void Mesh::setMaterial(Material * mtr)
{
	material = mtr;
	cout << mtr->textureList.size() << " " << material->textureList.size() << endl;
}

void Mesh::addTexture(unsigned int  _id, string _name,GLenum _type)
{
	material->textureList.push_back(Texture(_id,_name,_type));
}
void Mesh::draw(Shader &shader)
{
	shader.setUniform("model", mvp.model);
	shader.setUniform("view", mvp.view);
	shader.setUniform("projection", mvp.projection);
	shader.setUniform("compose", mvp.compose);
	shader.setUniform("transpose_inverse_model", mvp.transpose_inverse_model);
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (int i = 0; i < material->textureList.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
										  // retrieve texture number (the N in diffuse_textureN)
		string number;
		vector<Texture> textures = material->textureList;
		string name = textures[i].name_in_shader;
			if (name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if (name == "texture_specular")
				number = std::to_string(specularNr++); // transfer unsigned int to stream
			else if (name == "texture_normal")
				number = std::to_string(normalNr++); // transfer unsigned int to stream
			else if (name == "texture_height")
				number = std::to_string(heightNr++); // transfer unsigned int to stream
													 // now set the sampler to the correct texture unit
			else number = "";
			shader.setUniform(name + number, i);
			// and finally bind the texture
			glBindTexture(textures[i].texture_type, textures[i].id);
	}
	// draw mesh
	glBindVertexArray(_be->VAO);
	if(_useEBO) glDrawElements(_primitiveType, _vertex_number, GL_UNSIGNED_INT, 0);
	else glDrawArrays(_primitiveType, 0, _vertex_number);
	glBindVertexArray(0);
	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::draw()
{
	glBindVertexArray(_be->VAO);
	if (_useEBO) glDrawElements(_primitiveType, indices.size(), GL_UNSIGNED_INT, 0);
	else glDrawArrays(_primitiveType, 0, _vertex_number);
	glBindVertexArray(0);
}

void Mesh::init()
{
	if (_useEBO) 
		_be=BufferManager::genBindEBOBuffer(&vertices[0], vertices.size() * sizeof(Vertex), indices.data(), indices.size() * sizeof(Vertex));
	else _be=BufferManager::genBindVAOBuffer(&vertices[0], vertices.size() * sizeof(Vertex));
	//unsigned int VAO, VBO, EBO=0;
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//if(_useEBO) glGenBuffers(1, &EBO);
	//glBindVertexArray(VAO);
	////set data buffer
	//glBindBuffer(GL_ARRAY_BUFFER,VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);
	////set indices buffer
	//if (_useEBO)
	//{
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW);
	//}
	////set data attribute
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (void*)0);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	////finish _be->VAO binding
	//glBindVertexArray(0);
	//_be = new BufferElement(VAO, VBO, EBO);
}