#include "Material.h"

void Material::BindShader(shared_ptr<Shader> s)
{
	shader = s;
	hasBindShader = true;
}

void Material::ActiveMaterialInShader()
{
	if (hasBindShader) 
	{
		shader->use();
		shader->setUniform("color", basicColor);
		for (int i = 0; i < textureList.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(textureList[i].texture_type, textureList[i].id);
			shader->setUniform(textureList[i].name_in_shader, i);
		}
	}
	else printf("please bind shader first.\n");
}
