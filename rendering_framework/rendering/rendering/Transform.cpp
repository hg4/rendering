#include "Transform.h"



Transform::Transform()
{
	_model = mat4(1.0);
}


Transform::~Transform()
{
}

void Transform::SetLocalScale(vec3 s)
{
	if (s.x == _localScaleX&&s.y == _localScaleY&&s.z == _localScaleZ) return;
	_localScaleX = s.x;
	_localScaleY = s.y;
	_localScaleZ = s.z;
	_localScale = glm::transpose(glm::scale(mat4(1.0f), s));
	UpdateModelMatrix();
}

void Transform::SetLocalRotate(vec3 r)
{

	if (r.x == _localRotateX&&r.y == _localRotateY&&r.z == _localRotateZ) return;
	_localRotateX = r.x;
	_localRotateY = r.y;
	_localRotateZ = r.z;
	mat4 rotateX = glm::transpose(glm::rotate(mat4(1.0f), glm::radians(r.x), vec3(1.0f, 0.0f, 0.0f)));
	mat4 rotateY = glm::transpose(glm::rotate(mat4(1.0f), glm::radians(r.y), vec3(0.0f, 1.0f, 0.0f)));
	mat4 rotateZ = glm::transpose(glm::rotate(mat4(1.0f), glm::radians(r.z), vec3(0.0f, 0.0f, 1.0f)));
	_localRotate = rotateY*rotateX*rotateZ;
	UpdateModelMatrix();
}

void Transform::SetTranslate(vec3 t)
{
	if (_position == t) return;
	_position = t;
	_globalTranslate = glm::transpose(glm::translate(mat4(1.0f), t));
	UpdateModelMatrix();
}

void Transform::UpdateModelMatrix()
{
	_model = glm::transpose(_localScale*_localRotate*_globalTranslate);
}
mat4 Transform::GetModel()
{
	return _model;
}