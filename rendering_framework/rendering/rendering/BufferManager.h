#pragma once
//use for VAO _be->VBO _be->EBO FBO generating
#include <glad\glad.h>
#include "Texture.h"
#include "Vertex.h"
#include <memory>


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
	static shared_ptr<BufferElement> genBindVAOBuffer(const Vertex * data,const GLsizeiptr data_byte_length);
	static shared_ptr<BufferElement> genBindVAOBuffer(const Vertex * data, uint data_length, const Tangent * t_data, uint tan_length);
	static shared_ptr<BufferElement> genBindEBOBuffer(const Vertex * vertex_data, const GLsizeiptr vertex_data_byte_length,const unsigned int *indice_data, const GLsizeiptr indice_data_byte_length);
	static shared_ptr<BufferElement> genBindEBOBuffer(const Vertex * vertex_data, uint v_length, const unsigned int *indice_data,uint i_length,const Tangent * tan_data,uint tan_length );
	static shared_ptr<BufferElement> genBindFRBOBuffer(const int height,const int width);
	static void bind(GLenum type, shared_ptr<BufferElement> ele);
	static void unbind(GLenum type, shared_ptr<BufferElement> ele);
	static void deleteBuffer(BufferElement * ele);
	static void DynamicDraw(bool flag);
private:
	static GLenum _rbo_type;
	static GLenum _usage;
	static GLenum _fbo_attachment;
	static GLenum _draw_type;
};
