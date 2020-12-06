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
		Texture t=skybox->textureList[ENVCUBEMAP];
		MVPTransform * mvp = GenMVP();
		Texture prefilterMap = Texture(GenPrefilterMap(mvp, t.id), "prefilterMap", GL_TEXTURE_CUBE_MAP);
		Texture irradianceMap = Texture(GenIrradianceMap(mvp,t.id), "irradianceMap", GL_TEXTURE_CUBE_MAP);
		Texture LUT = Texture(GenLUT(), "brdfLUT", GL_TEXTURE_2D);
		textureList.push_back(irradianceMap);
		textureList.push_back(prefilterMap);
		textureList.push_back(LUT);
	}
	PbrEnvMaterial(const string& root_path, const string& root_name, const string& appendix) {
		loadTextures(root_path, root_name, appendix);
	}
	~PbrEnvMaterial();
	virtual bool loadTextures(const string& root_path, const string& root_name, const string& appendix);

private:
	MVPTransform* GenMVP();
	unsigned int PbrEnvMaterial::GenPrefilterMap(MVPTransform * mvp, unsigned int envCubemap);
	unsigned int PbrEnvMaterial::GenIrradianceMap(MVPTransform *mvp, unsigned int envCubemap);
	unsigned int PbrEnvMaterial::GenLUT();
};

