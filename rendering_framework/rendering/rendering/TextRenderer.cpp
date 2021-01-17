#include "TextRenderer.h"
#include <ft2build.h>
#include "Geometry.h"
#include "Common.h"
#include "MVPTransform.h"
#include FT_FREETYPE_H  



shared_ptr<TextRenderer> TextRenderer::T = shared_ptr<TextRenderer>(new TextRenderer());

TextRenderer::TextRenderer()
{
	
}


TextRenderer::~TextRenderer()
{
}

void TextRenderer::Init()
{
	initialized = true;
	FTLibPrepare();
	_s=shared_ptr<Shader>(new Shader("./shader/text_shader.vs", "./shader/text_shader.fs"));
	BufferManager::DynamicDraw(true);
	_renderQuad = Geometry::createQuad();
	BufferManager::DynamicDraw(false);
	_renderQuad->setMVP(MVPTransform(mat4(1.0), mat4(1.0), glm::ortho(0.0f,(float) SCR_WIDTH, 0.0f, (float)SCR_HEIGHT)));
	_renderQuad->material->BindShader(_s);
}

void TextRenderer::FTLibPrepare()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "./fonts/Consolas.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	FT_Set_Pixel_Sizes(face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //�����ֽڶ�������
	for (GLubyte c = 0; c < 128; c++)
	{
		// �����ַ������� 
		if (FT_Load_Char(face, c , FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// ��������
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
			);
		// ��������ѡ��
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// �����ַ���֮��ʹ��
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		_characters.insert(std::pair<GLchar, Character>(c, character));
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

}

void TextRenderer::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, vec3 color)
{
	_s->use();
	_s->setUniform("textColor", color);
	//glActiveTexture(GL_TEXTURE0);
	shared_ptr<BufferElement> be = _renderQuad->GetBufferElement();
	glBindVertexArray(be->VAO);

	// �����ı������е��ַ�
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		_renderQuad->ClearTextures();
		Character ch = _characters[*c];
		_renderQuad->addTexture(ch.TextureID, "character", GL_TEXTURE_2D);
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// ��ÿ���ַ�����VBO
			GLfloat vertices[6][8] = {
				{ xpos,     ypos + h, 0.0, 0.0f,0.0f,1.0f,  0.0, 0.0 },
				{ xpos,     ypos,   0.0, 0.0f,0.0f,1.0f,    0.0, 1.0 },
				{ xpos + w, ypos,    0.0, 0.0f,0.0f,1.0f,   1.0, 1.0 },

				{ xpos,     ypos + h, 0.0, 0.0f,0.0f,1.0f,  0.0, 0.0 },
				{ xpos + w, ypos,    0.0, 0.0f,0.0f,1.0f,   1.0, 1.0 },
				{ xpos + w, ypos + h, 0.0, 0.0f,0.0f,1.0f,  1.0, 0.0 }
			};
		// ���ı����ϻ�����������
		//glBindTexture(GL_TEXTURE_2D, ch.textureID);
		// ����VBO�ڴ������
		glBindBuffer(GL_ARRAY_BUFFER, be->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// �����ı���
		_renderQuad->draw();

		// ����λ�õ���һ�����ε�ԭ�㣬ע�ⵥλ��1/64����
		x += (ch.Advance >> 6) * scale; // λƫ��6����λ����ȡ��λΪ���ص�ֵ (2^6 = 64)
	}
	
}