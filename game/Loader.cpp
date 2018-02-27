#include "Loader.h"

Loader::Loader()
{
	m_font = new Font();

	m_textures = NULL;
	m_hamster_tex = nullptr;
	
	m_timer = 0;
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

	m_hamster_tex = new TextureSet();
	m_hamster_tex->load("hamster.atlas.xml");

	float hw = 25.0f;
	float hh = 25.0f;
	float hx = 50.0f - (hw / 2);
	float hy = 10.0f;
	

	m_wheel_anim = new PolyAnim(glm::vec2(hx, hy), glm::vec2(hx, hy + hh), glm::vec2(hx + hw, hy + hh), glm::vec2(hx + hw, hy), 0, 18, 0.0f, m_hamster_tex);
	m_hamster_anim = new PolyAnim(glm::vec2(hx, hy), glm::vec2(hx, hy + hh), glm::vec2(hx + hw, hy + hh), glm::vec2(hx + hw, hy), 18, 6, 0.0f, m_hamster_tex);

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
	if (m_hamster_tex != nullptr)
	{
		delete m_hamster_tex;
		m_hamster_tex = nullptr;
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

void Loader::render(const Shaders::UIShaderContext *context, const Shaders::GameShaderContext* game_shader)
{
	float center_x = Screen::getWidthFraction(0.5f);
	float center_y = Screen::getHeightFraction(0.5f);

	glDisable(GL_CULL_FACE);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepthf(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(context->shader);

	glBindBuffer(GL_ARRAY_BUFFER, 0);		// TODO REMOVE

	glUniform2f(context->location, Screen::getWidthSlice(3), center_y - Screen::getHeightSlice(7.5f));
	glUniform2f(context->scale, 1.0f, 1.0f);
	
	m_bartex->apply(0);
	m_barmesh->render(context);

	m_text->setPosition(center_x, center_y - Screen::getHeightSlice(6.0f));
	m_text->render(context, 0.0f, 0.0f);

	
	glUseProgram(game_shader->shader);
	m_hamster_tex->apply(0);

	float gw = 50.0f;
	float gh = gw / Screen::getAspect();

	// setup matrices
	float level_vp_mat[4] = { 1.0f / gw, 0.0f,
		0.0f, 1.0f / gh };
	glUniformMatrix2fv(game_shader->vp_matrix, 1, false, level_vp_mat);

	float level_rot_mat[4] = { 1.0f, 0.0f,
		0.0f, 1.0f };
	glUniformMatrix2fv(game_shader->rot_matrix, 1, false, level_rot_mat);

	//glUniform2f(m_game_shader->location, bgx + m_bg_width, -1.0f);
	glUniform2f(game_shader->location, 0.0f, 0.0f);
	glUniform2f(game_shader->scale, 1.0f, 1.0f);
	glUniform4f(game_shader->color, 1.0f, 1.0f, 1.0f, 1.0f);

	int frame = ((m_timer / 1000000) / 40) % m_wheel_anim->getNumFrames();
	m_wheel_anim->render(game_shader, frame);

	int hframe = ((m_timer / 1000000) / 40) % m_hamster_anim->getNumFrames();
	m_hamster_anim->render(game_shader, hframe);
}

void Loader::update(int time)
{
	m_timer += time;
}
