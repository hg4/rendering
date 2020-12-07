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
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace std;


//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
//unsigned int loadTexture(const char *path,bool loadFloat=false);
void renderScene( Shader &shader);


// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 1024;

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
const char *  model_path = "./model/nanosuit/nanosuit.obj";
unsigned int planeVAO;
WindowManager* window_manager;

//flag
bool pbr_env_on = true;

//obj
Mesh material_sphere,background_cube;
Model * m;

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
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float fps = 1.0 / deltaTime;
	printf("fps:%.2f\n", fps);
}
int main()
{
	 window_manager = new WindowManager(1024, 1024, vec3(0.0f, 0.0f, 80.0f));
	GLFWwindow * window = window_manager->getWindow();
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.
	// build and compile shaders
	// -------------------------
	Shader shader("./shader/heightmap_shader.vs", "./shader/pbr_shader.fs");
	Shader backgroundShader("./shader/cubemap_shader.vs", "./shader/cubemap_shader.fs");

	// load textures
	// -------------

	string model_name = "bamboo_wood_semigloss";
	string model_path = "./model/" + model_name;
	PbrMaterial * pbrMaterial = new PbrMaterial(model_path.c_str(), model_name.c_str(),"png");

	string gunModel = "./model/Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX";
	m = new Model(gunModel);
	string texture_path = "./model/Cerberus_by_Andrew_Maximov/Textures";
	PbrMaterial * gunPbrMaterial = new PbrMaterial(texture_path.c_str(), "Cerberus", "tga");
	//SkyboxMaterial* skybox = new SkyboxMaterial("./hdr/Arches_E_PineTree/Arches_E_PineTree_3k.hdr");
	SkyboxMaterial* skybox = new SkyboxMaterial("./model/skybox","box","jpg");
	PbrEnvMaterial* pbrEnv = new PbrEnvMaterial(skybox);
	MaterialList* mtrList = new MaterialList(gunPbrMaterial, pbrEnv);
	MaterialList* mtrlist0 = new MaterialList(pbrMaterial, pbrEnv);
	m->meshes[0].material->textureList.clear();
	//m->meshes[0].setMaterial(gunPbrMaterial);
	
	//pbrMaterial->BindShader(shader);
	mtrList->BindShader(shader);
	mtrlist0->BindShader(shader);
	background_cube=Geometry::createCube();
	//background_cube.addTexture(envCubemap, "envCubemap", GL_TEXTURE_CUBE_MAP);
	background_cube.setMaterial(skybox);
	background_cube.material->BindShader(backgroundShader);
	material_sphere = Geometry::createSphere();
	material_sphere.setMaterial(mtrlist0);
	

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	m->meshes[0].setMaterial(mtrList);
	m->meshes[0].transform.SetLocalScale(vec3(0.3f, 0.3f, 0.3f));
	m->meshes[0].transform.SetLocalRotate(vec3(0.0f, -90.0f, -90.0f));
	m->meshes[0].transform.SetTranslate(vec3(10.0f, 0.0f, -30.0f));
	/*glm::mat4 model = mat4(1.0);
*/
	glm::mat4 sphereModel = mat4(1.0);
	sphereModel = translate(sphereModel, vec3(0.0f, -20.0f, 0.0f));
	sphereModel = scale(sphereModel, vec3(10.0f));
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		//ShowFPS();
		// input
		// -----
		processInput(window);
		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glm::mat4 view = window_manager->camera_->GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(window_manager->camera_->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		
		//shader.use();
		/*shader.setCameraPosition(window_manager->camera_->Position);
		
		material_sphere.setMVP(sphereModel, view, projection);
		material_sphere.draw();

		m->setMVP(MVPTransform(m->meshes[0].transform.GetModel(), view, projection));
		m->Draw();*/
	
		background_cube.setMVP(mat4(1.0), view, projection);
		background_cube.draw();
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}

// renders the 3D scene
// --------------------
void renderScene(Shader &shader)
{
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;
	// floor
	glm::mat4 model = glm::mat4(1.0f);
	shader.setUniform("model", model);
	m->Draw(shader);
	//material_sphere.draw(shader);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(FORWARD, deltaTime*10);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(BACKWARD, deltaTime*10);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		window_manager->camera_->updateCameraVectors(1.0, window_manager->camera_->Right);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		window_manager->camera_->updateCameraVectors(-1.0, window_manager->camera_->Right);
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		window_manager->camera_->updateCameraVectors(1.0, window_manager->camera_->Up);
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		window_manager->camera_->updateCameraVectors(-1.0, window_manager->camera_->Up);
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		lightPos.y += 0.2;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		lightPos.y -= 0.2;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		lightPos.x -= 0.2;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		lightPos.x += 0.2;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		lightPos.z -= 0.2;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		lightPos.z += 0.2;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		pbr_env_on = !pbr_env_on;
	}
}

