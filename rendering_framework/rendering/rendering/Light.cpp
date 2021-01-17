#include "Light.h"
#include "TextureLoader.h"


Light::Light()
{
	intensity = 1.0f;
	shadowHeight = SCR_HEIGHT*2;
	shadowWidth = SCR_WIDTH*2;
	_initializedFBO = false;
	shadowBias = 0.05;
	_transform= shared_ptr<Transform>(new Transform());
}


Light::~Light()
{
}

void Light::UpdateShadowMap(shared_ptr<Scene> s)
{
}

shared_ptr<Transform> Light::GetTransform()
{
	return _transform;
}

void Light::Init()
{
	shadowShader = shared_ptr<Shader>(new Shader("./shader/shadow_mapping_depth.vs", "./shader/shadow_mapping_depth.fs"));
	FBO = BufferManager::genBindFBOBuffer();
	shadowMap = TextureLoader::genEmptyTexture2D(shadowWidth, shadowHeight, "shadowMap", 
		GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_BORDER,false);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap.id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	_initializedFBO = true;

}