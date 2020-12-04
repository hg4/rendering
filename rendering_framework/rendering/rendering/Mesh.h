#pragma once
#include "Vertex.h"
#include <vector>
#include <glad/glad.h>
#include "Shader.h"
#include "Texture.h"
#include "BufferManager.h"
#include "MVPTransform.h"
#include "Material.h"
#include "PbrMaterial.h"
using namespace std;
using namespace glm;

class Mesh{
public:
	Mesh(){}
	Mesh(vector<Vertex> &_v, vector<unsigned int> _i, vector<Texture> &_t,bool _use_ebo=true,GLenum _gl_type=GL_TRIANGLES){
		material = new PbrMaterial();
		_useEBO = _use_ebo;
		_primitiveType = _gl_type;
		vertices = _v;
		indices = _i;
		material->textureList = _t;
		init();
	}
	//只能用于三角形类型的接口，不能画line
	Mesh(float * _pos, float *_norm, float *_tex, int _length) :Mesh(_pos,_norm,_tex,_length,false,NULL,0,GL_TRIANGLES){
		material = new PbrMaterial();
	}
	Mesh(float * _pos, float *_norm, float *_tex, int _length, unsigned int * _indices, int _indice_length) :
		Mesh(_pos, _norm, _tex, _length, true, _indices, _indice_length, GL_TRIANGLES) {
		material = new PbrMaterial();
	}
	Mesh(float * _pos, float *_norm, float *_tex,int _length, 
		bool _use_ebo , unsigned int * _indices , int _indices_length,GLenum _gl_type){
		material = new PbrMaterial();
		_useEBO = _use_ebo;
		_primitiveType = _gl_type;
		_vertex_number = _length / 3;
		if (_useEBO) 
		{
			_vertex_number = _indices_length;
			indices = vector<unsigned int>{ _indices,_indices + _indices_length };
		}
		Vertex v;
		for (int i = 0; i < _vertex_number*3; i+=3) 
		{
			vec3 p = vec3(_pos[i + 0],_pos[i+1],_pos[i+2]);
			vec3 n = vec3(_norm[i + 0], _norm[i + 1], _norm[i + 2]);
			v.position = p;
			v.normal = n;
		}
		for (int i = 0; i < _vertex_number*2; i += 2) 
		{
			vec2 t = vec2(_tex[i + 0], _tex[i + 1]);
			v.texCoords = t;
		}
		vertices.push_back(v);
		init();
	}

	Mesh(float * _vertices,
		int _length, 
		int _vertex_size,
		bool _use_ebo , 
		unsigned int * _indices, 
		int _indices_length, 
		GLenum _gl_type) 
	{
		material = new PbrMaterial();
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

	Mesh(float * _vertices,
		int _length,
		int _vertex_size) :Mesh(_vertices, _length, _vertex_size, false, nullptr, 0, GL_TRIANGLES) 
	{
		material = new PbrMaterial();
	}

	Mesh(vector<float>& _vertices,
		int _vertex_size,
		vector<unsigned int>& _indices, 
		GLenum _gl_type) 
		:Mesh(_vertices.data(), _vertices.size(), _vertex_size, true, _indices.data(), _indices.size(), _gl_type)
	{
		material = new PbrMaterial();
	}

	Mesh(vector<float> &_vertices,
		int _vertex_size) :Mesh(_vertices, _vertex_size, vector<unsigned int>(), GL_TRIANGLES) 
	{
		material = new PbrMaterial();
	}
	void setMVP(mat4 model, mat4 view, mat4 projection) {
		mvp = MVPTransform(model, view, projection);
	}
	void setMVP(const MVPTransform& mvpt) {
		mvp = mvpt;
	}
	void setMaterial(Material * mtr);
	void addTexture(unsigned int  _id, string _name, GLenum _type);
	//Mesh(const string path);
	void draw(Shader &shader);
	void draw();
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	//vector<Texture> textures;
	Material * material;
	MVPTransform mvp;
private:
	bool _useEBO;
	GLenum _primitiveType;
	int _vertex_number;//不用EBO的时候表示要画的顶点数，用EBO的时候表示indice的数量
	BufferElement * _be;
	void init();

};