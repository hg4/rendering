#include "DirLight.h"
#include "Scene.h"


DirLight::DirLight()
{
	lightType = LIGHTTYPE::DIR;
	intensity = 1.0f;
	color = vec3(1.0f, 1.0f, 1.0f);
	_dir = vec3(0.0f, 0.0f, 1.0f);
	direction = _dir;
	shadowBias = 0.05;
}


DirLight::~DirLight()
{
}

DirLight::DirLight(vec3 position,vec3 dir, float intensity, vec3 color)
{
	lightType = LIGHTTYPE::DIR;
	_dir = dir;
	direction = _dir;
	this->position = position;
	this->intensity = intensity;
	this->color = color;
}

void DirLight::AddLightInShader(shared_ptr<Shader> s, int index)
{
	vec4 dir =_transform->GetModel() * vec4(_dir,0.0);
	direction = vec3(dir.x, dir.y, dir.z);
	s->setUniform("dirLight[" + to_string(index) + "].direction", direction);
	s->setUniform("dirLight[" + to_string(index) + "].position", position);
	s->setUniform("dirLight[" + to_string(index) + "].color", color);
	s->setUniform("dirLight[" + to_string(index) + "].intensity", intensity);
}

void DirLight::UpdateShadowMap(shared_ptr<Scene> s)
{

	if (!_initializedFBO) Init();
	glViewport(0, 0, shadowWidth, shadowHeight);


	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	shadowShader->use();
	
	mat4 lightView = lookAt(position, position + direction, vec3(0.0f, 1.0f, 0.0f));
	mat4 lightProjection = ortho(-15.0f, 15.0f, -15.0f, 15.0f,nearPlane,farPlane);
	LIGHTSPACE_MATRIX_VP = lightProjection * lightView;
	shadowShader->setUniform("LIGHTSPACE_MATRIX_VP", LIGHTSPACE_MATRIX_VP);
	
	s->RenderSceneShadowByGlobalShader(shadowShader);

	glBindFramebuffer(GL_FRAMEBUFFER,0);
}