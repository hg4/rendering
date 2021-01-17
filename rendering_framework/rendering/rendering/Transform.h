#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
using namespace glm;

class Transform
{
public:
	Transform();
	~Transform();
	void SetLocalScale(vec3 s);
	void SetLocalRotate(vec3 r);
	void SetTranslate(vec3 t);
	vec3 GetEulerAngle();
	mat4 GetModel();
private:
	mat4 _model;
	vec3 _position;
	float _localScaleX, _localScaleY, _localScaleZ;
	float _localRotateX, _localRotateY, _localRotateZ;
	//vector<mat4> _basicTransform;
	/*void Rotate(float angle, vec3 axis);
	void Translate(vec3 offset);
	void Scale(vec3 s);*/
	void Transform::UpdateModelMatrix();
	mat4 _localRotate;
	mat4 _localScale;
	mat4 _globalTranslate;
	//mat4 _fixedModel;
};

