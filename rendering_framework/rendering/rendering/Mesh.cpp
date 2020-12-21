#include "Mesh.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <string>

Mesh::Mesh(vector<Vertex>& _v, vector<unsigned int>& _i, vector<Texture>& _t, vector<Tangent> &_tg)
{
	material = (new Material());
	_useEBO = true;
	_useTangent = true;
	_primitiveType = GL_TRIANGLES;
	tangents = _tg;
	vertices = _v;
	indices = _i;
	material->textureList = _t;
	_vertex_number = _v.size();
	init();
}

Mesh::Mesh(vector<Vertex> &_v, vector<unsigned int> &_i, vector<Texture> &_t, GLenum _gl_type) {
	material = (new Material());
	_useEBO = true;
	_useTangent = false;
	_primitiveType = _gl_type;
	vertices = _v;
	indices = _i;
	material->textureList = _t;
	_vertex_number = _v.size();
	init();
}
//只能用于三角形类型的接口，不能画line
Mesh::Mesh(float * _pos, float *_norm, float *_tex, int _length) :Mesh(_pos, _norm, _tex, _length, false, NULL, 0, GL_TRIANGLES) {

}
Mesh::Mesh(float * _pos, float *_norm, float *_tex, int _length, unsigned int * _indices, int _indice_length) :
	Mesh(_pos, _norm, _tex, _length, true, _indices, _indice_length, GL_TRIANGLES) {

}
Mesh::Mesh(float * _pos, float *_norm, float *_tex, int _length,
	bool _use_ebo, unsigned int * _indices, int _indices_length, GLenum _gl_type) {
	material = (new Material());
	_useTangent = false;
	_useEBO = _use_ebo;
	_primitiveType = _gl_type;
	_vertex_number = _length / 3;
	if (_useEBO)
	{
		_vertex_number = _indices_length;
		indices = vector<unsigned int>{ _indices,_indices + _indices_length };
	}
	Vertex v;
	for (int i = 0; i < _vertex_number * 3; i += 3)
	{
		vec3 p = vec3(_pos[i + 0], _pos[i + 1], _pos[i + 2]);
		vec3 n = vec3(_norm[i + 0], _norm[i + 1], _norm[i + 2]);
		v.position = p;
		v.normal = n;
	}
	for (int i = 0; i < _vertex_number * 2; i += 2)
	{
		vec2 t = vec2(_tex[i + 0], _tex[i + 1]);
		v.texCoords = t;
	}
	vertices.push_back(v);
	init();
}

Mesh::Mesh(float * _vertices,
	int _length,
	int _vertex_size,
	bool _use_ebo,
	unsigned int * _indices,
	int _indices_length,
	GLenum _gl_type)
{
	material = (new Material());
	_useTangent = false;
	_useEBO = _use_ebo;
	_primitiveType = _gl_type;
	_vertex_number = _length / _vertex_size;
	//	primitive_number_ = _vertex_number / 3;
	for (int i = 0; i < _vertex_number*_vertex_size; i += _vertex_size)
	{
		vec3 p = vec3(_vertices[i + 0], _vertices[i + 1], _vertices[i + 2]);
		vec3 n = vec3(_vertices[i + 3], _vertices[i + 4], _vertices[i + 5]);
		vec2 t = vec2(_vertices[i + 6], _vertices[i + 7]);
		vertices.push_back(Vertex(p, n, t));
	}
	if (_useEBO)
	{
		_vertex_number = _indices_length;
		indices = vector<unsigned int>{ _indices,_indices + _indices_length };
	}
	init();
}

Mesh::Mesh(float * _vertices,
	int _length,
	int _vertex_size) :Mesh(_vertices, _length, _vertex_size, false, nullptr, 0, GL_TRIANGLES)
{
	
}

Mesh::Mesh(vector<float>& _vertices,
	int _vertex_size,
	vector<unsigned int>& _indices,
	GLenum _gl_type)
	:Mesh(_vertices.data(), _vertices.size(), _vertex_size, true, _indices.data(), _indices.size(), _gl_type)
{

}

Mesh::Mesh(vector<float> &_vertices,
	int _vertex_size) :Mesh(_vertices, _vertex_size, vector<unsigned int>(), GL_TRIANGLES)
{

}
void Mesh::setMVP(mat4 model, mat4 view, mat4 projection) {
	mvp = MVPTransform(model, view, projection);
}
void Mesh::setMVP(const MVPTransform& mvpt) {
	mvp = mvpt;
}
void Mesh::ClearTextures() {
	material->textureList.clear();
}

void Mesh::setMaterial(Material * mtr)
{
	material=(mtr);
	/*cout << mtr->textureList.size() << " " << material->textureList.size() << endl;*/
}

void Mesh::addTexture(unsigned int  _id, string _name,GLenum _type)
{
	material->textureList.push_back(Texture(_id,_name,_type));
}


void Mesh::draw()
{
	material->shader->setMVPTransform(mvp);
	material->ActiveMaterialInShader();
	glBindVertexArray(_be->VAO);
	if (_useEBO) glDrawElements(_primitiveType, indices.size(), GL_UNSIGNED_INT, 0);
	else glDrawArrays(_primitiveType, 0, _vertex_number);
	glBindVertexArray(0);
}

void Mesh::Render()
{
	glBindVertexArray(_be->VAO);
	if (_useEBO) glDrawElements(_primitiveType, indices.size(), GL_UNSIGNED_INT, 0);
	else glDrawArrays(_primitiveType, 0, _vertex_number);
	glBindVertexArray(0);
}

void Mesh::init()
{
	if (_useEBO)
		if (_useTangent)
			_be = BufferManager::genBindEBOBuffer(&vertices[0], vertices.size(), indices.data(), indices.size(), &tangents[0], tangents.size());
		else _be=BufferManager::genBindEBOBuffer(&vertices[0], vertices.size() * sizeof(Vertex), indices.data(), indices.size() * sizeof(unsigned int));
	else if (_useTangent)
		_be = BufferManager::genBindVAOBuffer(&vertices[0], vertices.size(),&tangents[0],tangents.size());
		else _be=BufferManager::genBindVAOBuffer(&vertices[0], vertices.size() * sizeof(Vertex));

}