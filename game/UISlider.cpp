#include "UISlider.h"

UISlider::UISlider(float x, float y, float width, float height, int min_value, int max_value,
				   TextureSet* texture, std::string frame_texture, std::string slider_texture)
				   : UIItem(x, y, width, height)
{
	init(min_value, max_value, texture, frame_texture, slider_texture);
	generateVB();
}

UISlider::UISlider(float width, float height, int min_value, int max_value,
				   TextureSet* texture, std::string frame_texture, std::string slider_texture)
				   : UIItem(0, 0, width, height)
{
	init(min_value, max_value, texture, frame_texture, slider_texture);
	generateVB();
}

UISlider::~UISlider()
{
	if (m_mesh)
		delete m_mesh;
}

void UISlider::init(int min_value, int max_value, TextureSet* texture, std::string frame_tex, std::string slider_tex)
{
	m_mesh = new QuadMesh2(2);

	m_texture = texture;
	m_slider_tex = m_texture->findTexture(slider_tex);
	m_frame_tex = m_texture->findTexture(frame_tex);

	m_min_value = min_value;
	m_max_value = max_value;
	m_current_value = min_value;

	m_pushed = false;
}

void UISlider::setValue(int value)
{
	assert(value < m_min_value || value > m_max_value);

	m_current_value = value;

	generateVB();
}

int UISlider::getValue()
{
	return m_current_value;
}

void UISlider::setRange(int min_value, int max_value)
{
	m_min_value = min_value;
	m_max_value = max_value;
	m_current_value = min_value;

	generateVB();
}

void UISlider::generateVB()
{
	float slider_scale = (float)(m_current_value - m_min_value) / (float)(m_max_value - m_min_value);

	m_mesh->reset();

	// slider
	m_mesh->insert(glm::vec2(m_width * slider_scale, 0.0f),
				   glm::vec2(0.0f, 0.0f),
				   glm::vec2(m_width * slider_scale, m_height),
				   glm::vec2(0.0f, m_height),
				   glm::vec2(m_slider_tex->m_left + (m_slider_tex->m_right * slider_scale), m_slider_tex->m_top),
				   glm::vec2(m_slider_tex->m_left, m_slider_tex->m_top),
				   glm::vec2(m_slider_tex->m_left + (m_slider_tex->m_right * slider_scale), m_slider_tex->m_bottom),
				   glm::vec2(m_slider_tex->m_left, m_slider_tex->m_bottom));

	// frame
	m_mesh->insert(glm::vec2(m_width, 0.0f),
				   glm::vec2(0.0f, 0.0f),
				   glm::vec2(m_width, m_height),
				   glm::vec2(0.0f, m_height),
				   glm::vec2(m_frame_tex->m_left, m_frame_tex->m_top),
				   glm::vec2(m_frame_tex->m_right, m_frame_tex->m_top),
				   glm::vec2(m_frame_tex->m_left, m_frame_tex->m_bottom),
				   glm::vec2(m_frame_tex->m_right, m_frame_tex->m_bottom));
}

void UISlider::onTouch(UI::UITouchEvent event, float x, float y)
{
	// don't handle event if item is not enabled
	if (!m_enabled)
		return;

	bool inside = isInside(x, y);

	if (event == UI::UI_TOUCH_DOWN && inside)
	{
		m_pushed = true;
	}
	else if (event == UI::UI_TOUCH_UP)
	{
		m_pushed = false;
	}

	if (m_pushed && inside)
	{
		float leftx = m_x - (m_width / 2);

		float num_vals = (float)(m_max_value - m_min_value);

		// NOTE!
		// position min and position max are half width (0.5f)
		// this adds up to max_val - min_val distinct positions

		float ix = x - leftx;
		int position = (int)(ix / (m_width / num_vals) + 0.5f) + m_min_value;

		if (position != m_current_value)
		{
			m_current_value = position;
			generateVB();
		}
	}
}

void UISlider::setSize(float width, float height)
{
	m_width = width;
	m_height = height;

	generateVB();
}

void UISlider::setTexture(TextureSet* texture, std::string frame_texture, std::string slider_texture)
{
	m_texture = texture;
	m_frame_tex = m_texture->findTexture(frame_texture);
	m_slider_tex = m_texture->findTexture(slider_texture);

	generateVB();
}

void UISlider::onRemove()
{
}

void UISlider::onUpdate(int time)
{
}

void UISlider::onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y)
{
	// don't draw if item is not enabled
	if (!m_enabled)
		return;

	m_texture->apply(0);

	glUniform2f(context->location, m_x - (m_width / 2) + parent_x + m_trans_pos.x, m_y - (m_height / 2) + parent_y + m_trans_pos.y);
	glUniform2f(context->scale, 1.0f, 1.0f);
	glUniform4f(context->color, m_color.r, m_color.b, m_color.g, m_color.a);

	m_mesh->render(context);
}