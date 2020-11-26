#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <iostream>
#include <vector>
#include "Texture.h"

class TextureLoader {
public:
	static Texture genEmptyTexture2D(int width, int height, string texture_name, GLenum data_format, GLenum data_type,
		GLenum tex_surrounding, bool useMipmap);
	static unsigned int genEmptyTexture2D(int width, int height, GLenum data_format, GLenum data_type,
		GLenum tex_surrounding = GL_CLAMP_TO_EDGE, bool useMipmap = false);
	static Texture genEmptyTextureCubeMap(int width, int height, string texture_name, GLenum data_format, GLenum data_type,
		GLenum tex_surrounding, bool useMipmap);
	static unsigned int genEmptyTextureCubeMap(int width, int height, GLenum data_format, GLenum data_type,
		GLenum tex_surrounding = GL_CLAMP_TO_EDGE, bool useMipmap = false);
	static Texture loadTexture2D(const char * path, std::string texture_name,
		GLenum data_type, bool useMipmap, GLenum tex_surrounding);
	static unsigned int loadTexture2D(const char * path,
		GLenum data_type = GL_UNSIGNED_BYTE, bool useMipmap = true, GLenum tex_surrounding = GL_REPEAT);
	static Texture loadTextureCubeMap(std::vector<std::string> texture_faces_path, std::string texture_name,
		GLenum data_type, bool useMipmap, GLenum tex_surrounding);
	static unsigned int loadTextureCubeMap(std::vector<std::string> texture_faces_path,
		GLenum data_type = GL_UNSIGNED_BYTE, bool useMipmap = true, GLenum tex_surrounding = GL_CLAMP_TO_EDGE);
	/*static Texture genEmptyTexture2D(int width, int height, string texture_name, GLenum data_format, GLenum data_type,
		GLenum tex_surrounding, bool useMipmap) {
		unsigned int id=genEmptyTexture2D(width, height, data_format, data_type, tex_surrounding, useMipmap);
		return Texture(id, texture_name, GL_TEXTURE_2D);
	}
	static unsigned int genEmptyTexture2D(int width, int height, GLenum data_format, GLenum data_type,
		GLenum tex_surrounding = GL_CLAMP_TO_EDGE, bool useMipmap = false) {
		unsigned int textureID = genTextures(GL_TEXTURE_2D);
		GLenum texture_format = getTexFormat(data_format, data_type);
		bindTextures(width, height, texture_format, data_format, nullptr, GL_TEXTURE_2D, data_type, tex_surrounding, useMipmap);
		return textureID;
	}
	static Texture genEmptyTextureCubeMap(int width, int height,string texture_name, GLenum data_format, GLenum data_type,
		GLenum tex_surrounding, bool useMipmap) {
		unsigned int id = genEmptyTextureCubeMap(width, height, data_format, data_type, tex_surrounding, useMipmap);
		return Texture(id, texture_name, GL_TEXTURE_CUBE_MAP);
	}
	static unsigned int genEmptyTextureCubeMap(int width,int height,GLenum data_format,GLenum data_type, 
		GLenum tex_surrounding = GL_CLAMP_TO_EDGE,bool useMipmap=false) {
		unsigned int textureID = genTextures(GL_TEXTURE_CUBE_MAP);
		GLenum texture_format = getTexFormat(data_format, data_type);
		for (unsigned int i = 0; i < 6; i++) {
			bindTextures(width, height, texture_format, data_format, nullptr, GL_TEXTURE_CUBE_MAP, data_type,tex_surrounding,useMipmap,i);
		}
		return textureID;
	}
	static Texture loadTexture2D(const char * path, std::string texture_name,
		GLenum data_type, bool useMipmap, GLenum tex_surrounding) {
		unsigned int id = loadTexture2D(path, data_type, useMipmap, tex_surrounding);
		return Texture(id, texture_name, GL_TEXTURE_2D);
	}
	static unsigned int loadTexture2D(const char * path,
		GLenum data_type = GL_UNSIGNED_BYTE, bool useMipmap=true,GLenum tex_surrounding = GL_REPEAT  ) {
		unsigned int textureID;
		GLenum	texture_format, data_format;
		int width, height;
		void * data=NULL;
		data=loadTextureData(path, data_type, data_format, width, height);
		texture_format = getTexFormat(data_format, data_type);
		textureID=genTextures(GL_TEXTURE_2D);
		bindTextures(width, height, texture_format, data_format, data, GL_TEXTURE_2D, data_type, useMipmap,tex_surrounding);
		stbi_image_free(data);
		return textureID;
	}
	static Texture loadTextureCubeMap(std::vector<std::string> texture_faces_path, std::string texture_name,
		GLenum data_type, bool useMipmap, GLenum tex_surrounding) {
		unsigned int id = loadTextureCubeMap(texture_faces_path, data_type, useMipmap, tex_surrounding);
		return Texture(id, texture_name, GL_TEXTURE_CUBE_MAP);
	}
	static unsigned int loadTextureCubeMap(std::vector<std::string> texture_faces_path, 
		GLenum data_type = GL_UNSIGNED_BYTE, bool useMipmap = true, GLenum tex_surrounding = GL_CLAMP_TO_EDGE) {
		unsigned int textureID;
		GLenum	texture_format, data_format;
		int width, height;
		void * data = NULL;
		textureID = genTextures(GL_TEXTURE_CUBE_MAP);
		for (unsigned int i = 0; i < texture_faces_path.size(); i++) {
			data = loadTextureData(texture_faces_path[i].c_str(), data_type,data_format,width,height);
			texture_format = getTexFormat(data_format, data_type);
			textureID = genTextures(GL_TEXTURE_CUBE_MAP);
			bindTextures(width, height, texture_format, data_format, data, GL_TEXTURE_CUBE_MAP, data_type, tex_surrounding,useMipmap,i);
			stbi_image_free(data);
		}
		return textureID;
	}*/
private:
	static void * loadTextureData(const char * path, GLenum data_type, GLenum &data_format, int &width, int &height);
	//static void * loadTextureData(const char * path, GLenum data_type, GLenum &data_format, int &width, int &height) {
	//	int  nrComponents;
	//	void *data = NULL;
	//	stbi_set_flip_vertically_on_load(true);
	//	if (data_type == GL_UNSIGNED_BYTE) {
	//		data = stbi_load(path, &width, &height, &nrComponents, 0);
	//	}
	//	else if (data_type == GL_FLOAT) {
	//		data=stbi_loadf(path, &width, &height, &nrComponents, 0);
	//	}
	//	if (data)
	//	{
	//		if (nrComponents == 1)
	//			data_format = GL_RED;
	//		else if (nrComponents == 3)
	//			data_format = GL_RGB;
	//		else if (nrComponents == 4)
	//			data_format = GL_RGBA;
	//	}
	//	else {
	//		std::cout << "Texture failed to load at path: " << path << std::endl;
	//		stbi_image_free(data);
	//	}
	//	return data;
	//}
	static GLenum getTexFormat(GLenum data_format,GLenum data_type) {
		if (data_format == GL_RGB&&data_type == GL_UNSIGNED_BYTE) return GL_RGB;
		if (data_format == GL_RGB&&data_type == GL_FLOAT) return GL_RGB16F;
		if (data_format == GL_RG&&data_type == GL_FLOAT) return GL_RG16F;
		if (data_format == GL_DEPTH_COMPONENT) return GL_DEPTH_COMPONENT;
		return GL_RGB;
	}
	static unsigned int genTextures(GLenum tex_type) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(tex_type, textureID);
		return textureID;
	}
	static void bindTextures(int width, int height, GLenum format_texture, GLenum format_data, const void* data,
		GLenum texture_type,GLenum data_type, GLenum texture_surrounding,bool useMipmap,int index=0) {
		if (texture_type == GL_TEXTURE_2D) {
			glTexImage2D(GL_TEXTURE_2D, 0, format_texture, width, height, 0, format_data, data_type, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_surrounding);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_surrounding);
			if (useMipmap) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
		}
		else if (texture_type == GL_TEXTURE_CUBE_MAP) {
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + index,
				0, format_texture, width, height, 0, format_data, data_type, data
			);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, texture_surrounding);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, texture_surrounding);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, texture_surrounding);
			if (useMipmap) {
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			}
			else {
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
		}
		
	}
	/*static unsigned int genBindTextures2D(int width, int height, GLenum format_texture, GLenum format_data, const void* data,
		GLenum type = GL_UNSIGNED_BYTE,GLenum texture_surrounding = GL_REPEAT) {
		unsigned int textureID=genTextures(GL_TEXTURE_2D);
		bindTextures(width, height, format_texture, format_data, data,GL_TEXTURE_2D,type, texture_surrounding);
		return textureID;
	}*/
};