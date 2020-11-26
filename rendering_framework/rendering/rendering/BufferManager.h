#pragma once
//use for VAO _be->VBO _be->EBO FBO generating
#include <glad\glad.h>
#include "Texture.h"
#include "Vertex.h"


struct  BufferElement {
	unsigned int VAO,VBO, EBO,FBO,RBO;
	BufferElement(unsigned int vao,unsigned int vbo,unsigned int ebo):
		VAO(vao),VBO(vbo),EBO(ebo),FBO(0),RBO(0){}
	BufferElement(unsigned int fbo, unsigned int rbo) :
		VAO(0), VBO(0),EBO(0), FBO(fbo), RBO(rbo) {}
};
class BufferManager {
public:
	static unsigned int  genBindFBOBuffer();
	static unsigned int genBindRBOBuffer(const int height, const int width);
	static BufferElement* genBindVAOBuffer(const Vertex * data,const GLsizeiptr data_byte_length);
	static BufferElement* genBindEBOBuffer(const Vertex * vertex_data, const GLsizeiptr vertex_data_byte_length,const unsigned int *indice_data, const GLsizeiptr indice_data_byte_length);
	static BufferElement* genBindFRBOBuffer(const int height,const int width);
	static void bind(GLenum type, BufferElement * ele);
	static void unbind(GLenum type, BufferElement * ele);
	static void deleteBuffer(BufferElement * ele);
private:
	static GLenum _rbo_type;
	static GLenum _usage;
	static GLenum _fbo_attachment;
};
