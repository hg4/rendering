#pragma once
#include "Light.h"

class DirLight :
	public Light
{
public:
	DirLight();
	DirLight(vec3 position,vec3 dir,float intensity=1.0f,vec3 color=vec3(1.0f,1.0f,1.0f));
	~DirLight();
	
	virtual void AddLightInShader(shared_ptr<Shader> s, int index);
	virtual void UpdateShadowMap(shared_ptr<Scene> s);
};

