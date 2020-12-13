#pragma once
#include "Material.h"
#include "SkyboxMaterial.h"
enum PBRENV_PARAMETER{IRRADIANCEMAP,PREFILTERMAP,BRDFLUT};
class PbrEnvMaterial :
	public Material
{
public:
	PbrEnvMaterial();
	PbrEnvMaterial(SkyboxMaterial * skybox) {
		_skybox = skybox;
		string rootPath = _skybox->GetRootPath();
		string rootName = _skybox->GetRootName();
		string preproPath = rootPath + "/preprocess";
		if (_access(preproPath.c_str(), 0)==0) {
			loadTextures(preproPath, rootName, "ird");
			loadTextures(preproPath, rootName, "flt");
			loadTextures(preproPath, rootName, "lut");
			return;
		}

		Texture t=skybox->textureList[ENVCUBEMAP];
		MVPTransform * mvp = GenMVP();
		Texture prefilterMap = Texture(GenPrefilterMap(mvp, t.id), "prefilterMap", GL_TEXTURE_CUBE_MAP);
		Texture irradianceMap = Texture(GenIrradianceMap(mvp,t.id), "irradianceMap", GL_TEXTURE_CUBE_MAP);
		
		Texture LUT = Texture(GenLUT(), "brdfLUT", GL_TEXTURE_2D);
	
		textureList.push_back(irradianceMap);
		textureList.push_back(prefilterMap);
		//loadTextures(preproPath, rootName, "flt");
		textureList.push_back(LUT);
		//loadTextures(preproPath, rootName, "lut");
		SavePbrEnv();
	}
	PbrEnvMaterial(const string& root_path, const string& root_name, vector<string>& appendix) {
		for (int i = 0; i < appendix.size();i++)
			loadTextures(root_path, root_name, appendix[i]);
	}
	~PbrEnvMaterial();
	virtual bool loadTextures(const string& root_path, const string& root_name, const string& appendix);
	unsigned int prefilterSize = 128;
	unsigned int irradianceSize = 32;
	unsigned int lutSize = 512;
private:
	MVPTransform* GenMVP();
	unsigned int PbrEnvMaterial::GenPrefilterMap(MVPTransform * mvp, unsigned int envCubemap);
	unsigned int PbrEnvMaterial::GenIrradianceMap(MVPTransform *mvp, unsigned int envCubemap);
	unsigned int PbrEnvMaterial::GenLUT();
	void SavePbrEnv();
	SkyboxMaterial * _skybox;
	uint _maxMipLevels = 5;
};

