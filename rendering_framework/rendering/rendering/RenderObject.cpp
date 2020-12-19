#include "RenderObject.h"
#include "Scene.h"


RenderObject::RenderObject()
{
	transform = shared_ptr<Transform>(new Transform());
	mtr = shared_ptr<Material>(new Material());
}


RenderObject::~RenderObject()
{
}

RenderObject::RenderObject(shared_ptr<Mesh> m, const string & objName)
{
	mesh = m;
	name = objName;
	transform = shared_ptr<Transform>(new Transform());
	mtr = shared_ptr<Material>(new Material());

}
RenderObject::RenderObject(shared_ptr<Mesh> m, shared_ptr<Material> _mtr, const string & objName)
{
	mesh = m;
	name = objName;
	mtr = _mtr;
	transform = shared_ptr<Transform>(new Transform());
}

void RenderObject::SetParentScene(shared_ptr<Scene> s)
{
	parentScene = s;
}

void RenderObject::SetMVP(const mat4 & parentModel)
{
	_mvpTransform.model = parentModel * transform->GetModel();
	_mvpTransform.view = parentScene->mainCamera->GetViewMatrix();
	_mvpTransform.projection = parentScene->mainCamera->projectionMatrix;
	_mvpSet = true;
}

void RenderObject::Scale(vec3 s)
{
	transform->SetLocalScale(s);
}

void RenderObject::Rotate(vec3 r)
{
	transform->SetLocalRotate(r);
}

void RenderObject::Translate(vec3 t)
{
	transform->SetTranslate(t);
}



void RenderObject::AddTexture(unsigned int _id, string _name, GLenum _type)
{
	mtr->textureList.push_back(Texture(_id, _name, _type));
}

void RenderObject::SetMaterial(Material * _mtr)
{
	mtr.reset(_mtr);
}

void RenderObject::Render()
{
	//mat4 view=parentScene->mainCamera->GetViewMatrix();
	//mat4 projection = parentScene->mainCamera->projectionMatrix;
	mtr->shader->setCameraPosition(parentScene->mainCamera->Position);
	for (int i = 0; i < parentScene->lightList.size(); i++) {
		mtr->shader->setLight(*(parentScene->lightList[i]));
	}
	if (!_mvpSet) SetMVP(mat4(1.0));
	mtr->shader->setMVPTransform(_mvpTransform);
	mtr->ActiveMaterialInShader();
	mesh->Render();
}

shared_ptr<Shader> RenderObject::GetShader()
{
	if (mtr->hasBindShader) return mtr->shader;
	return nullptr;
}