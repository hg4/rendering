
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "MVPTransform.h"
#include "Model.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "BufferManager.h"
#include "Geometry.h"
#include "SkyboxMaterial.h"
#include "PbrEnvMaterial.h"
#include "WindowManager.h"
#include "MaterialList.h"
#include "timer.h"
#include "TextRenderer.h"
#include "Scene.h"
#include "PointLight.h"
#include "DirLight.h"
#include "Common.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


using namespace std;



void processInput(GLFWwindow *window,shared_ptr<Scene> s);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
enum class AXIS{X,Y,Z};

struct GlobalControl
{
	AXIS axis;
	shared_ptr<RenderObject> focus;
};

// camera
//Camera camera(glm::vec3(0.0f, 0.0f, -3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
// meshes
float sheen = 0.04f;
int index = 0;
WindowManager* window_manager;
GlobalControl global;
//flag
bool pbr_env_on = true;

//obj

shared_ptr<Scene> s;

void PrintVec4(const vec4& ans){
	cout << ans[0] << " " << ans[1] << " " << ans[2]<<" " <<ans[3]<< endl;
}
void PrintMat4(const mat4& mat) {
	cout << "mat4*4:\n";
	cout << mat[0][0] << " " << mat[1][0] <<" "<< mat[2][0] << " " << mat[3][0] << endl;
	cout << mat[0][1] << " " << mat[1][1] << " " << mat[2][1] << " " << mat[3][1] << endl;
	cout << mat[0][2] << " " << mat[1][2] << " " << mat[2][2] << " " << mat[3][2] << endl;
	cout << mat[0][3] << " " << mat[1][3] << " " << mat[2][3] << " " << mat[3][3] << endl;
}

