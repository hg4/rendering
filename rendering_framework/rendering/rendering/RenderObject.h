#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "MVPTransform.h"
#include "Transform.h"
#include "Material.h"
#include "Texture.h"
#include <memory>

#include <glad\glad.h>
#include <GLFW\glfw3.h>

class Scene;
class RenderObject
{
public:

	shared_ptr<Transform> transform;
	shared_ptr<Material> mtr;
	shared_ptr<Scene> parentScene;
	shared_ptr<RenderObject> parent;
	string name;
	RenderObject();
	RenderObject(shared_ptr<Mesh> m,const string& objName);
	RenderObject(shared_ptr<Mesh> m, shared_ptr<Material> _mtr, const string& objName);
	~RenderObject();

	virtual void SetMaterial(Material * _mtr);
	virtual void Scale(vec3 s);
	virtual void Rotate(vec3 r);
	virtual void Translate(vec3 t);
	virtual void SetMVP(const mat4& parentModel);
	virtual void Render();
	virtual void SetParentScene(shared_ptr<Scene> s);

	void AddTexture(unsigned int  _id, string _name, GLenum _type);
	shared_ptr<Shader> GetShader();

private:
	bool _mvpSet;
	shared_ptr<Mesh> mesh;
	MVPTransform _mvpTransform;
};
