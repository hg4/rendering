#pragma once
#include <glad/glad.h>
#include <string>
using namespace std;

class Texture {
public:
	Texture() {}
	Texture(unsigned int _id, string _name, GLenum  _type) :name_in_shader(_name), texture_type(_type), id(_id) {}
	Texture(unsigned int _id, string _path, string _name) :name_in_shader(_name), path(_path), id(_id) {}
	unsigned int id;
	string path;
	string name_in_shader;
	GLenum texture_type;
};