#include "Scene.h"



Scene::Scene()
{
	useGlobalShader = false;
}


Scene::~Scene()
{
	cout << "delete scene.\n";
}

void Scene::AddLight(shared_ptr<Light> l)
{
	if (l->lightType == LIGHTTYPE::POINT) l->numberId = pointLightNum++;
	if (l->lightType == LIGHTTYPE::DIR) l->numberId = dirLightNum++;
	if (l->lightType == LIGHTTYPE::SPOT) l->numberId = spotLightNum++;
	if (l->lightType == LIGHTTYPE::AREA) l->numberId = areaLightNum++;
	lightList.push_back(l);
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
void Scene::RenderSceneShadowByGlobalShader(shared_ptr<Shader> shader)
{
	for (int i = 0; i < renderList.size(); i++)
	{
		renderList[i]->SetMVP(mat4(1.0));
		if(renderList[i]->castShadow)
		renderList[i]->RenderByTempGlobalShader(shader);
	}
}

void Scene::RenderScene()
{
	
	for (int i = 0; i < renderList.size(); i++) 
	{
		
		/*for (int j = 0; j < lightList.size(); j++) {		
			renderList[i]->GetShader()->setLight(lightList[j], j);
		}*/
		
		renderList[i]->SetMVP(mat4(1.0));
		renderList[i]->Render();
	}
}

void Scene::AddRenderObject(shared_ptr<RenderObject> r)
{
	r->SetParentScene(shared_from_this());
	renderList.push_back(r);
}

void Scene::GenerateShadowMapInMainLight()
{
	mainLight->UpdateShadowMap(shared_from_this());
}