#pragma once
#include "Material.h"
#include "stb_image_write.h"
#include <direct.h>
#include <io.h>
enum SKYBOX_PARAMETER{ENVCUBEMAP};
class SkyboxMaterial :
	public Material
{
public:
	unsigned int size = 1024;
	SkyboxMaterial();
	SkyboxMaterial(const string& root_path, const string& root_name, const string& appendix) {
		loadTextures(root_path, root_name, appendix);
	}
	SkyboxMaterial(const string& equirectangularPath) {
		int p = equirectangularPath.find_last_of('/');
		int dotp = equirectangularPath.find_last_of('.');
		_rootPath = equirectangularPath.substr(0, p);
		_rootName = equirectangularPath.substr(p + 1, dotp-p-1);
		string skyboxPath = _rootPath + "/skybox";
		if (_access(skyboxPath.c_str(), 0)==0) {
			loadTextures(skyboxPath, _rootName, "hdr");
			return;
		}
		unsigned int id=genEnvmap(equirectangularPath);
		Texture t = Texture(id, "envCubemap", GL_TEXTURE_CUBE_MAP);
		textureList.push_back(t);
		SaveEnvCubemap();
	}
	~SkyboxMaterial();
	virtual bool loadTextures(const string& root_path, const string& root_name, const string& appendix);
	string GetRootPath() { return _rootPath; }
	string GetRootName() { return _rootName; }
private:
	string _rootPath,_rootName;
	MVPTransform* GenMVP();
	unsigned int genEnvmap(const string& equirectangularPath);
	void SaveEnvCubemap();
	
	
};

