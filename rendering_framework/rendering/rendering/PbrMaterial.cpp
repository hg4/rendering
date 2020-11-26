#include "PbrMaterial.h"


bool PbrMaterial::loadTextures(const string& root_path, const string& root_name)
{
	vector<string> appendix{ "albedo" ,"ao","metallic","normal", "roughness" };
	for (int i = 0; i < appendix.size(); i++) {
		string finalPath = root_path + "/" + root_name + "-" + appendix[i]+".png";
		textureList.push_back(TextureLoader::loadTexture2D(finalPath.c_str(),"texture_"+appendix[i],GL_UNSIGNED_BYTE,true,GL_CLAMP_TO_EDGE));
	}
	return true;
}
