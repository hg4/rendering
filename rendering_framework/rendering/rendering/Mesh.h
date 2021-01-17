#pragma once
#include "Vertex.h"
#include <vector>
#include <glad/glad.h>
#include "Shader.h"
#include "Texture.h"
#include "BufferManager.h"
#include "MVPTransform.h"
#include "Transform.h"
#include "Material.h"
#include "PbrMaterial.h"
#include <memory>
using namespace std;
using namespace glm;

class Mesh{
public:
	Mesh(){}
	Mesh(vector<Vertex>& _v, vector<unsigned int>& _i, vector<Texture>& _t, GLenum _gl_type = GL_TRIANGLES);
	Mesh(vector<Vertex>& _v, vector<unsigned int>& _i, vector<Texture>& _t, vector<Tangent> &_tg);
	Mesh(float * _pos, float * _norm, float * _tex, int _length);
	Mesh(float * _pos, float * _norm, float * _tex, int _length, unsigned int * _indices, int _indice_length);
	Mesh(float * _pos, float * _norm, float * _tex, int _length, bool _use_ebo, unsigned int * _indices, int _indices_length, GLenum _gl_type);
	Mesh(float * _vertices, int _length, int _vertex_size, bool _use_ebo, unsigned int * _indices, int _indices_length, GLenum _gl_type);
	Mesh(float * _vertices, int _length, int _vertex_size);
	Mesh(vector<float>& _vertices, int _vertex_size, vector<unsigned int>& _indices, GLenum _gl_type);
	Mesh(vector<float>& _vertices, int _vertex_size);
	void setMVP(mat4 model, mat4 view, mat4 projection);
	void setMVP(const MVPTransform & mvpt);
	void setMaterial(shared_ptr<Material> mtr);
	void ClearTextures();
	void addTexture(unsigned int  _id, string _name, GLenum _type);
	//Mesh(const string path);
	void draw();
	void Render();
	shared_ptr<BufferElement> GetBufferElement() { return _be; }
	vector<Vertex> vertices;
	vector<Tangent> tangents;
	vector<unsigned int> indices;
	//vector<Texture> textures;
	shared_ptr<Material> material;
	//Material* material;
	MVPTransform mvp;
	Transform transform;
private:
	bool _useEBO,_useTangent;
	GLenum _primitiveType;
	int _vertex_number;//不用EBO的时候表示要画的顶点数，用EBO的时候表示indice的数量
	shared_ptr<BufferElement> _be;
	void init();

};