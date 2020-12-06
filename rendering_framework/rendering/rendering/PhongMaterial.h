#pragma once
#include "Material.h"

enum PHONG_PARAMETER { AMIBENT, DIFFUSE, SPECULAR };
class PhongMaterial :
	public Material
{
public:
	
	PhongMaterial();
	~PhongMaterial();
	virtual bool loadTextures(const string& root_path, const string& root_name, const string& appendix);

	Texture GetTexture(PHONG_PARAMETER p) const;
	void SetTexture(PHONG_PARAMETER p, const Texture &t);
	void RenameParameterInShader(PHONG_PARAMETER p, const string& name);
};

