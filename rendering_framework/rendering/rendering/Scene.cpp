#include "Scene.h"



Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::AddLight(shared_ptr<Light> l)
{
}

//void Scene::AddModel(shared_ptr<Model> m)
//{
//	shared_ptr<Model> p(m);
//	p->SetParentScene(shared_ptr<Scene>(this));
//	modelList.push_back(p);
//}

void Scene::SetCamera(shared_ptr<Camera> c)
{
	mainCamera = c;
}

//void Scene::RenderScene()
//{
//	for (int i = 0; i < modelList.size(); i++) 
//	{
//		modelList[i]->Render();
//	}
//}

void Scene::RenderScene()
{
	for (int i = 0; i < renderList.size(); i++) 
	{
		renderList[i]->SetMVP(mat4(1.0));
		renderList[i]->Render();
	}
}

void Scene::AddRenderObject(shared_ptr<RenderObject> r)
{
	r->SetParentScene(shared_ptr<Scene>(this));
	renderList.push_back(r);
}