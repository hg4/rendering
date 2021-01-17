#pragma once
#include <iostream>
#include "MVPTransform.h"
#include "Light.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Light;
class Shader :
	public enable_shared_from_this<Shader>
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
	void setCameraPosition(glm::vec3 value, const std::string& name = "viewPos");
	void setMVPTransform(MVPTransform& mvp);
	void setLight(shared_ptr<Light> l,int index);
	unsigned int id;
	std::string vertex_shader;
	std::string fragment_shader;
};

