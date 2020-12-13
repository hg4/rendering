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

void Scene::AddModel(shared_ptr<Model> m)
{
	shared_ptr<Model> p(m);
	p->SetParentScene(shared_ptr<Scene>(this));
	modelList.push_back(p);
}

void Scene::SetCamera(shared_ptr<Camera> c)
{
	mainCamera = c;
}

void Scene::RenderScene()
{
	for (int i = 0; i < modelList.size(); i++) 
	{
		modelList[i]->Render();
	}
}

