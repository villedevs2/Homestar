#include "UIMessageBox.h"

UIMessageBox::UIMessageBox(float x, float y, float width, float height, TextureSet* texset,
						   const UIMessageBox::Definition& def)
						  : UIItem(x + (width/2), y + (height/2), width, height)
{
	m_mesh = new QuadMesh2(18);

	m_texset = texset;

	m_border_left = 8.0f;
	m_border_top = 8.0f;
	m_border_right = 8.0f;
	m_border_bottom = 8.0;
	m_titlebar_height = 40.0f;	

/*
	float lb_w = width / 4;
	float lb_h = height / 4;
	float lb_x = x + 24 + (lb_w / 2);
	float lb_y = y + height - 24 - (lb_h / 2);

	float rb_w = width / 4;
	float rb_h = height / 4;
	float rb_x = x + width - 24 - (rb_w / 2);
	float rb_y = y + height - 24 - (rb_h / 2);

	m_left_button = new UIButton(1, lb_x, lb_y, lb_w, lb_h, atlas, def.left_button_up, def.left_button_down);
	m_right_button = new UIButton(2, rb_x, rb_y, rb_w, rb_h, atlas, def.right_button_up, def.right_button_down);

	m_border_left = 8.0f;
	m_border_top = 8.0f;
	m_border_right = 8.0f;
	m_border_bottom = 8.0;
	m_titlebar_height = 32.0f;	

	m_tex_corner_tl = m_atlas->getTexture(def.corner_tl);
	m_tex_corner_tr = m_atlas->getTexture(def.corner_tr);
	m_tex_corner_bl = m_atlas->getTexture(def.corner_bl);
	m_tex_corner_br = m_atlas->getTexture(def.corner_br);
	m_tex_border_l = m_atlas->getTexture(def.border_l);
	m_tex_border_r = m_atlas->getTexture(def.border_r);
	m_tex_border_t = m_atlas->getTexture(def.border_t);
	m_tex_border_b = m_atlas->getTexture(def.border_b);
	m_tex_contents = m_atlas->getTexture(def.contents);
	m_tex_titlebar = m_atlas->getTexture(def.titlebar);

	m_smallfont = def.smallfont;

	m_text = new Text(strlen(def.text)+1);
	m_text->setFont(m_smallfont);
	m_text->setAnchor(Text::ANCHOR_TOP_LEFT);
	m_text->set(def.text);
	m_text->setPosition(x + m_border_left + 2, y + m_border_top + 2);
*/

	float button_width = width / 4;
	float button_height = height / 4;

	Texture* left_button_up_tex = m_texset->findTexture(def.left_button_up);
	Texture* left_button_down_tex = m_texset->findTexture(def.left_button_down);
	Texture* right_button_up_tex = m_texset->findTexture(def.right_button_up);
	Texture* right_button_down_tex = m_texset->findTexture(def.right_button_down);

	m_left_button = new UIButton(1, 0, 0, button_width, button_height, left_button_up_tex, left_button_down_tex);
	m_right_button = new UIButton(2, 0, 0, button_width, button_height, right_button_up_tex, right_button_down_tex);

	m_tex_corner_tl = m_texset->findTexture(def.corner_tl);
	m_tex_corner_tr = m_texset->findTexture(def.corner_tr);
	m_tex_corner_bl = m_texset->findTexture(def.corner_bl);
	m_tex_corner_br = m_texset->findTexture(def.corner_br);
	m_tex_border_l = m_texset->findTexture(def.border_l);
	m_tex_border_r = m_texset->findTexture(def.border_r);
	m_tex_border_t = m_texset->findTexture(def.border_t);
	m_tex_border_b = m_texset->findTexture(def.border_b);
	m_tex_contents = m_texset->findTexture(def.contents);
	m_tex_titlebar = m_texset->findTexture(def.titlebar);

	m_smallfont = def.smallfont;

	m_text = new Text(strlen(def.text)+1);
	m_text->setFont(m_smallfont);
	m_text->setAnchor(Text::ANCHOR_X_LEFT | Text::ANCHOR_Y_TOP);
	m_text->set(def.text);
	m_text->setPosition(0, 0);	// dummy

	m_titlefont = def.titlefont;

	m_title = new Text(strlen(def.title)+1);
	m_title->setFont(m_titlefont);
	m_title->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_CENTER);
	m_title->set(def.title);
	m_title->setPosition(0, 0);	// dummy


	calculateLayout();

	generateVB();
}

UIMessageBox::~UIMessageBox()
{
	if (m_mesh)
		delete m_mesh;

	if (m_left_button)
		delete m_left_button;

	if (m_right_button)
		delete m_right_button;

	if (m_title)
		delete m_title;

	if (m_text)
		delete m_text;
}

void UIMessageBox::calculateLayout()
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

	m_left_button->setPosition(lb_x, lb_y);
	m_right_button->setPosition(rb_x, rb_y);

	float text_x = x + m_border_left + 3;
	float text_y = y + m_border_top + m_titlebar_height + 3;

	m_text->setPosition(text_x, text_y);


	float title_x = m_x;
	float title_y = y + m_border_top + (m_titlebar_height / 2) + 3;

	m_title->setPosition(title_x, title_y);
}

void UIMessageBox::generateVB()
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
	float xl2 = x + m_border_left;
	float xr1 = x + m_width - m_border_right;
	float xr2 = x + m_width;

	float yt1 = y;
	float yt2 = y + m_border_top;
	float yb1 = y + m_height - m_border_bottom;
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

void UIMessageBox::setTitle(const char* text)
{
	m_title->set(text);
}

void UIMessageBox::setText(const char* text)
{
	m_text->set(text);
}

void UIMessageBox::onRemove()
{
}

void UIMessageBox::onUpdate(int time)
{
}

void UIMessageBox::onTouch(UI::UITouchEvent event, float x, float y)
{
	// don't handle event if item is not enabled
	if (!m_enabled)
		return;

	bool inside = isInside(x, y);

	if (inside)
	{
		m_left_button->onTouch(event, x, y);
		m_right_button->onTouch(event, x, y);
	}
}

void UIMessageBox::onRender(const Shaders::UIShaderContext *context, float parent_x, float parent_y)
{
	// don't draw if item is not enabled
	if (!m_enabled)
		return;

	glUniform2f(context->location, m_x - (m_width / 2) + parent_x, m_y - (m_height / 2) + parent_y);
	glUniform2f(context->scale, 1.0f, 1.0f);

	// messagebox layout
	m_texset->apply(0);
	m_mesh->render(context);

	// title text
	m_title->render(context, parent_x, parent_y);

	// content text
	m_text->render(context, parent_x, parent_y);

	// buttons
	m_left_button->onRender(context, parent_x, parent_y);
	m_right_button->onRender(context, parent_x, parent_y);
}