void ShowFPS() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float fps = 1.0 / deltaTime;
	string s = "FPS:"+to_string(fps);
	int dotpos = s.find_first_of('.');
	if (dotpos + 2 < s.length()) s = s.substr(0, dotpos + 3);
	TextRenderer::T->RenderText(s.c_str(), 10.0f, 10.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
	glDisable(GL_BLEND);
	//printf("fps:%.2f\n", fps);
}
shared_ptr<Scene> ComposeScene() {
	shared_ptr<Scene> myScene(new Scene());
	shared_ptr<Shader> clothShader(new Shader("./shader/pbr_shader.vs", "./shader/cloth_pbr_shader.fs"));
	shared_ptr<Shader> shader(new Shader("./shader/pbr_shader.vs", "./shader/pbr_shader.fs"));
	shared_ptr<Shader> backgroundShader(new Shader("./shader/cubemap_shader.vs", "./shader/cubemap_shader.fs"));
	shared_ptr<Shader> quadShader(new Shader("./shader/plane_shader.vs", "./shader/plane_shader.fs"));
	shared_ptr<Shader> phongShader(new Shader("./shader/pbr_shader.vs", "./shader/phong_shader.fs"));
	// load textures
	// -------------
	string model_name = "bamboo_wood_semigloss";
	string model_path = "./model/" + model_name;
	string gunModel = "./model/Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX";

	shared_ptr<Model> md(new Model(gunModel,"Cerberus_LP"));
	string texture_path = "./model/Cerberus_by_Andrew_Maximov/Textures";
	shared_ptr<PbrMaterial> pbrPlaneMaterial(new PbrMaterial(model_path.c_str(), model_name.c_str(), "png"));

	//shared_ptr<SkyboxMaterial> skybox(new SkyboxMaterial("./hdr/Arches_E_PineTree/Arches_E_PineTree_3k.hdr"));
	shared_ptr<SkyboxMaterial> skybox(new SkyboxMaterial("./hdr/Sunset/SunsetSkyboxHDR.hdr"));
	skybox->BindShader(backgroundShader);

	shared_ptr<PbrMaterial> gunPbrMaterial(new PbrMaterial(texture_path.c_str(), "Cerberus", "tga"));
	shared_ptr<PbrEnvMaterial> pbrEnv(new PbrEnvMaterial(skybox));
	shared_ptr<MaterialList> mtrList(new MaterialList(gunPbrMaterial, pbrEnv));
	mtrList->BindShader(shader);
	shared_ptr<MaterialList> pbrPlane(new MaterialList(pbrPlaneMaterial, pbrEnv));
	pbrPlane->BindShader(shader);
	//md->ClearRenderObjectTextures("Default");
	skybox->textureList.push_back(pbrEnv->textureList[PBRENV_PARAMETER::IRRADIANCEMAP]);
	md->Scale(vec3(0.1f, 0.1f, 0.1f));
	md->Rotate(vec3(0.0f,-90.0f,-90.0f));
	md->Translate(vec3(0.0f, 0.0f, -30.0f));
	md->SetMaterial(mtrList);
	shared_ptr<RenderObject> sky(new RenderObject(Geometry::createCube(), skybox, "skybox"));
	shared_ptr<RenderObject> plane(new RenderObject(Geometry::createPlane(), "plane"));
	md->enableShadowEffect();
	/*sphere->enableShadowEffect();
	sphere->SetMaterial(pbrPlane);*/
	plane->enableShadowEffect();
	plane->SetMaterial(pbrPlane);
	//plane->mtr->BindShader(shader);
	plane->Scale(vec3(3.0f, 3.0f, 3.0f));
	plane->Translate(vec3(0.0f, -5.5f, 0.0f));

	// cloth 
	string clothModel = "./model/cloth-base-mesh/Cloth.obj";
	string bagModel = "./model/little-natural-bag/model.dae";
	shared_ptr<Model> cloth(new Model(clothModel, "Cloth"));
	shared_ptr<Model> bag(new Model(bagModel, "Bag"));
	string clothTexturePath = "./model/cloth-base-mesh/fabric_0026_4k";
	shared_ptr<PbrMaterial> pbrClothMaterial(new PbrMaterial(clothTexturePath.c_str(), "Cloth", "jpg"));

	shared_ptr<MaterialList> clothMtr(new MaterialList(pbrClothMaterial, pbrEnv));


	clothMtr->BindShader(clothShader);
	bag->SetMaterial(clothMtr);
	cloth->SetMaterial(clothMtr);
	//cloth->enableShadowEffect();
	cloth->Scale(vec3(3.0f, 3.0f, 3.0f));
	//cloth->Rotate(vec3(90.0f, 0.0f, 0.0f));
	cloth->Translate(vec3(0.0f, -2.0f, 0.f));
	//myScene->AddRenderObject(bag);
	//myScene->AddRenderObject(md);
	myScene->AddRenderObject(cloth);
	//myScene->AddRenderObject(plane);
	myScene->AddRenderObject(sky);
	shared_ptr<Light> dirL(new DirLight(vec3(0.0f, 36.0f, 10.0f), vec3(0.0f, -1.0f, -1.0f), 2.f, vec3(1.0f, 1.0f, 1.0f)));
	myScene->mainLight = dirL;
	myScene->AddLight(shared_ptr<Light>(new PointLight(vec3(8.0f, 5.0f, -28.0f),50.0f,vec3(1.0f, 1.0f, 1.0f))));
	myScene->AddLight(dirL);
	return myScene;
}
int main()
{
	 window_manager = new WindowManager(1024, 1024, vec3(0.0f, 0.0f, 3.0f));
	GLFWwindow * window = window_manager->getWindow();
	glfwSetKeyCallback(window, key_callback);
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	if (!TextRenderer::T->initialized) TextRenderer::T->Init();
	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	
	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
	// build and compile shaders
	// -------------------------
	//Shader shader("./shader/heightmap_shader.vs", "./shader/pbr_shader.fs");
	//Shader backgroundShader("./shader/cubemap_shader.vs", "./shader/cubemap_shader.fs");
	shared_ptr<Shader> quadShader(new Shader("./shader/shader.vs", "./shader/shader.fs"));
	// load textures
	// -------------
	TIMING_BEGIN("preprecess begin.")
	/*string model_name = "bamboo_wood_semigloss";
	string model_path = "./model/" + model_name;
	PbrMaterial * pbrMaterial = new PbrMaterial(model_path.c_str(), model_name.c_str(),"png");
*/
	s = ComposeScene();
	s->SetCamera(shared_ptr<Camera>(window_manager->camera_));
	global.focus = s->renderList[index++];
	cout << "now focus object: " << global.focus->name << endl;
	/*string gunModel = "./model/Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX";
	m = new Model(gunModel);
	string texture_path = "./model/Cerberus_by_Andrew_Maximov/Textures";
	PbrMaterial * gunPbrMaterial = new PbrMaterial(texture_path.c_str(), "Cerberus", "tga");*/
	//SkyboxMaterial* skybox = new SkyboxMaterial("./hdr/Arches_E_PineTree/Arches_E_PineTree_3k.hdr");

	//SkyboxMaterial* skybox = new SkyboxMaterial("./model/skybox","","jpg");
	//PbrEnvMaterial* pbrEnv = new PbrEnvMaterial(skybox);
	//MaterialList* mtrList = new MaterialList(gunPbrMaterial, pbrEnv);
	//MaterialList* mtrlist0 = new MaterialList(pbrMaterial, pbrEnv);
	//m->meshes[0]->material->textureList.clear();

//	mtrList->BindShader(shader);
//	mtrlist0->BindShader(shader);
	//shared_ptr<Mesh> background_cube=Geometry::createCube();
//	//background_cube.addTexture(pbrEnv->textureList[PREFILTERMAP].id, "envCubemap", GL_TEXTURE_CUBE_MAP);
	//background_cube->setMaterial(skybox);
	//background_cube->material->BindShader(backgroundShader);
//	material_sphere = Geometry::createSphere();
//	material_sphere.setMaterial(mtrlist0);

	TIMING_END("preprocess end.")
	
	s->GenerateShadowMapInMainLight();
	shared_ptr<Mesh> background = Geometry::createQuad();
	background->addTexture(s->mainLight->shadowMap.id, "depthMap", GL_TEXTURE_2D);
	background->material->BindShader(quadShader);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	/*m->meshes[0]->setMaterial(mtrList);
	m->meshes[0]->transform.SetLocalScale(vec3(0.3f, 0.3f, 0.3f));
	m->meshes[0]->transform.SetLocalRotate(vec3(0.0f, -90.0f, -90.0f));
	m->meshes[0]->transform.SetTranslate(vec3(10.0f, 0.0f, -30.0f));*/
	/*glm::mat4 model = mat4(1.0);
*/
	//glm::mat4 sphereModel = mat4(1.0);
	//sphereModel = translate(sphereModel, vec3(0.0f, -20.0f, 0.0f));
	//sphereModel = scale(sphereModel, vec3(10.0f));

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		//ShowFPS();
		// input
		// -----
		//processInput(window,s);
		// render
		// ------
		s->GenerateShadowMapInMainLight();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glm::mat4 view = window_manager->camera_->GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(window_manager->camera_->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, nearPlane,farPlane);
		window_manager->camera_->SetProjection(projection);
		
		////shader.use();
		//shader.setCameraPosition(window_manager->camera_->Position);
		//
		//material_sphere.setMVP(sphereModel, view, projection);
		//material_sphere.draw();
		
		//m->setMVP(MVPTransform(m->meshes[0]->transform.GetModel(), view, projection));
		//m->Draw();
	
		
		///*background.setMVP(mat4(1.0), view, projection);
		//background->draw();
	/*	background_cube->setMVP(mat4(1.0), view, projection);
		background_cube->draw();*/

//		s->renderList[0]->mtr->shader->setUniform("sheen", sheen);
		s->RenderScene();

		ShowFPS();
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(FORWARD, deltaTime*3);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(BACKWARD, deltaTime * 3);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(LEFT, deltaTime * 3);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(RIGHT, deltaTime * 3);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(DOWN, deltaTime * 3);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(UP, deltaTime * 3);
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		global.axis = AXIS::X;
		cout << "x axis is selected.\n";
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		global.axis = AXIS::Y;
		cout << "y axis is selected.\n";
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		global.axis = AXIS::Z;
		cout << "z axis is selected.\n";
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		index = (index%s->renderList.size());
		global.focus = s->renderList[index++];
		cout << "now focus object: " << global.focus->name <<endl;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		vec3 euler = global.focus->transform->GetEulerAngle();
		if (global.axis == AXIS::X)
			euler += vec3(1.0f, 0.0f, 0.0f);
		else if (global.axis == AXIS::Y)
			euler += vec3(0.0f, 1.0f, 0.0f);
		else if (global.axis == AXIS::Z)
			euler += vec3(0.0f, 0.0f, 1.0f);
		global.focus->transform->SetLocalRotate(euler);
		cout << "focus rotation. now euler angle is : " << euler.x << " " << euler.y << " " << euler.z << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		vec3 euler = s->mainLight->GetTransform()->GetEulerAngle();
		if(global.axis == AXIS::X)
			euler += vec3(1.0f,0.0f,0.0f);
		else if(global.axis == AXIS::Y)
			euler += vec3(0.0f, 1.0f, 0.0f);
		else if(global.axis == AXIS::Z)
			euler += vec3(0.0f, 0.0f, 1.0f);
		s->mainLight->GetTransform()->SetLocalRotate(euler);
		cout << "light rotation. now euler angle is : " << euler.x << " " << euler.y << " " << euler.z << endl;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		sheen += 0.01;
		cout << "sheen:" << sheen << endl;
	}
}

