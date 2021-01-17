#include "TextureLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void * TextureLoader::loadTextureData(const char * path, GLenum data_type, GLenum &data_format, int &width, int &height, bool isflip) {
	int  nrComponents;
	void *data = NULL;
	stbi_set_flip_vertically_on_load(isflip);
	if (data_type == GL_UNSIGNED_BYTE) {
		data = stbi_load(path, &width, &height, &nrComponents, 0);
	}
	else if (data_type == GL_FLOAT) {
		data = stbi_loadf(path, &width, &height, &nrComponents, 0);
	}
	if (data)
	{
		if (nrComponents == 1)
			data_format = GL_RED;
		else if (nrComponents == 2)
			data_format = GL_RG;
		else if (nrComponents == 3)
			data_format = GL_RGB;
		else if (nrComponents == 4)
			data_format = GL_RGBA;
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return data;
}

Texture TextureLoader::genEmptyTexture2D(int width, int height, string texture_name, GLenum data_format, GLenum data_type,
	GLenum tex_surrounding, bool useMipmap) {
	unsigned int id = genEmptyTexture2D(width, height, data_format, data_type, tex_surrounding, useMipmap);
	return Texture(id, texture_name, GL_TEXTURE_2D);
}
unsigned int TextureLoader::genEmptyTexture2D(int width, int height, GLenum data_format, GLenum data_type,
	GLenum tex_surrounding , bool useMipmap ) {
	unsigned int textureID = genTextures(GL_TEXTURE_2D);
	GLenum texture_format = getTexFormat(data_format, data_type);
	bindTextures(width, height, texture_format, data_format, nullptr, GL_TEXTURE_2D, data_type, tex_surrounding, useMipmap);
	return textureID;
}
Texture TextureLoader::genEmptyTextureCubeMap(int width, int height, string texture_name, GLenum data_format, GLenum data_type,
	GLenum tex_surrounding, bool useMipmap) {
	unsigned int id = genEmptyTextureCubeMap(width, height, data_format, data_type, tex_surrounding, useMipmap);
	return Texture(id, texture_name, GL_TEXTURE_CUBE_MAP);
}
unsigned int TextureLoader::genEmptyTextureCubeMap(int width, int height, GLenum data_format, GLenum data_type,
	GLenum tex_surrounding , bool useMipmap ) {
	unsigned int textureID = genTextures(GL_TEXTURE_CUBE_MAP);
	GLenum texture_format = getTexFormat(data_format, data_type);
	for (unsigned int i = 0; i < 6; i++) {
		bindTextures(width, height, texture_format, data_format, nullptr, GL_TEXTURE_CUBE_MAP, data_type, tex_surrounding, useMipmap, i);
	}
	return textureID;
}
Texture TextureLoader::loadTexture2D(const char * path, std::string texture_name,
	GLenum data_type, bool useMipmap, GLenum tex_surrounding) {
	unsigned int id = loadTexture2D(path, data_type, useMipmap, tex_surrounding);
	return Texture(id, texture_name, GL_TEXTURE_2D);
}
unsigned int TextureLoader::loadTexture2D(const char * path,
	GLenum data_type , bool useMipmap , GLenum tex_surrounding ) {
	unsigned int textureID;
	GLenum	texture_format, data_format;
	int width, height;
	void * data = NULL;
	data = loadTextureData(path, data_type, data_format, width, height);
	texture_format = getTexFormat(data_format, data_type);
	textureID = genTextures(GL_TEXTURE_2D);
	bindTextures(width, height, texture_format, data_format, data, GL_TEXTURE_2D, data_type, useMipmap, tex_surrounding);
	stbi_image_free(data);
	return textureID;
}
Texture TextureLoader::loadTextureCubeMap(std::vector<std::string> texture_faces_path, std::string texture_name,
	GLenum data_type, bool useMipmap, GLenum tex_surrounding,int mipmapLevel) {
	unsigned int id = loadTextureCubeMap(texture_faces_path, data_type, useMipmap, tex_surrounding,mipmapLevel);
	return Texture(id, texture_name, GL_TEXTURE_CUBE_MAP);
}
unsigned int TextureLoader::loadTextureCubeMap(std::vector<std::string> texture_faces_path,
	GLenum data_type , bool useMipmap , GLenum tex_surrounding, int mipmapLevel) {
	unsigned int textureID;
	GLenum	texture_format, data_format;
	int width, height;
	void * data = NULL;
	textureID = genTextures(GL_TEXTURE_CUBE_MAP);
	for (unsigned int i = 0; i < texture_faces_path.size(); i++) {
		data = loadTextureData(texture_faces_path[i].c_str(), data_type, data_format, width, height,false);
		texture_format = getTexFormat(data_format, data_type);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipmapLevel, texture_format, width, height, 0, data_format, data_type, data);
		//bindTextures(width, height, texture_format, data_format, data, GL_TEXTURE_CUBE_MAP, data_type, tex_surrounding, useMipmap, i,mipmapLevel);
		stbi_image_free(data);
	}
	ParameterSetting(GL_TEXTURE_CUBE_MAP, GL_CLAMP_TO_EDGE,useMipmap);
	return textureID;
}

Texture TextureLoader::loadTextureCubeMapMipmap(vector<std::vector<std::string>> texture_faces_path, std::string texture_name,
	GLenum data_type,GLenum tex_surrounding) {
	unsigned int textureID;
	GLenum	texture_format, data_format;
	int width, height;
	void * data = NULL;
	textureID = genTextures(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, texture_faces_path.size()-1);
	for (unsigned int i = 0; i < texture_faces_path.size(); i++) {
		for (unsigned int j = 0; j < texture_faces_path[i].size(); j++) {
			data = loadTextureData(texture_faces_path[i][j].c_str(), data_type, data_format, width, height, false);
			texture_format = getTexFormat(data_format, data_type);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j , i, texture_format, width, height, 0, data_format, data_type, data);
			//bindTextures(width, height, texture_format, data_format, data, GL_TEXTURE_CUBE_MAP, data_type, tex_surrounding, useMipmap, i,mipmapLevel);
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, tex_surrounding);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, tex_surrounding);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, tex_surrounding);
	return Texture(textureID,texture_name,GL_TEXTURE_CUBE_MAP);
}

void TextureLoader::bindTextures(int width, int height, GLenum format_texture, GLenum format_data, const void* data,
	GLenum texture_type, GLenum data_type, GLenum texture_surrounding, bool useMipmap, int index, int mipmapLevel) {
	float BorderColor[] = { 1.0f,1.0f,1.0f,1.0f };
	if (texture_type == GL_TEXTURE_2D) {
		glTexImage2D(GL_TEXTURE_2D, mipmapLevel, format_texture, width, height, 0, format_data, data_type, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture_surrounding);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture_surrounding);
		if (texture_surrounding == GL_CLAMP_TO_BORDER) {
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);
		}
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
		if (texture_surrounding == GL_CLAMP_TO_BORDER) {
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);
		}
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