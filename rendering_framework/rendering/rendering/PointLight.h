#pragma once
#include "Light.h"
class PointLight :
	public Light
{
public:
	PointLight();
	PointLight(vec3 position, float intensity=1.0, vec3 color=vec3(1.0f,1.0f,1.0f),  vec3 attenuation = vec3(1.0f, 0.22f, 0.20f));
	~PointLight();
	virtual void AddLightInShader(shared_ptr<Shader> s,int index);
	vec3 attenuation;
};

