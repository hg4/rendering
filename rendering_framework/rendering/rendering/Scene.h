#pragma once
#include <vector>
#include "Light.h"
#include "Camera.h"
//#include "Model.h"
#include "RenderObject.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

//class Model;
//class Light;
//class Camera;

class Scene:public enable_shared_from_this<Scene>
{
public:
	vector<shared_ptr<RenderObject>> renderList;
	//vector<shared_ptr<Model>> modelList;
	vector<shared_ptr<Light>> lightList;
	vector<shared_ptr<Camera>> cameraList;
	shared_ptr<Camera> mainCamera;
	shared_ptr<Light> mainLight;
	int pointLightNum,dirLightNum,spotLightNum,areaLightNum;
	bool useGlobalShader;
	Scene();
	~Scene();
	void AddLight(shared_ptr<Light> l);
	void AddRenderObject(shared_ptr<RenderObject> r);
	//void AddModel(shared_ptr<Model> m);
	void SetCamera(shared_ptr<Camera> c);
	void GenerateShadowMapInMainLight();
	void RenderScene();
	void RenderSceneShadowByGlobalShader(shared_ptr<Shader> shader);

};

