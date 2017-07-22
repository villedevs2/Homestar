#include "Loader.h"

Loader::Loader()
{
	m_font = new Font();

	m_textures = NULL;
}

Loader::~Loader()
{
	delete m_font;
}

void Loader::init(int numticks)
{
	m_numticks = numticks;

	m_barmesh = new QuadMesh2(4);
	m_barmesh->set_num(1);

	// load resources
	m_font->load("font10.font.xml", Screen::getHeightSlice(18));
	m_textures = new TextureSet();
	m_textures->load("menutexts.atlas.xml");

	m_bartex = m_textures->findTexture("slider_bar");

	m_text = new Text(20);
	m_text->setScale(1, 1);
	m_text->setFont(m_font);
	m_text->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_CENTER);
	m_text->set("LOADING...");

	m_tick = 0;

	setPosition(m_tick);
}

void Loader::shutdown()
{
	if (m_textures != NULL)
	{
		delete m_textures;
		m_textures = NULL;
	}

	delete m_barmesh;
	delete m_text;
}

void Loader::setPosition(int pos)
{
	m_tick = pos;

	float full_bar_w = Screen::getWidthSlice(3);

	float phase = (float)(m_tick) / (float)(m_numticks);
	float width = (Screen::getWidth() - full_bar_w) * phase;
	float height = Screen::getHeightSlice(15);

	m_barmesh->set(0, glm::vec2(0, 0), glm::vec2(width, 0), glm::vec2(0, height), glm::vec2(width, height), m_bartex);
}

void Loader::render(const Shaders::UIShaderContext *context)
{
	float center_x = Screen::getWidthFraction(0.5f);
	float center_y = Screen::getHeightFraction(0.5f);

	glDisable(GL_CULL_FACE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepthf(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, 0);		// TODO REMOVE

	glUniform2f(context->location, Screen::getWidthSlice(3), center_y - Screen::getHeightSlice(7.5f));
	glUniform2f(context->scale, 1.0f, 1.0f);
	
	m_bartex->apply(0);
	m_barmesh->render(context);

	m_text->setPosition(center_x, center_y - Screen::getHeightSlice(6.0f));
	m_text->render(context, 0.0f, 0.0f);
}

void Loader::update(int time)
{
}
