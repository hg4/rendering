#pragma once
#include <memory>
#include <glad\glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include "Mesh.h"
#include "Shader.h"
using namespace std;
using namespace glm;

struct Character {
	GLuint     TextureID;  // ���������ID
	glm::ivec2 Size;       // ���δ�С
	glm::ivec2 Bearing;    // �ӻ�׼�ߵ�������/������ƫ��ֵ
	GLuint     Advance;    // ԭ�����һ������ԭ��ľ���
};



class TextRenderer
{
public:
	static shared_ptr<TextRenderer> T;
	bool initialized = false;
	TextRenderer();
	~TextRenderer();
	void RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color);
	void Init();
private:
	std::map<GLchar, Character> _characters;
	unsigned int VAO, VBO;
	void FTLibPrepare();
	shared_ptr<Shader> _s;
	shared_ptr<Mesh> _renderQuad;
};


