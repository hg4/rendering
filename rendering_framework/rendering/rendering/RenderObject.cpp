#include "RenderObject.h"
#include "Scene.h"


RenderObject::RenderObject()
{
	transform = shared_ptr<Transform>(new Transform());
	mtr = shared_ptr<Material>(new Material());
	castShadow = false;
	recieveShadow = false;
}


RenderObject::~RenderObject()
{
}

RenderObject::RenderObject(shared_ptr<Mesh> m, const string & objName)
{
	mesh = m;
	name = objName;
	type = false;
	transform = shared_ptr<Transform>(new Transform());
	mtr = shared_ptr<Material>(new Material());

}
RenderObject::RenderObject(shared_ptr<Mesh> m, shared_ptr<Material> _mtr, const string & objName)
{
	mesh = m;
	name = objName;
	mtr = _mtr;
	type = false;
	transform = shared_ptr<Transform>(new Transform());
}

void RenderObject::SetParentScene(shared_ptr<Scene> s)
{
	parentScene = s;
}

void RenderObject::SetMVP(const mat4 & parentModel)
{
	_mvpTransform.model = parentModel * transform->GetModel();
	_mvpTransform.view = shared_ptr<Scene>(parentScene)->mainCamera->GetViewMatrix();
	_mvpTransform.projection = shared_ptr<Scene>(parentScene)->mainCamera->projectionMatrix;
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

void RenderObject::enableShadowEffect()
{
	castShadow = true;
	recieveShadow = true;

}


void RenderObject::AddTexture(unsigned int _id, string _name, GLenum _type)
{
	mtr->textureList.push_back(Texture(_id, _name, _type));
}

void RenderObject::SetMaterial(shared_ptr<Material> _mtr)
{
	mtr = _mtr;
}

void RenderObject::Render()
{
	//mat4 view=parentScene->mainCamera->GetViewMatrix();
	//mat4 projection = parentScene->mainCamera->projectionMatrix;
	mtr->shader->setCameraPosition(shared_ptr<Scene>(parentScene)->mainCamera->Position);
	mtr->shader->setUniform("pointLightNum", shared_ptr<Scene>(parentScene)->pointLightNum);
	mtr->shader->setUniform("dirLightNum", shared_ptr<Scene>(parentScene)->dirLightNum);
	for (int i = 0; i < shared_ptr<Scene>(parentScene)->lightList.size(); i++) {
		shared_ptr<Light> l = shared_ptr<Scene>(parentScene)->lightList[i];
		mtr->shader->setLight(l,l->numberId);
	}
	mtr->shader->setUniform("enableShadow", recieveShadow);
	if (recieveShadow) {
		shared_ptr<Light> l = shared_ptr<Scene>(parentScene)->mainLight;
		mtr->shader->setUniform("LIGHTSPACE_MATRIX_VP", l->LIGHTSPACE_MATRIX_VP);
		mtr->shader->setUniform("bias", l->shadowBias);
		mtr->textureList.push_back(l->shadowMap);
	}
	if (!_mvpSet) SetMVP(mat4(1.0));
	mtr->shader->setMVPTransform(_mvpTransform);

	mtr->ActiveMaterialInShader();
	mesh->Render();
}
void RenderObject::RenderByTempGlobalShader(shared_ptr<Shader> shader) {
	shared_ptr<Shader> temp = mtr->shader;
	mtr->shader = shader;
	if (!_mvpSet) SetMVP(mat4(1.0));
	mtr->shader->setMVPTransform(_mvpTransform);
	mtr->ActiveMaterialInShader();
	mesh->Render();
	mtr->shader = temp;
}

shared_ptr<Shader> RenderObject::GetShader()
{
	if (mtr->hasBindShader) return mtr->shader;
	return nullptr;
}