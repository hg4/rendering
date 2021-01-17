#include "PointLight.h"



PointLight::PointLight()
{
	lightType = LIGHTTYPE::POINT;
	color = vec3(1.0f, 1.0f, 1.0f);
	attenuation = vec3(1.0f, 0.22f, 0.20f);
	position = vec3(0.0f, 0.0f, 0.0f);
}

PointLight::PointLight(vec3 position, float intensity, vec3 color,  vec3 attenuation)
{
	this->intensity = intensity;
	this->position = position;
	this->color = color;
	this->attenuation = attenuation;
	lightType = LIGHTTYPE::POINT;
}

PointLight::~PointLight()
{
}

void PointLight::AddLightInShader(shared_ptr<Shader> s,int index)
{
	s->setUniform("pointLight[" + to_string(index) + "].position", position);
	s->setUniform("pointLight[" + to_string(index) + "].color", color);
	s->setUniform("pointLight[" + to_string(index) + "].attenuation", attenuation);
	s->setUniform("pointLight[" + to_string(index) + "].intensity", intensity);
}

