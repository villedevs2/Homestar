#include "UITextButton.h"

UITextButton::UITextButton(int id, float x, float y, float width, float height, std::string text, Font* font, Texture* texture)
	: UIBaseButton(id, x, y, width, height)
{
	init(text, font, texture, texture);
}

UITextButton::UITextButton(int id, float x, float y, float width, float height, std::string text, Font* font, Texture* texup, Texture* texdown)
	: UIBaseButton(id, x, y, width, height)
{
	init(text, font, texup, texdown);
}

UITextButton::UITextButton(int id, float x, float y, float width, float height, float left, float right, std::string text, Font* font, TextureSet* texset, const UITextButton::Def& def)
	: UIBaseButton(id, x, y, width, height)
{
	initDef(text, left, right, font, texset, def);
}


UITextButton::~UITextButton()
{
	delete m_text;
}

void UITextButton::init(std::string text, Font* font, Texture* texup, Texture* texdown)
{
	m_font = font;
	m_texture_up = texup;
	m_texture_down = texdown;

	m_text = new Text(text.length() + 1);
	m_text->setFont(font);
	m_text->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_CENTER);
	m_text->set(text.c_str());

	float w = m_width;
	float h = m_height;

	m_mesh_up = new QuadMesh2(1);
	m_mesh_up->insert(glm::vec2(0.0f, 0.0f), glm::vec2(w, 0.0f), glm::vec2(0.0f, h), glm::vec2(w, h), m_texture_up);

	m_mesh_down = new QuadMesh2(1);
	m_mesh_down->insert(glm::vec2(0.0f, 0.0f), glm::vec2(w, 0.0f), glm::vec2(0.0f, h), glm::vec2(w, h), m_texture_down);
}

void UITextButton::initDef(std::string text, float left, float right, Font* font, TextureSet* texset, const UITextButton::Def& def)
{
	m_font = font;
	m_texset = texset;

	m_texture_up = m_texset->findTexture(def.tex_up);
	m_texture_down = m_texset->findTexture(def.tex_down);
	m_texup_left = m_texset->findTexture(def.tex_up_left);
	m_texup_right = m_texset->findTexture(def.tex_up_right);
	m_texdown_left = m_texset->findTexture(def.tex_down_left);
	m_texdown_right = m_texset->findTexture(def.tex_down_right);

	m_text = new Text(text.length() + 1);
	m_text->setFont(font);
	m_text->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_CENTER);
	m_text->set(text.c_str());

	float w = m_width;
	float h = m_height;

	m_left_size = left;
	m_right_size = right;

	float lmx = 0.0f + m_left_size;
	float rmx = w - m_right_size;
	float lx = 0.0f;
	float rx = w;

	m_mesh_up = new QuadMesh2(3);
	// mid
	m_mesh_up->insert(glm::vec2(lmx, 0.0f), glm::vec2(rmx, 0.0f), glm::vec2(lmx, h), glm::vec2(rmx, h), m_texture_up);
	// left
	m_mesh_up->insert(glm::vec2(lx, 0.0f), glm::vec2(lmx, 0.0f), glm::vec2(lx, h), glm::vec2(lmx, h), m_texup_left);
	// right
	m_mesh_up->insert(glm::vec2(rmx, 0.0f), glm::vec2(rx, 0.0f), glm::vec2(rmx, h), glm::vec2(rx, h), m_texup_right);

	m_mesh_down = new QuadMesh2(3);
	// mid
	m_mesh_down->insert(glm::vec2(lmx, 0.0f), glm::vec2(rmx, 0.0f), glm::vec2(lmx, h), glm::vec2(rmx, h), m_texture_down);
	// left
	m_mesh_down->insert(glm::vec2(lx, 0.0f), glm::vec2(lmx, 0.0f), glm::vec2(lx, h), glm::vec2(lmx, h), m_texdown_left);
	// right
	m_mesh_down->insert(glm::vec2(rmx, 0.0f), glm::vec2(rx, 0.0f), glm::vec2(rmx, h), glm::vec2(rx, h), m_texdown_right);
}

void UITextButton::onRemove()
{
}

void UITextButton::onUpdate(int time)
{
}

void UITextButton::onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y)
{
	// don't draw if not enabled
	if (!m_enabled)
		return;

	float posx = m_x - (m_width / 2) + parent_x;
	float posy = m_y - (m_height / 2) + parent_y;

	glUniform2f(context->location, posx + m_trans_pos.x, posy + m_trans_pos.y);
	glUniform2f(context->scale, 1.0f, 1.0f);
	glUniform4f(context->color, m_color.r, m_color.g, m_color.b, m_color.a);

	if (m_state == UIBaseButton::STATE_UP)
	{
		m_texture_up->apply(0);
		m_mesh_up->render(context);
		
		m_text->setPosition(m_x + parent_x + m_trans_pos.x, m_y + parent_y + m_trans_pos.y);
		m_text->render(context, 0.0f, 0.0f);
	}
	else
	{
		m_texture_down->apply(0);
		m_mesh_down->render(context);

		m_text->setPosition(m_x + parent_x + m_trans_pos.x, m_y + parent_y + m_trans_pos.y);
		m_text->render(context, 0.0f, 0.0f);
	}
}