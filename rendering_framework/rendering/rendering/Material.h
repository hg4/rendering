#pragma once
#include "Texture.h"
#include "Shader.h"
#include <vector>
#include <memory>
using namespace std;

class Material {
public:
	vector<Texture> textureList;
	shared_ptr<Shader> shader;
	bool hasBindShader=false;
	vec3 basicColor;
	/*shared_ptr<Shader> shader;
	bool useDefaultShader;*/
	Material() 
	{
		/*shader = shared_ptr<Shader>(new Shader("./shader/standard_shader.vs", "./shader/standard_shader.fs"));
		useDefaultShader = true;*/
		basicColor = vec3(0.5f, 0.5f, 0.5f);
	}
	Material(Shader &s)
	{
		BindShader(s);
	}
	void BindShader(Shader &s);
	void ActiveMaterialInShader();
	virtual bool loadTextures(const string& root_path, const string& root_name,const string& appendix){
		return false;
	}
	virtual ~Material(){}


};