#include "PbrEnvMaterial.h"
#include "TextureLoader.h"
#include "Geometry.h"
#include "BufferManager.h"
#include "Mesh.h"

bool PbrEnvMaterial::loadTextures(const string & root_path, const string & root_name, const string & appendix)
{
	vector<string> type{ "right","left","top","bottom","front","back" };
	vector<string> facesPath;
	vector < vector<string>> faceMipmap;
	if (appendix == "flt") {
		for (int m = 0; m < _maxMipLevels; m++) {
			facesPath.clear();
			for (int i = 0; i < type.size(); i++) {
				string finalPath = "";
				finalPath = root_path + "/prefilter/"+to_string(m)+"/" + root_name + "_" + type[i] + "_" + appendix + ".hdr";
				facesPath.push_back(finalPath);
			}
			faceMipmap.push_back(facesPath);
		}
		Texture t = TextureLoader::loadTextureCubeMapMipmap(faceMipmap, "prefilterMap",GL_FLOAT,GL_CLAMP_TO_EDGE);
		textureList.push_back(t);
	}
	else if (appendix == "ird") {
		for (int i = 0; i < type.size(); i++) {
			string finalPath = "";
			finalPath = root_path + "/irradiance/" + root_name + "_" + type[i] + "_" + appendix + ".hdr";
			facesPath.push_back(finalPath);
		}
		Texture t = TextureLoader::loadTextureCubeMap(facesPath, "irradianceMap", GL_FLOAT,false,GL_CLAMP_TO_EDGE);
		textureList.push_back(t);
	}
	else if (appendix == "lut") {
		string finalPath = root_path + "/brdfLUT/" + root_name + "_" + appendix + ".hdr";
		Texture t = TextureLoader::loadTexture2D(finalPath.c_str(), "brdfLUT", GL_FLOAT, false, GL_CLAMP_TO_EDGE);
		textureList.push_back(t);
	}
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
	unsigned int prefilterMap = TextureLoader::genEmptyTextureCubeMap(prefilterSize, prefilterSize, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, true);
	unsigned int FBO = BufferManager::genBindFBOBuffer();
	unsigned int RBO;
	Shader prefilterShader("./shader/cube_generator.vs", "./shader/envmap_spec_prefilter.fs");
	Mesh cube = Geometry::createCube();
	cube.material->BindShader(prefilterShader);
	cube.addTexture(envCubemap, "environmentMap", GL_TEXTURE_CUBE_MAP);
	//unsigned int maxMipLevels = log2f((float)prefilterSize)-1;
	
	for (int mip = 0; mip < _maxMipLevels; mip++) {
		int mipWidth = prefilterSize * pow(0.5, mip);
		int mipHeight = prefilterSize * pow(0.5, mip);
		RBO = BufferManager::genBindRBOBuffer(mipHeight, mipWidth);
		glViewport(0, 0, mipWidth, mipHeight);
		float roughness = (float)mip / (float)(_maxMipLevels - 1);
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
	BufferElement* be = BufferManager::genBindFRBOBuffer(irradianceSize, irradianceSize);
	unsigned int irradianceMap = TextureLoader::genEmptyTextureCubeMap(irradianceSize, irradianceSize, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, false);
	Shader irradianceShader("./shader/cube_generator.vs", "./shader/envmap_preshader.fs");
	glViewport(0, 0, irradianceSize, irradianceSize); // don't forget to configure the viewport to the capture dimensions.
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
unsigned int PbrEnvMaterial::GenLUT() 
{
	BufferManager::genBindFRBOBuffer(lutSize, lutSize);
	Shader LUTShader("./shader/shader.vs", "./shader/lut_preshader.fs");
	unsigned int brdfLUT = TextureLoader::genEmptyTexture2D(lutSize, lutSize, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE, false);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUT, 0);
	glViewport(0, 0, lutSize, lutSize);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Mesh quad = Geometry::createQuad();
	quad.material->BindShader(LUTShader);
	quad.draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return brdfLUT;
}

void PbrEnvMaterial::SavePbrEnv() {
	uint mipSize = prefilterSize;
	float * temp;
	vector<string> type{ "right","left","top","bottom","front","back" };
	string rootPath=_skybox->GetRootPath();
	string rootName = _skybox->GetRootName();
	string command = rootPath + "/preprocess";
	_mkdir(command.c_str());
	command = rootPath + "/preprocess/prefilter";
	_mkdir(command.c_str());
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureList[PREFILTERMAP].id);
	for (int m = 0; m < _maxMipLevels; m++) {
		string tmp = command + "/" + to_string(m);
		_mkdir(tmp.c_str());
		mipSize = prefilterSize*pow(0.5, m);
		temp = new float[mipSize * mipSize * 3];
		for (int i = 0; i < 6; i++) {
			glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m, GL_RGB, GL_FLOAT, temp);
			string filename = tmp + "/" + rootName + "_" + type[i] + "_flt.hdr";
			if (stbi_write_hdr(filename.c_str(), mipSize, mipSize, 3, temp)) {
				std::cout << type[i] << "_flt save success." << std::endl;
			}
		}
		delete temp;
	}
	
	temp = new float[irradianceSize* irradianceSize * 3];
	command= rootPath + "/preprocess/irradiance";
	_mkdir(command.c_str());
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureList[IRRADIANCEMAP].id);
	for (int i = 0; i < 6; i++) {
		glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, GL_FLOAT, temp);
		string filename = command + "/" + rootName + "_" + type[i] + "_ird.hdr";
		if (stbi_write_hdr(filename.c_str(), irradianceSize, irradianceSize, 3, temp)) {
			std::cout << type[i] << "_ird save success." << std::endl;
		}
	}
	delete temp;
	temp = new float[lutSize*lutSize * 3];
	command = rootPath + "/preprocess/brdfLUT";
	_mkdir(command.c_str());
	glBindTexture(GL_TEXTURE_2D, textureList[BRDFLUT].id);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, temp);
	string filename = command + "/" + rootName + "_lut.hdr";
	stbi_flip_vertically_on_write(true);
	if (stbi_write_hdr(filename.c_str(), lutSize, lutSize, 3, temp)) {
		std::cout << "lut save success." << std::endl;
	}
}

PbrEnvMaterial::PbrEnvMaterial()
{
}


PbrEnvMaterial::~PbrEnvMaterial()
{
}
