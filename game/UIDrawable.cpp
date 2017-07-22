#include "UIDrawable.h"

UIDrawable::UIDrawable(float x, float y, float width, float height, Texture* texture)
	: UIItem(x, y, width, height)
{
	init(texture);
}

UIDrawable::UIDrawable(float width, float height, Texture* texture)
	: UIItem(0.0f, 0.0f, width, height)
{
	init(texture);
}

UIDrawable::~UIDrawable()
{
	if (m_mesh)
		delete m_mesh;
}

void UIDrawable::init(Texture* texture)
{
	m_texture = texture;

	m_mesh = new QuadMesh2(1);

	generateVB();
}

void UIDrawable::generateVB()
{
	m_mesh->reset();

	if (m_texture != NULL)
	{
		m_mesh->insert(glm::vec2(0.0f, 0.0f),
					   glm::vec2(m_width, 0.0f),
					   glm::vec2(0.0f, m_height),
					   glm::vec2(m_width, m_height),
					   m_texture);
	}
	else
	{
		m_mesh->insert(glm::vec2(m_width, 0.0f),
					   glm::vec2(0.0f, 0.0f),
					   glm::vec2(m_width, m_height),
					   glm::vec2(0.0f, m_height),
					   glm::vec2(1.0f, 0.0f),
					   glm::vec2(0.0f, 0.0f),
					   glm::vec2(1.0f, 1.0f),
					   glm::vec2(0.0f, 1.0f));
	}
}

void UIDrawable::setSize(float width, float height)
{
	m_width = width;
	m_height = height;

	generateVB();
}

void UIDrawable::setTexture(Texture* texture)
{
	m_texture = texture;

	generateVB();
}

void UIDrawable::onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y)
{
	// don't draw if not enabled
	if (!m_enabled)
		return;

	m_texture->apply(0);

	glUniform2f(context->location, m_x - (m_width / 2) + parent_x, m_y - (m_height / 2) + parent_y);
	glUniform2f(context->scale, 1.0f, 1.0f);
	glUniform4f(context->color, m_color.r, m_color.g, m_color.b, m_color.a);

	m_mesh->render(context);
}