#pragma once
#include <vector>
#include "Light.h"
#include "Camera.h"
#include "Model.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

class Model;
class Light;
class Camera;

class Scene
{
public:
	vector<shared_ptr<Model>> modelList;
	vector<shared_ptr<Light>> lightList;
	vector<shared_ptr<Camera>> cameraList;
	shared_ptr<Camera> mainCamera;
	
	Scene();
	~Scene();
	void AddLight(shared_ptr<Light> l);
	void AddModel(shared_ptr<Model> m);
	void SetCamera(shared_ptr<Camera> c);
	void RenderScene();
};

