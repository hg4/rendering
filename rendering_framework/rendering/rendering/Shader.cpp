#include "Shader.h"
#include <glad\glad.h>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shader::Shader()
{
}

Shader::Shader(const char * vertex_shader_path, const char * fragment_shader_path)
{
	//read shader
	std::stringstream vertex_stream, fragment_stream;
	std::ifstream fin;
	fin.open(vertex_shader_path);
	vertex_stream << fin.rdbuf();
	fin.close();
	fin.open(fragment_shader_path);
	fragment_stream << fin.rdbuf();
	fin.close();
	vertex_shader = vertex_stream.str();
	fragment_shader = fragment_stream.str();

	//complie shader
	int success;
	char infoLog[512];
	const char * vertex_shader_c = vertex_shader.c_str();
	const char * fragment_shader_c = fragment_shader.c_str();
	unsigned int vertex=glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_shader_c,NULL );
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout<<vertex_shader_path << ":ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_shader_c, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << fragment_shader_path << ":ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//link shader program
	id=glCreateProgram();
	glAttachShader(id,vertex);
	glAttachShader(id,fragment);
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::LINK::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
	
}


Shader::~Shader()
{
}

void Shader::use()
{
	glUseProgram(id);
}

void Shader::setUniform(const std::string & name, float value) const
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setUniform(const std::string & name, int value) const
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setUniform(const std::string & name, bool value) const
{
	glUniform1ui(glGetUniformLocation(id, name.c_str()),(unsigned int)value);
}

void Shader::setUniform(const std::string & name, glm::vec3 value) const
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), value[0], value[1], value[2]);
}

void Shader::setUniform(const std::string & name, glm::vec4 value) const
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), value[0], value[1], value[2], value[3]);
}

void Shader::setUniform(const std::string & name, glm::mat4 &mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setCameraPosition(glm::vec3 value, const std::string & name)
{
	use();
	setUniform(name, value);
}

void Shader::setMVPTransform(MVPTransform & mvp)
{
	use();
	setUniform("model", mvp.model);
	setUniform("view", mvp.view);
	setUniform("projection", mvp.projection);
	setUniform("transpose_inverse_model", mvp.transpose_inverse_model);
}

void Shader::setLight(Light & l)
{
	use();

}