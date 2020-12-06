#include "SkyboxMaterial.h"
#include "BufferManager.h"
#include "TextureLoader.h"
#include "Geometry.h"
#include "Mesh.h"

SkyboxMaterial::SkyboxMaterial()
{
}


SkyboxMaterial::~SkyboxMaterial()
{
}

bool SkyboxMaterial::loadTextures(const string & root_path, const string & root_name, const string & appendix)
{
	vector<string> type{ "top","bottom","front","back","left","right"};
	vector<string> facesPath;
	for (int i = 0; i < type.size(); i++) {
		string finalPath = root_path + "/" + root_name + "_" + type[i] + "." + appendix;
		facesPath.push_back(finalPath);
	}
	unsigned int id=TextureLoader::loadTextureCubeMap(facesPath, GL_UNSIGNED_BYTE, false);
	Texture t = Texture(id, "envCubemap", GL_TEXTURE_CUBE_MAP);
	textureList.push_back(t);
	return true;
}

unsigned int SkyboxMaterial::genEnvmap(const string& equirectangularPath) {
	MVPTransform* mvp = GenMVP();
	BufferElement * be = BufferManager::genBindFRBOBuffer(512, 512);
	Shader equirectangularToCubemapShader("./shader/cube_generator.vs", "./shader/cube_generator.fs");
	Texture hdr = TextureLoader::loadTexture2D(equirectangularPath.c_str(),
		"equirectangularMap",
		GL_FLOAT,
		false,
		GL_CLAMP_TO_EDGE
		);
	unsigned int envCubemap = TextureLoader::genEmptyTextureCubeMap(512, 512, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, false);
	// ----------------------------------------------------------------------------------------------
	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, be->FBO);
	Mesh cube = Geometry::createCube();
	cube.material->BindShader(equirectangularToCubemapShader);
	cube.addTexture(hdr.id, hdr.name_in_shader, GL_TEXTURE_2D);
	for (unsigned int i = 0; i < 6; ++i)
	{
		cube.setMVP(mvp[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		cube.draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return envCubemap;
}

MVPTransform * SkyboxMaterial::GenMVP()
{
		glm::mat4 model = glm::mat4(1.0);
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 views[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
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