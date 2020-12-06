#include "PhongMaterial.h"
#include "TextureLoader.h"


PhongMaterial::PhongMaterial()
{
}


PhongMaterial::~PhongMaterial()
{
}

bool PhongMaterial::loadTextures(const string & root_path, const string & root_name, const string & appendix)
{
	vector<string> type{ "amibent" ,"diffuse","specular"};
	for (int i = 0; i < type.size(); i++) {
		string finalPath = root_path + "/" + root_name + "_" + type[i] + "." + appendix;
		textureList.push_back(TextureLoader::loadTexture2D(finalPath.c_str(), "texture_" + type[i], GL_UNSIGNED_BYTE, true, GL_CLAMP_TO_EDGE));
	}
	return true;
}