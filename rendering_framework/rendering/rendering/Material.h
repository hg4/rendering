#pragma once
#include "Texture.h"
#include <vector>
using namespace std;

class Material {
public:
	vector<Texture> textureList;
	virtual bool loadTextures(const string& root_path, const string& root_name) const {
		return false;
	}
	virtual ~Material(){}
};