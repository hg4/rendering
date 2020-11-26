#pragma once
#include "Texture.h"
#include "Material.h"
#include "TextureLoader.h"
#include <vector>
using namespace std;

class PbrMaterial:public Material{
public:
	virtual bool loadTextures(const string& root_path,const string& root_name);
	PbrMaterial(){}
	PbrMaterial(const char * root_path, const char * root_name)
	{
		bool flag = loadTextures(root_path, root_name);
	}
};