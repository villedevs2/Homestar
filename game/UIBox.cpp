#include "UIBox.h"

UIBox::UIBox(float x, float y, float width, float height, float border_width, float border_height, TextureSet* texset,
		     const UIBox::Definition& def)
			 : UIItem(x + (width/2), y + (height/2), width, height)
{
	m_mesh = new QuadMesh2(18);

	m_border_width = border_width;
	m_border_height = border_height;

	m_texset = texset;

	m_tex_corner_tl = m_texset->findTexture(def.corner_tl);
	m_tex_corner_tr = m_texset->findTexture(def.corner_tr);
	m_tex_corner_bl = m_texset->findTexture(def.corner_bl);
	m_tex_corner_br = m_texset->findTexture(def.corner_br);
	m_tex_border_l = m_texset->findTexture(def.border_l);
	m_tex_border_r = m_texset->findTexture(def.border_r);
	m_tex_border_t = m_texset->findTexture(def.border_t);
	m_tex_border_b = m_texset->findTexture(def.border_b);
	m_tex_contents = m_texset->findTexture(def.contents);

	calculateLayout();

	generateVB();
}

UIBox::~UIBox()
{
	if (m_mesh)
		delete m_mesh;
}

void UIBox::calculateLayout()
{
	float x = m_x - (m_width / 2);
	float y = m_y - (m_height / 2);

	float lb_w = m_width / 4;
	float lb_h = m_height / 4;
	float lb_x = x + 24 + (lb_w / 2);
	float lb_y = y + m_height - 24 - (lb_h / 2);

	float rb_w = m_width / 4;
	float rb_h = m_height / 4;
	float rb_x = x + m_width - 24 - (rb_w / 2);
	float rb_y = y + m_height - 24 - (rb_h / 2);
}

void UIBox::generateVB()
{
	/*		xl1 xl2							xr1 xr2

	yt1		*---*----------------------------*---*
			| 0 |            B0              | 1 |
	yt2		*---*----------------------------*---*
			|   |                            |   |
			|   *----------------------------*   |
			|   |                            |   |
			|B1 |                            |B2 |
			|   |                            |   |
			|   |                            |   |
			|   |                            |   |
	yb1		*---*----------------------------*---*
			| 2 |            B3              | 3 |
	yb2		*---*----------------------------*---*

	*/

	// 0,0 is in the center!
	float x = 0;
	float y = 0;

	float xl1 = x;
	float xl2 = x + m_border_width;
	float xr1 = x + m_width - m_border_width;
	float xr2 = x + m_width;

	float yt1 = y;
	float yt2 = y + m_border_height;
	float yb1 = y + m_height - m_border_height;
	float yb2 = y + m_height;

	float content_y = yt2;		// TODO: change if titlebar is added

	// build polygons
	// ----------------------------

	m_mesh->reset();

	// top-left corner
	m_mesh->insert(glm::vec2(xl1, yt1), glm::vec2(xl2, yt1), glm::vec2(xl1, yt2), glm::vec2(xl2, yt2), m_tex_corner_tl);

	// top border
	m_mesh->insert(glm::vec2(xl2, yt1), glm::vec2(xr1, yt1), glm::vec2(xl2, yt2), glm::vec2(xr1, yt2), m_tex_border_t);

	// top-right corner
	m_mesh->insert(glm::vec2(xr1, yt1), glm::vec2(xr2, yt1), glm::vec2(xr1, yt2), glm::vec2(xr2, yt2), m_tex_corner_tr);

	// left border
	m_mesh->insert(glm::vec2(xl1, yt2), glm::vec2(xl2, yt2), glm::vec2(xl1, yb1), glm::vec2(xl2, yb1), m_tex_border_l);

	// title bar
	// TODO?

	// content area
	m_mesh->insert(glm::vec2(xl2, content_y), glm::vec2(xr1, content_y), glm::vec2(xl2, yb1), glm::vec2(xr1, yb1), m_tex_contents);

	// right border
	m_mesh->insert(glm::vec2(xr1, yt2), glm::vec2(xr2, yt2), glm::vec2(xr1, yb1), glm::vec2(xr2, yb1), m_tex_border_r);

	// bottom-left corner
	m_mesh->insert(glm::vec2(xl1, yb1), glm::vec2(xl2, yb1), glm::vec2(xl1, yb2), glm::vec2(xl2, yb2), m_tex_corner_bl);

	// bottom border
	m_mesh->insert(glm::vec2(xl2, yb1), glm::vec2(xr1, yb1), glm::vec2(xl2, yb2), glm::vec2(xr1, yb2), m_tex_border_b);

	// bottom-right corner
	m_mesh->insert(glm::vec2(xr1, yb1), glm::vec2(xr2, yb1), glm::vec2(xr1, yb2), glm::vec2(xr2, yb2), m_tex_corner_br);
}

void UIBox::onRemove()
{
}

void UIBox::onUpdate(int time)
{
}

void UIBox::onTouch(UI::UITouchEvent event, float x, float y)
{
	// don't handle event if item is not enabled
	if (!m_enabled)
		return;
}

void UIBox::onRender(const Shaders::UIShaderContext *context, float parent_x, float parent_y)
{
	// don't draw if item is not enabled
	if (!m_enabled)
		return;

	glUniform2f(context->location, m_x - (m_width / 2) + parent_x, m_y - (m_height / 2) + parent_y);
	glUniform2f(context->scale, 1.0f, 1.0f);
	glUniform4f(context->color, m_color.r, m_color.g, m_color.b, m_color.a);

	// messagebox layout
	m_texset->apply(0);
	m_mesh->render(context);
}
