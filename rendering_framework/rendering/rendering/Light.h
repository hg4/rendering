#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.h"
#include <memory>
#include "BufferManager.h"
#include "Common.h"
#include "Shader.h"
#include "Transform.h"

using namespace std;
using namespace glm;

enum class LIGHTTYPE {POINT,DIR,SPOT,AREA};
class Shader;
class Scene;
class Light
{
public:
	Light();
	virtual ~Light();
	vec3 position;
	vec3 color;
	vec3 direction;

	float intensity;
	float shadowBias;
	int numberId;
	LIGHTTYPE lightType;
	shared_ptr<Shader> shadowShader;
	Texture shadowMap;
	mat4 LIGHTSPACE_MATRIX_VP;
	uint shadowHeight, shadowWidth;
	virtual void AddLightInShader(shared_ptr<Shader> s,int index) = 0;
	virtual void UpdateShadowMap(shared_ptr<Scene> s);
	shared_ptr<Transform> GetTransform();
protected:
	uint _initializedFBO;
	uint FBO;
	vec3 _dir;
	shared_ptr<Transform> _transform;
	void Init();
};

