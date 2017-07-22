#include "UIButton.h"

UIButton::UIButton(int id, float x, float y, float width, float height, Texture* up_texture, Texture* down_texture)
	: UIBaseButton(id, x, y, width, height)
{
	init(up_texture, down_texture);
}

UIButton::UIButton(float x, float y, float width, float height, Texture* up_texture, Texture* down_texture)
	: UIBaseButton(x, y, width, height)
{
	init(up_texture, down_texture);
}

UIButton::UIButton(int id, float width, float height, Texture* up_texture, Texture* down_texture)
	: UIBaseButton(id, 0.0f, 0.0f, width, height)
{
	init(up_texture, down_texture);
}

UIButton::UIButton(float width, float height, Texture* up_texture, Texture* down_texture)
	: UIBaseButton(0.0f, 0.0f, width, height)
{
	init(up_texture, down_texture);
}

UIButton::~UIButton()
{
	delete m_mesh_up;
	delete m_mesh_down;
}

void UIButton::init(Texture* up_texture, Texture* down_texture)
{
	m_texture_up = up_texture;
	m_texture_down = down_texture;

	float w = m_width;
	float h = m_height;

	m_mesh_up = new QuadMesh2(1);
	m_mesh_up->insert(glm::vec2(0.0f, 0.0f), glm::vec2(w, 0.0f), glm::vec2(0.0f, h), glm::vec2(w, h), m_texture_up);

	m_mesh_down = new QuadMesh2(1);
	m_mesh_down->insert(glm::vec2(0.0f, 0.0f), glm::vec2(w, 0.0f), glm::vec2(0.0f, h), glm::vec2(w, h), m_texture_down);
}

void UIButton::onRemove()
{
}

void UIButton::onUpdate(int time)
{
}

void UIButton::onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y)
{
	// don't draw if not enabled
	if (!m_enabled)
		return;

	glUniform2f(context->location,
				m_x - (m_width / 2) + parent_x + m_trans_pos.x,
				m_y - (m_height / 2) + parent_y + m_trans_pos.y);
	glUniform2f(context->scale,
				1.0f * m_trans_scale.x,
				1.0f * m_trans_scale.y);
	glUniform4f(context->color, m_color.r, m_color.g, m_color.b, m_color.a);

	if (m_state == UIBaseButton::STATE_UP)
	{
		m_texture_up->apply(0);
		m_mesh_up->render(context);
	}
	else
	{
		m_texture_down->apply(0);
		m_mesh_down->render(context);
	}
}