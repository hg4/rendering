#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Shader
{
public:
	Shader();
	Shader(const char * vertex_shader_path,const char * fragment_shader_path);
	~Shader();
	void use();
	void setUniform(const std::string &name, float value) const;
	void setUniform(const std::string &name, int value) const;
	void setUniform(const std::string &name, bool value) const;
	void setUniform(const std::string &name, glm::vec3 value) const;
	void setUniform(const std::string &name, glm::vec4 value) const;
	void setUniform(const std::string &name, glm::mat4 &mat) const;
	unsigned int id;
	std::string vertex_shader;
	std::string fragment_shader;
};

