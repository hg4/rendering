#include "TextureLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void * TextureLoader::loadTextureData(const char * path, GLenum data_type, GLenum &data_format, int &width, int &height) {
	int  nrComponents;
	void *data = NULL;
	stbi_set_flip_vertically_on_load(true);
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
	GLenum data_type, bool useMipmap, GLenum tex_surrounding) {
	unsigned int id = loadTextureCubeMap(texture_faces_path, data_type, useMipmap, tex_surrounding);
	return Texture(id, texture_name, GL_TEXTURE_CUBE_MAP);
}
unsigned int TextureLoader::loadTextureCubeMap(std::vector<std::string> texture_faces_path,
	GLenum data_type , bool useMipmap , GLenum tex_surrounding ) {
	unsigned int textureID;
	GLenum	texture_format, data_format;
	int width, height;
	void * data = NULL;
	textureID = genTextures(GL_TEXTURE_CUBE_MAP);
	for (unsigned int i = 0; i < texture_faces_path.size(); i++) {
		data = loadTextureData(texture_faces_path[i].c_str(), data_type, data_format, width, height);
		texture_format = getTexFormat(data_format, data_type);
		textureID = genTextures(GL_TEXTURE_CUBE_MAP);
		bindTextures(width, height, texture_format, data_format, data, GL_TEXTURE_CUBE_MAP, data_type, tex_surrounding, useMipmap, i);
		stbi_image_free(data);
	}
	return textureID;
}