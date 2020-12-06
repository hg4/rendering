#include "PbrMaterial.h"


bool PbrMaterial::loadTextures(const string& root_path, const string& root_name, const string& appendix)
{
	vector<string> type{ "albedo" ,"ao","metallic","normal", "roughness" };
	for (int i = 0; i < type.size(); i++) {
		string finalPath = root_path + "/" + root_name + "_" + type[i]+"."+appendix;
		textureList.push_back(TextureLoader::loadTexture2D(finalPath.c_str(),"texture_"+type[i],GL_UNSIGNED_BYTE,true,GL_CLAMP_TO_EDGE));
	}
	return true;
}

Texture PbrMaterial::GetTexture(PBR_PARAMETER p) const
{
	return textureList[p];
}

void PbrMaterial::SetTexture(PBR_PARAMETER p, const Texture & t)
{
	textureList[p] = t;
}

void PbrMaterial::RenameParameterInShader(PBR_PARAMETER p, const string & name)
{
	textureList[p].name_in_shader = name;
}

//void PbrMaterial::BindShader(Shader &s)
//{
//	shader = s;
//	hasBindShader = true;
//	for (int i = 0; i < textureList.size(); i++)
//	{
//		glActiveTexture(GL_TEXTURE0 + i);
//		glBindTexture(textureList[i].texture_type, textureList[i].id);
//		s.setUniform(textureList[i].name_in_shader, i);
//	}
//}

