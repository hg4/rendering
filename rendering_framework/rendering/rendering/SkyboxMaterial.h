#pragma once
#include "Material.h"
enum SKYBOX_PARAMETER{ENVCUBEMAP};
class SkyboxMaterial :
	public Material
{
public:
	SkyboxMaterial();
	SkyboxMaterial(const string& root_path, const string& root_name, const string& appendix) {
		loadTextures(root_path, root_name, appendix);
	}
	SkyboxMaterial(const string& equirectangularPath) {
		unsigned int id=genEnvmap(equirectangularPath);
		Texture t = Texture(id, "envCubemap", GL_TEXTURE_CUBE_MAP);
		textureList.push_back(t);
	}
	~SkyboxMaterial();
	virtual bool loadTextures(const string& root_path, const string& root_name, const string& appendix);
private:
	MVPTransform* GenMVP();
	unsigned int genEnvmap(const string& equirectangularPath);
};

