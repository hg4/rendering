#include "BufferManager.h"

GLenum BufferManager::_rbo_type = GL_DEPTH24_STENCIL8;
GLenum BufferManager::_usage = GL_STATIC_DRAW;
GLenum BufferManager::_fbo_attachment = GL_DEPTH_STENCIL_ATTACHMENT;
GLenum BufferManager::_draw_type = GL_STATIC_DRAW;

unsigned int BufferManager::genBindFBOBuffer()
{
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	return FBO;
}

unsigned int BufferManager::genBindRBOBuffer(const int height, const int width)
{
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, _rbo_type, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, _fbo_attachment, GL_RENDERBUFFER, RBO);
	return RBO;
}

BufferElement* BufferManager::genBindVAOBuffer(const Vertex * data, uint data_length, const Tangent * t_data, uint tan_length)
{
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*data_length + sizeof(Tangent)*tan_length, nullptr, _draw_type);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*data_length, data);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex)*data_length, sizeof(Tangent)*data_length, t_data);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Tangent), (void*)(sizeof(Vertex)*data_length+ offsetof(Tangent, tangent)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Tangent), (void*)(sizeof(Vertex)*data_length+ offsetof(Tangent, bitangent)));
	//glBindVertexArray(0);
	return new BufferElement(VAO, VBO, 0);
}
BufferElement* BufferManager::genBindVAOBuffer(const Vertex * data, const GLsizeiptr data_byte_length)
{
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	//set data buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, data_byte_length, data, _draw_type);
	//set data attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	/*glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));*/
	//finish VAO binding
	//glBindVertexArray(0);
	return new BufferElement(VAO,VBO,0);
}
BufferElement * BufferManager::genBindEBOBuffer(const Vertex * vertex_data, uint v_length, const unsigned int * indice_data, uint i_length, const Tangent * tan_data, uint tan_length)
{
	unsigned int EBO;

	BufferElement* be = genBindVAOBuffer(vertex_data, v_length,tan_data,tan_length);
	bind(GL_ARRAY_BUFFER, be);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * i_length, indice_data, _draw_type);
	be->EBO = EBO;
	//unbind(GL_ARRAY_BUFFER, be);
	return be;
}

BufferElement* BufferManager::genBindEBOBuffer(const Vertex * vertex_data, const GLsizeiptr vertex_data_byte_length, const unsigned int * indice_data, const GLsizeiptr indice_data_byte_length)
{
	unsigned int EBO;

	BufferElement* be = genBindVAOBuffer(vertex_data, vertex_data_byte_length);
	bind(GL_ARRAY_BUFFER, be);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indice_data_byte_length, indice_data, _draw_type);
	be->EBO = EBO;
	//unbind(GL_ARRAY_BUFFER, be);
	return be;
}

BufferElement* BufferManager::genBindFRBOBuffer(const int height, const int width)
{
	unsigned int FBO, RBO;
	//glGenFramebuffers(1, &FBO);
	////创建RBO作为FBO的深度缓冲，因为不采样深度用RBO效率更高
	//glGenRenderbuffers(1, &RBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	//glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	//glRenderbufferStorage(GL_RENDERBUFFER,_rbo_type,width,height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, _fbo_attachment, GL_RENDERBUFFER, RBO);
	//glBindFramebuffer(GL_FRAMEBUFFER,0);
	FBO=genBindFBOBuffer();
	RBO = genBindRBOBuffer(height, width);
	return new BufferElement(FBO, RBO);
}

void BufferManager::bind(GLenum type,BufferElement * ele)
{
	if (type == GL_ARRAY_BUFFER) glBindVertexArray(ele->VAO);
	else if (type == GL_FRAMEBUFFER) glBindFramebuffer(type,ele->FBO);
}

void BufferManager::unbind(GLenum type, BufferElement * ele)
{
	if (type == GL_ARRAY_BUFFER) glBindVertexArray(0);
	else if (type == GL_FRAMEBUFFER) glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void BufferManager::deleteBuffer(BufferElement * ele)
{
	if (ele->VAO != 0) glDeleteBuffers(1, &ele->VAO);
	if (ele->VBO != 0) glDeleteBuffers(1, &ele->VBO);
	if (ele->EBO != 0) glDeleteBuffers(1, &ele->EBO);
	if (ele->FBO != 0) glDeleteFramebuffers(1, &ele->FBO);
	if (ele->RBO != 0) glDeleteRenderbuffers(1, &ele->RBO);
}

void BufferManager::DynamicDraw(bool flag)
{
	if (flag) _draw_type = GL_DYNAMIC_DRAW;
	else _draw_type = GL_STATIC_DRAW;
}
