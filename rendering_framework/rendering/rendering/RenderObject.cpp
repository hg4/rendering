#include "RenderObject.h"



RenderObject::RenderObject()
{
	mtr = shared_ptr<Material>(new Material());
}


RenderObject::~RenderObject()
{
}

RenderObject::RenderObject(shared_ptr<Mesh> m, const string & objName)
{
	mesh = m;
	name = objName;
	mtr = shared_ptr<Material>(new Material());

}
RenderObject::RenderObject(shared_ptr<Mesh> m, shared_ptr<Material> _mtr, const string & objName)
{
	mesh = m;
	name = objName;
	mtr = _mtr;
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
	mat4 view=parentScene->mainCamera->GetViewMatrix();
	mat4 projection = parentScene->mainCamera->projectionMatrix;
	mtr->shader->setCameraPosition(parentScene->mainCamera->Position);
	for (int i = 0; i < parentScene->lightList.size(); i++) {
		mtr->shader->setLight(*(parentScene->lightList[i]));
	}
	mtr->shader->setMVPTransform(MVPTransform(transform.GetModel(),view,projection));
	mtr->ActiveMaterialInShader();
	mesh->Render();
}

shared_ptr<Shader> RenderObject::GetShader()
{
	if (mtr->hasBindShader) return mtr->shader;
	return nullptr;
}