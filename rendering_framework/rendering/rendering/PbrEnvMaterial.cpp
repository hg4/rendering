#include "PbrEnvMaterial.h"
#include "TextureLoader.h"
#include "Geometry.h"
#include "BufferManager.h"
#include "Mesh.h"

bool PbrEnvMaterial::loadTextures(const string & root_path, const string & root_name, const string & appendix)
{
	//todo after save the cubemap
	return true;
}

MVPTransform* PbrEnvMaterial::GenMVP() {
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

//todo: material and pbr preprocess
unsigned int PbrEnvMaterial::GenPrefilterMap(MVPTransform * mvp, unsigned int envCubemap) {
	unsigned int prefilterMap = TextureLoader::genEmptyTextureCubeMap(128, 128, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, true);
	unsigned int FBO = BufferManager::genBindFBOBuffer();
	unsigned int RBO;
	Shader prefilterShader("./shader/cube_generator.vs", "./shader/envmap_spec_prefilter.fs");
	prefilterShader.use();
	Mesh cube = Geometry::createCube();
	cube.material->BindShader(prefilterShader);
	cube.addTexture(envCubemap, "environmentMap", GL_TEXTURE_CUBE_MAP);
	unsigned int maxMipLevels = 5;
	for (int mip = 0; mip < maxMipLevels; mip++) {
		int mipWidth = 128 * pow(0.5, mip);
		int mipHeight = 128 * pow(0.5, mip);
		RBO = BufferManager::genBindRBOBuffer(mipHeight, mipWidth);
		glViewport(0, 0, mipWidth, mipHeight);
		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.setUniform("roughness", roughness);
		for (int i = 0; i < 6; i++) {
			cube.setMVP(mvp[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cube.draw();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return prefilterMap;
}
unsigned int PbrEnvMaterial::GenIrradianceMap(MVPTransform *mvp, unsigned int envCubemap) {
	BufferElement* be = BufferManager::genBindFRBOBuffer(32, 32);
	unsigned int irradianceMap = TextureLoader::genEmptyTextureCubeMap(32, 32, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, false);
	Shader irradianceShader("./shader/cube_generator.vs", "./shader/envmap_preshader.fs");
	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	Mesh cube = Geometry::createCube();
	cube.material->BindShader(irradianceShader);
	cube.addTexture(envCubemap, "environmentMap", GL_TEXTURE_CUBE_MAP);
	for (unsigned int i = 0; i < 6; ++i)
	{
		cube.setMVP(mvp[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cube.draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return irradianceMap;
}
unsigned int PbrEnvMaterial::GenLUT() {

	BufferManager::genBindFRBOBuffer(512, 512);
	Shader LUTShader("./shader/shader.vs", "./shader/lut_preshader.fs");
	unsigned int brdfLUT = TextureLoader::genEmptyTexture2D(512, 512, GL_RG, GL_FLOAT, GL_CLAMP_TO_EDGE, false);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);
	glViewport(0, 0, 512, 512);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Mesh quad = Geometry::createQuad();
	quad.material->BindShader(LUTShader);
	quad.draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return brdfLUT;
}

PbrEnvMaterial::PbrEnvMaterial()
{
}


PbrEnvMaterial::~PbrEnvMaterial()
{
}