//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void processInput(GLFWwindow *window, shared_ptr<Scene> s)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		window_manager->camera_->ProcessKeyboard(FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		window_manager->camera_->ProcessKeyboard(BACKWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		window_manager->camera_->ProcessKeyboard(LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		window_manager->camera_->ProcessKeyboard(RIGHT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//		window_manager->camera_->ProcessKeyboard(DOWN, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//		window_manager->camera_->ProcessKeyboard(UP, deltaTime);
//
//	/*if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
//		s->mainLight->direction.x += 0.02;
//	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
//		s->mainLight->direction.x -= 0.02;
//	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
//		s->mainLight->direction.y -= 0.02;
//	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
//		s->mainLight->direction.y += 0.02;
//	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
//		s->mainLight->direction.z -= 0.02;
//	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
//		s->mainLight->direction.z += 0.02;
//	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
//		s->mainLight->shadowBias += 0.001;
//	}
//	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
//		s->mainLight->shadowBias -= 0.001;
//	}*/
//	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
//		global.axis = AXIS::X;
//		cout << "x axis is selected.\n";
//	}
//	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
//		global.axis = AXIS::Y;
//		cout << "y axis is selected.\n";
//	}
//	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
//		global.axis = AXIS::Z;
//		cout << "z axis is selected.\n";
//	}
//	
//	/*if (glfwGetKey(window, GLFW_MOD_SHIFT&&GLFW_KEY_TAB) == GLFW_PRESS) {
//		if (global.focus->type) {
//			global.focus = dynamic_cast<Model*>(global.focus.get())->GetRenderObjs()[0];
//		}
//	}
//	else */
//	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
//		global.focus = s->renderList[index++];
//	}
//	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
//		global.axis = AXIS::Z;
//		cout << "z axis is selected.\n";
//	}
//	/*if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
//		vec3 euler = s->mainLight->GetTransform()->GetEulerAngle();
//		if(global.axis == AXIS::X)
//			euler += vec3(1.0f,0.0f,0.0f);
//		else if(global.axis == AXIS::Y)
//			euler += vec3(0.0f, 1.0f, 0.0f);
//		else if(global.axis == AXIS::Z)
//			euler += vec3(0.0f, 0.0f, 1.0f);
//		s->mainLight->GetTransform()->SetLocalRotate(euler);
//		cout << "light rotation. now euler angle is : " << euler.x << " " << euler.y << " " << euler.z << endl;
//	}*/
//}
//
