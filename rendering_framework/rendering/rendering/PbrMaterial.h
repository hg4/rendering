#pragma once
#include "Texture.h"
#include "Material.h"
#include "TextureLoader.h"
#include "Shader.h"
#include <vector>
using namespace std;


enum PBR_PARAMETER{ALBEDO,AO,METALLIC,NORMAL,ROUGHNESS};
class PbrMaterial:public Material{
public:
	virtual bool loadTextures(const string& root_path, const string& root_name, const string& appendix);
	PbrMaterial(){}
	PbrMaterial(const char * root_path, const char * root_name,const char * appendix):Material()
	{
		bool flag = loadTextures(root_path, root_name,appendix);
	}
	//virtual void BindShader(Shader &s);
	Texture GetTexture(PBR_PARAMETER p) const;
	void SetTexture(PBR_PARAMETER p,const Texture &t);
	void RenameParameterInShader(PBR_PARAMETER p,const string& name);


};