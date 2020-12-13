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
		GLenum data_type, bool useMipmap, GLenum tex_surrounding, int mipmapLevel=0);
	static unsigned int loadTextureCubeMap(std::vector<std::string> texture_faces_path,
		GLenum data_type = GL_UNSIGNED_BYTE, bool useMipmap = true, GLenum tex_surrounding = GL_CLAMP_TO_EDGE, int mipmapLevel=0);
	static Texture TextureLoader::loadTextureCubeMapMipmap(vector<std::vector<std::string>> texture_faces_path, std::string texture_name,
		GLenum data_type, GLenum tex_surrounding);
private:
	static void * loadTextureData(const char * path, GLenum data_type, GLenum &data_format, int &width, int &height,bool isflip=true);
	
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
		GLenum texture_type,GLenum data_type, GLenum texture_surrounding,bool useMipmap,int index=0,int mipmapLevel=0) {
		if (texture_type == GL_TEXTURE_2D) {
			glTexImage2D(GL_TEXTURE_2D, mipmapLevel, format_texture, width, height, 0, format_data, data_type, data);
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
				mipmapLevel, format_texture, width, height, 0, format_data, data_type, data
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
	static void ParameterSetting(GLenum texture_type, GLenum texture_surrounding, bool useMipmap) {
		if (texture_type == GL_TEXTURE_2D) {
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