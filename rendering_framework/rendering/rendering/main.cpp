#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "TextureLoader.h"
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
//texture
unsigned int texture_albedo, texture_ao, texture_normal, texture_metal, texture_roughness;
//flag
bool pbr_env_on = true;

unsigned int genEnvmapFBO() {
	unsigned int captureFBO;
	unsigned int captureRBO;
	Shader equirectangularToCubemapShader("./shader/cube_generator.vs", "./shader/cube_generator.fs");
	glGenFramebuffers(1, &captureFBO);
	//创建RBO作为FBO的深度缓冲，因为不采样深度用RBO效率更高
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	//加载hdr数据纹理
	//unsigned int hdrTexture = loadTexture("./hdr/Arches_E_PineTree/Arches_E_PineTree_3k.hdr",true);
	unsigned int hdrTexture = TextureLoader::loadTexture2D("./hdr/Arches_E_PineTree/Arches_E_PineTree_3k.hdr", GL_FLOAT);
	//分配cubemap的缓冲
	unsigned int envCubemap=TextureLoader::genEmptyTextureCubeMap(512,512,GL_RGB,GL_FLOAT,GL_CLAMP_TO_EDGE,false);
	//unsigned int envCubemap;
	//glGenTextures(1, &envCubemap);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	//for (unsigned int i = 0; i < 6; ++i)
	//{
	//	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	//}
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};
	
	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	equirectangularToCubemapShader.use();
	equirectangularToCubemapShader.setUniform("equirectangularMap", 0);
	equirectangularToCubemapShader.setUniform("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equirectangularToCubemapShader.setUniform("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Geometry::renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return envCubemap;
}

unsigned int genPrefilterMap(unsigned int envCubemap) {
	unsigned int prefilterMap=TextureLoader::genEmptyTextureCubeMap(128,128,GL_RGB,GL_FLOAT,GL_CLAMP_TO_EDGE,true);
	
	/*unsigned int prefilterMap;
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);*/

	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);


	Shader prefilterShader("./shader/cube_generator.vs", "./shader/envmap_spec_prefilter.fs");
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};
	prefilterShader.use();
	prefilterShader.setUniform("projection", captureProjection);
	prefilterShader.setUniform("environmentMap", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (int mip = 0; mip < maxMipLevels; mip++) {
		int mipWidth = 128 * pow(0.5,mip);
		int mipHeight = 128 * pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);
		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.setUniform("roughness", roughness);
		for (int i = 0; i < 6; i++) {
			prefilterShader.setUniform("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Geometry::renderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return prefilterMap;
}
unsigned int genIrradianceMap(unsigned int envCubemap) {
	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32,32);

	unsigned int irradianceMap=TextureLoader::genEmptyTextureCubeMap(32,32,GL_RGB,GL_FLOAT,GL_CLAMP_TO_EDGE,false);
	Shader irradianceShader("./shader/cube_generator.vs", "./shader/envmap_preshader.fs");
	/*glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
			GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
*/
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	irradianceShader.use();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP,envCubemap);
	irradianceShader.setUniform("environmentMap", 1);
	irradianceShader.setUniform("projection", captureProjection);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader.setUniform("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Geometry::renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return irradianceMap;
}
unsigned int genLUT() {
	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

	Shader LUTShader("./shader/shader.vs", "./shader/lut_preshader.fs");
	unsigned int brdfLUT=TextureLoader::genEmptyTexture2D(512,512,GL_RG,GL_FLOAT,GL_CLAMP_TO_EDGE,false);
	//glGenTextures(1, &brdfLUT);
	//glBindTexture(GL_TEXTURE_2D, brdfLUT);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);//只有r和g通道
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);

	glViewport(0, 0, 512, 512);
	LUTShader.use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Geometry::renderQuad();

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
	Shader backgroudShader("./shader/cubemap_shader.vs", "./shader/cubemap_shader.fs");
	//Shader backgroudShader("./shader/shader.vs", "./shader/shader.fs");
	//Model test_model(model_path);
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.8f, 300.8f, 300.8f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};

	// load textures
	// -------------
	string model_name = "bamboo-wood-semigloss";
	string albedo_path = "./model/" + model_name + "/" + model_name + "-albedo.png";
	string ao_path = "./model/" + model_name + "/" + model_name + "-ao.png";
	string metal_path = "./model/" + model_name + "/" + model_name + "-metal.png";
	string normal_path = "./model/" + model_name + "/" + model_name + "-normal.png";
	string roughness_path = "./model/" + model_name + "/" + model_name + "-roughness.png";
	texture_albedo = TextureLoader::loadTexture2D(albedo_path.c_str(),GL_UNSIGNED_BYTE,GL_CLAMP_TO_EDGE);
	//texture_albedo = loadTexture(albedo_path.c_str());
	texture_ao = TextureLoader::loadTexture2D(ao_path.c_str());
	texture_metal = TextureLoader::loadTexture2D(metal_path.c_str());
	texture_normal = TextureLoader::loadTexture2D(normal_path.c_str());
	texture_roughness = TextureLoader::loadTexture2D(roughness_path.c_str());
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, woodTexture);

	unsigned int envCubemap = genEnvmapFBO();

	unsigned int irradianceMap = genIrradianceMap(envCubemap);
	unsigned int prefilterMap = genPrefilterMap(envCubemap);
	unsigned int lutMap = genLUT();
	// shader configuration
	// --------------------

	// lighting info
	// -------------


	// render loop
	// -----------
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
		shader.setUniform("view", view);
		shader.setUniform("projection", projection);
		shader.setUniform("viewPos", window_manager->camera_->Position);
	//	shader.setUniform("texture_diffuse1", 0);
		if (pbr_env_on) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, lutMap);
			shader.setUniform("irradianceMap", 0);
			shader.setUniform("prefilterMap", 1);
			shader.setUniform("brdfLUT", 2);
		}
	
		for (int i = 0; i < 4; i++) {
			shader.setUniform("lightPositions[" + to_string(i) +"]", lightPositions[i]);
			shader.setUniform("lightColors[" + to_string(i) + "]", lightColors[i]);
		}
		renderScene(shader);

	/*	backgroudShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lutMap);
		backgroudShader.setUniform("screenTexture", 0);
		Geometry::renderQuad();*/
		backgroudShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
		backgroudShader.setUniform("environmentMap", 0);
		backgroudShader.setUniform("view", view);
		backgroudShader.setUniform("projection", projection);
		Geometry::renderCube();
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------

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
	//for (int row = 0; row < nrRows; ++row)
	//{
	//	shader.setUniform("metallic", (float)row / (float)nrRows);
	//	for (int col = 0; col < nrColumns; ++col)
	//	{
	//		shader.setUniform("texture_albedo",3);
	//		glActiveTexture(GL_TEXTURE3);
	//		glBindTexture(GL_TEXTURE_2D, texture_albedo);
	//		// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
	//		// on direct lighting.
	//		shader.setUniform("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

	//		model = glm::mat4(1.0f);
	//		model = glm::translate(model, glm::vec3(
	//			(col - (nrColumns / 2)) * spacing,
	//			(row - (nrRows / 2)) * spacing,
	//			0.0f
	//		));
	//		shader.setUniform("model", model);
	//		Geometry::renderSphere();
	//	}
	//}
	shader.setUniform("model", model);
	shader.setUniform("texture_albedo", 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture_albedo);
	shader.setUniform("texture_ao", 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texture_ao);
	shader.setUniform("texture_metallic", 5);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, texture_metal);
	shader.setUniform("texture_roughness", 6);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, texture_roughness);
	Geometry::renderSphere();
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

