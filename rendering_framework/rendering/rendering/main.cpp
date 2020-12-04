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
#include "WindowManager.h"
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
MVPTransform* genMVP() {
	glm::mat4 model = glm::mat4(1.0);
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 views[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};
	MVPTransform * mvp_arr = new MVPTransform[6];
	for (int i = 0; i < 6; i++) {
		mvp_arr[i] = MVPTransform(model, views[i], projection);
	}
	return mvp_arr;
}

unsigned int genEnvmapFBO(MVPTransform * mvp) {
	BufferElement * be = BufferManager::genBindFRBOBuffer(512, 512);
	Shader equirectangularToCubemapShader("./shader/cube_generator.vs", "./shader/cube_generator.fs");

	//加载hdr数据纹理
	//unsigned int hdrTexture = loadTexture("./hdr/Arches_E_PineTree/Arches_E_PineTree_3k.hdr",true);
	Texture hdr = TextureLoader::loadTexture2D("./hdr/Arches_E_PineTree/Arches_E_PineTree_3k.hdr",
																								"equirectangularMap",
																								GL_FLOAT,
																								false,
																								GL_CLAMP_TO_EDGE
																								);
	unsigned int hdrTexture=hdr.id;
	//分配cubemap的缓冲
	unsigned int envCubemap=TextureLoader::genEmptyTextureCubeMap(512,512,GL_RGB,GL_FLOAT,GL_CLAMP_TO_EDGE,false);
	// ----------------------------------------------------------------------------------------------
	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	equirectangularToCubemapShader.use();
	//equirectangularToCubemapShader.setUniform("equirectangularMap", 0);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, be->FBO);
	Mesh cube = Geometry::createCube();
	cube.addTexture(hdr.id, hdr.name_in_shader, GL_TEXTURE_2D);
	for (unsigned int i = 0; i < 6; ++i)
	{
		cube.setMVP(mvp[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cube.draw(equirectangularToCubemapShader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return envCubemap;
}
//todo: material and pbr preprocess
unsigned int genPrefilterMap(MVPTransform * mvp,unsigned int envCubemap) {
	unsigned int prefilterMap=TextureLoader::genEmptyTextureCubeMap(128,128,GL_RGB,GL_FLOAT,GL_CLAMP_TO_EDGE,true);
	unsigned int FBO=BufferManager::genBindFBOBuffer();
	unsigned int RBO;
	Shader prefilterShader("./shader/cube_generator.vs", "./shader/envmap_spec_prefilter.fs");
	prefilterShader.use();
	Mesh cube = Geometry::createCube();
	cube.addTexture(envCubemap, "environmentMap", GL_TEXTURE_CUBE_MAP);
	unsigned int maxMipLevels = 5;
	for (int mip = 0; mip < maxMipLevels; mip++) {
		int mipWidth = 128 * pow(0.5,mip);
		int mipHeight = 128 * pow(0.5, mip);
		RBO = BufferManager::genBindRBOBuffer(mipHeight, mipWidth);
		glViewport(0, 0, mipWidth, mipHeight);
		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.setUniform("roughness", roughness);
		for (int i = 0; i < 6; i++) {
			cube.setMVP(mvp[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cube.draw(prefilterShader);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return prefilterMap;
}
unsigned int genIrradianceMap(MVPTransform *mvp,unsigned int envCubemap) {
	BufferElement* be=BufferManager::genBindFRBOBuffer(32, 32);
	unsigned int irradianceMap=TextureLoader::genEmptyTextureCubeMap(32,32,GL_RGB,GL_FLOAT,GL_CLAMP_TO_EDGE,false);
	Shader irradianceShader("./shader/cube_generator.vs", "./shader/envmap_preshader.fs");
	irradianceShader.use();
	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	Mesh cube = Geometry::createCube();
	cube.addTexture(envCubemap, "environmentMap", GL_TEXTURE_CUBE_MAP);
	for (unsigned int i = 0; i < 6; ++i)
	{
		cube.setMVP(mvp[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cube.draw(irradianceShader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return irradianceMap;
}
unsigned int genLUT() {
	
	BufferManager::genBindFRBOBuffer(512, 512);
	Shader LUTShader("./shader/shader.vs", "./shader/lut_preshader.fs");
	unsigned int brdfLUT=TextureLoader::genEmptyTexture2D(512,512,GL_RG,GL_FLOAT,GL_CLAMP_TO_EDGE,false);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);
	glViewport(0, 0, 512, 512);
	LUTShader.use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Mesh quad=Geometry::createQuad();
	quad.draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return brdfLUT;
}
unsigned int  genDepthFBO() {
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return depthMap;
}
int main()
{
	 window_manager = new WindowManager(1024, 1024, vec3(0.0, 0.0, -3.0));
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

	string model_name = "bamboo-wood-semigloss";
	string model_path = "./model/" + model_name;
	PbrMaterial * pbrMaterial = new PbrMaterial(model_path.c_str(), model_name.c_str());

	string gunModel = "./model/Cerberus_by_Andrew_Maximov/Cerberus_LP.FBX";
	m = new Model(gunModel);
	string texture_path = "./model/Cerberus_by_Andrew_Maximov/Textures";
	Texture texture_metallic = TextureLoader::loadTexture2D((texture_path + "/Cerberus_M.tga").c_str(),"texture_metallic", GL_UNSIGNED_BYTE, true, GL_CLAMP_TO_EDGE);
	Texture texture_roughness = TextureLoader::loadTexture2D((texture_path + "/Cerberus_R.tga").c_str(), "texture_roughness", GL_UNSIGNED_BYTE, true, GL_CLAMP_TO_EDGE);
	Texture texture_ao = TextureLoader::loadTexture2D((texture_path + "/Cerberus_AO.tga").c_str(), "texture_ao", GL_UNSIGNED_BYTE, true, GL_CLAMP_TO_EDGE);
	Texture texture_albedo = TextureLoader::loadTexture2D((texture_path + "/Cerberus_A.tga").c_str(), "texture_albedo", GL_UNSIGNED_BYTE, true, GL_CLAMP_TO_EDGE);
	m->meshes[0].material->textureList.clear();
	m->meshes[0].addTexture(texture_metallic.id, texture_metallic.name_in_shader, texture_metallic.texture_type);
	m->meshes[0].addTexture(texture_roughness.id, texture_roughness.name_in_shader, texture_roughness.texture_type);
	m->meshes[0].addTexture(texture_ao.id, texture_ao.name_in_shader, texture_ao.texture_type);
	m->meshes[0].addTexture(texture_albedo.id, texture_albedo.name_in_shader, texture_albedo.texture_type);


	MVPTransform * mvp = genMVP();
	unsigned int envCubemap = genEnvmapFBO(mvp);
	unsigned int irradianceMap = genIrradianceMap(mvp,envCubemap);
	unsigned int prefilterMap = genPrefilterMap(mvp,envCubemap);
	unsigned int brdfLUT = genLUT();
	background_cube=Geometry::createCube();
	background_cube.addTexture(envCubemap, "envCubemap", GL_TEXTURE_CUBE_MAP);
	material_sphere = Geometry::createSphere();
	material_sphere.setMaterial(pbrMaterial);
	material_sphere.addTexture(irradianceMap, "irradianceMap", GL_TEXTURE_CUBE_MAP);
	material_sphere.addTexture(prefilterMap, "prefilterMap", GL_TEXTURE_CUBE_MAP);
	material_sphere.addTexture(brdfLUT, "brdfLUT", GL_TEXTURE_2D);

	m->meshes[0].addTexture(irradianceMap, "irradianceMap", GL_TEXTURE_CUBE_MAP);
	m->meshes[0].addTexture(prefilterMap, "prefilterMap", GL_TEXTURE_CUBE_MAP);
	m->meshes[0].addTexture(brdfLUT, "brdfLUT", GL_TEXTURE_2D);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);
		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		glm::mat4 view = window_manager->camera_->GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(window_manager->camera_->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		material_sphere.setMVP(mat4(1.0), view, projection);
		m->meshes[0].setMVP(mat4(1.0), view, projection);
	/*	shader.setUniform("view", view);
		shader.setUniform("projection", projection);*/
		shader.setUniform("viewPos", window_manager->camera_->Position);
		renderScene(shader);
		backgroundShader.use();
		background_cube.setMVP(mat4(1.0), view, projection);
	//	backgroudShader.setUniform("envCubemap", 0);
		/*backgroundShader.setUniform("view", view);
		backgroundShader.setUniform("projection", projection);*/
		background_cube.draw(backgroundShader);
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
		window_manager->camera_->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		window_manager->camera_->ProcessKeyboard(BACKWARD, deltaTime);
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

