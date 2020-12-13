#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "MVPTransform.h"
#include "Transform.h"
#include "Material.h"
#include "Texture.h"
#include <memory>
#include "Scene.h"
#include <glad\glad.h>
#include <GLFW\glfw3.h>
class Scene;
class RenderObject
{
public:
	shared_ptr<Mesh> mesh;
	Transform transform;
	shared_ptr<Material> mtr;
	shared_ptr<Scene> parentScene;
	string name;
	RenderObject();
	RenderObject(shared_ptr<Mesh> m,const string& objName);
	RenderObject(shared_ptr<Mesh> m, shared_ptr<Material> _mtr, const string& objName);
	~RenderObject();
	void AddTexture(unsigned int  _id, string _name, GLenum _type);
	void SetMaterial(Material * _mtr);
	void Render();
	shared_ptr<Shader> GetShader();
};

