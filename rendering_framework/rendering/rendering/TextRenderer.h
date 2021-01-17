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
	GLuint     TextureID;  // 字形纹理的ID
	glm::ivec2 Size;       // 字形大小
	glm::ivec2 Bearing;    // 从基准线到字形左部/顶部的偏移值
	GLuint     Advance;    // 原点距下一个字形原点的距离
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


