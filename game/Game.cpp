#include "Game.h"

Game::Game()
{
	m_engine = new Engine();
	m_menu = new GameMenu();

	m_loader = new Loader();

	// initial gamestate
	m_gamestate = GAMESTATE_MENU_INIT;

	//m_uiflipped = false;
	//m_uiflip_timer = 0;
}

Game::~Game()
{
	delete m_engine;
	delete m_menu;

	delete m_loader;
}

void Game::init(int screen_width, int screen_height)
{
	Debug::start();

	Atlas::init();

	Screen::setup(screen_width, screen_height);

	// init shaders
	GLenum error = 0;

	GLuint shader;

	// load 2D game shader
	shader = Shaders::loadShaderProgram("game_vertshader.glsl",
										"game_fragshader.glsl");
	if (shader == 0)
		Debug::error("Engine::init(): load game shader failed.");

	m_game_shader.shader = shader;

	m_game_shader.position					= glGetAttribLocation(shader, "a_position");
	m_game_shader.tex_coord					= glGetAttribLocation(shader, "a_texcoord");
	m_game_shader.color_sampler				= glGetUniformLocation(shader, "s_color_texture");
	m_game_shader.location					= glGetUniformLocation(shader, "v_location");
	m_game_shader.scale						= glGetUniformLocation(shader, "v_scale");
	m_game_shader.color						= glGetUniformLocation(shader, "v_color");
	m_game_shader.vp_matrix					= glGetUniformLocation(shader, "m_vp_matrix");
	m_game_shader.rot_matrix					= glGetUniformLocation(shader, "m_rot_matrix");

	if (glGetError() != GL_NO_ERROR)
		Debug::error("Engine::init(): game shader attribute mapping failed.");


	// load 3D level shader
	shader = Shaders::loadShaderProgram("level_vertshader.glsl",
										"level_fragshader.glsl");

	if (shader == 0)
		Debug::error("Engine::init(): load level shader failed.");

	m_level_shader.shader = shader;

	m_level_shader.position					= glGetAttribLocation(shader, "a_position");
	m_level_shader.tex_coord				= glGetAttribLocation(shader, "a_texcoord");
	m_level_shader.amb_coord				= glGetAttribLocation(shader, "a_ambcoord");
	m_level_shader.color					= glGetAttribLocation(shader, "a_color");
	m_level_shader.normal					= glGetAttribLocation(shader, "a_normal");
	m_level_shader.light					= glGetUniformLocation(shader, "u_light");
	m_level_shader.cam_pos					= glGetUniformLocation(shader, "u_campos");
	m_level_shader.vp_matrix				= glGetUniformLocation(shader, "m_vp_matrix");
	m_level_shader.v_matrix					= glGetUniformLocation(shader, "m_v_matrix");
	m_level_shader.color_sampler			= glGetUniformLocation(shader, "s_color_texture");
	m_level_shader.amb_sampler				= glGetUniformLocation(shader, "s_amb_texture");

	if (glGetError() != GL_NO_ERROR)
		Debug::error("Engine::init(): level shader attribute mapping failed.");


	// load 2D UI shader
	shader = Shaders::loadShaderProgram("ui_vertshader.glsl",
										"ui_fragshader.glsl");
	if (shader == 0)
		Debug::error("Engine::init(): load UI shader failed.");

	m_ui_shader.shader = shader;

	m_ui_shader.position						= glGetAttribLocation(shader, "a_position");
	m_ui_shader.tex_coord					= glGetAttribLocation(shader, "a_texcoord");
	m_ui_shader.color_sampler				= glGetUniformLocation(shader, "s_color_texture");
	m_ui_shader.location						= glGetUniformLocation(shader, "v_location");
	m_ui_shader.scale						= glGetUniformLocation(shader, "v_scale");
	m_ui_shader.vp_matrix					= glGetUniformLocation(shader, "m_vp_matrix");
	m_ui_shader.rot_matrix					= glGetUniformLocation(shader, "m_rot_matrix");
	m_ui_shader.transform					= glGetUniformLocation(shader, "v_transform");
	m_ui_shader.color						= glGetUniformLocation(shader, "v_color");

	if (glGetError() != GL_NO_ERROR)
		Debug::error("Engine::init(): UI shader attribute mapping failed.");


	// load particle shader
	shader = Shaders::loadShaderProgram("particle_vertshader.glsl",
										"particle_fragshader.glsl");

	if (shader == 0)
		Debug::error("Engine::init(): load particle shader failed.");

	m_particle_shader.shader = shader;

	m_particle_shader.position				= glGetAttribLocation(shader, "a_position");
	//m_particle_shader.direction				= glGetAttribLocation(shader, "a_direction");
	m_particle_shader.texture				= glGetAttribLocation(shader, "a_texture");
	m_particle_shader.color_sampler			= glGetUniformLocation(shader, "s_color_texture");
	m_particle_shader.location				= glGetUniformLocation(shader, "v_location");
	m_particle_shader.vp_matrix				= glGetUniformLocation(shader, "m_vp_matrix");
	m_particle_shader.part_size				= glGetUniformLocation(shader, "f_part_size");

	if (glGetError() != GL_NO_ERROR)
		Debug::error("Engine::init(): particle shader attribute mapping failed.");

	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);

	Debug::log("Engine::init(): loaded shaders");


	// init loading screen
	m_loader->init(100);

	// set initial gamestate
	m_gamestate = GAMESTATE_MENU_INIT;
	m_nextpage = GameMenu::PAGE_MAIN_MENU;
}

void Game::shutdown()
{
	m_engine->shutdown();
	m_loader->shutdown();
	m_menu->shutdown();

	Debug::end();
}

void Game::reportMS(int framems)
{
	m_engine->reportMS(framems);
}

void Game::reportF2FMS(int ms)
{
	m_engine->reportF2FMS(ms);
}

void Game::reportFPS(int fps)
{
	m_engine->reportFPS(fps);
}

void Game::changeState(Game::GameState newstate)
{
	m_gamestate = newstate;
}

void Game::onUpdate(int time)
{
	// increase flip timer if we're upside down
	/*
	float angle = fabs(m_uiflip_angle);
	if ((!m_uiflipped && angle > 135.0f) || (m_uiflipped && angle < 45.0f))
	{
		m_uiflip_timer += time;
		if (m_uiflip_timer >= 1000000000)		// flip if we're upside down for > 1.0 secs
		{
			m_uiflip_timer = 0;
			m_uiflipped = !m_uiflipped;
		}
	}
	*/

	switch (m_gamestate)
	{
		case GAMESTATE_MENU_INIT:
		{
			m_menu->init(m_nextpage);

			changeState(GAMESTATE_MENU);
			break;
		}

		case GAMESTATE_MENU:
		{
			m_menu->onUpdate(time);
			if (m_menu->getStatus() == GameMenu::GAMEMENU_STATUS_STARTGAME)
			{
				int level = m_menu->getSelectedLevel();
				m_menu->shutdown();

				changeState(GAMESTATE_INGAME_LOAD);
				m_loader->setPosition(0);

				const char* level_name[2] = {"level1.level", "level1.level"};

				m_engine->init(level_name[level]);
				m_engine->setShaders(&m_game_shader, &m_level_shader, &m_ui_shader, &m_particle_shader);
			}
			break;
		}

		case GAMESTATE_INGAME:
		{
			m_engine->onUpdate(time);
			if (m_engine->getStatus() == Engine::STATUS_EXIT)
			{
				m_engine->shutdown();

				changeState(GAMESTATE_MENU_INIT);
				m_nextpage = GameMenu::PAGE_LEVEL_SELECT;
			}
			break;
		}

		case GAMESTATE_INGAME_LOAD:
		{
			int pos = m_engine->load();
			m_loader->setPosition(pos);
			m_loader->update(time);

			if (pos >= 100)
				changeState(GAMESTATE_INGAME);
			break;
		}

		default:
			break;
	}
}

void Game::onRender()
{
	/*
	if (m_uiflipped)
		glUniform4f(m_ui_shader.transform, -1.0f, -1.0f, 0.0f, 0.0f);
	else*/
		glUniform4f(m_ui_shader.transform, 1.0f, 1.0f, 0.0f, 0.0f);

	switch (m_gamestate)
	{
		case GAMESTATE_MENU_INIT:
		{
			break;
		}

		case GAMESTATE_MENU:
		{
			m_menu->onRender(&m_ui_shader);
			break;
		}

		case GAMESTATE_INGAME:
		{
			m_engine->onRender();
			break;
		}

		case GAMESTATE_INGAME_LOAD:
		{
			m_loader->render(&m_ui_shader);
			break;
		}

		default:
			break;
	}
}

void Game::onTouch(UI::UITouchEvent type, float x, float y)
{
	/*
	if (m_uiflipped)
	{
		x = Screen::getWidth() - x;
		y = Screen::getHeight() - y;
	}
	*/

	switch (m_gamestate)
	{
		case GAMESTATE_MENU:
		{
			m_menu->onTouch(type, x, y);
			break;
		}

		case GAMESTATE_INGAME:
		{
			m_engine->onTouch(type, x, y);
			break;
		}

		default:
			break;
	}
}

void Game::onSensor(float *sensor_data)
{
#if 0
	switch (m_gamestate)
	{
		case GAMESTATE_INGAME:
		{
			/*
			// calculate orientation matrix
			glm::vec3 gravity = glm::vec3(sensor_data[0], sensor_data[1], sensor_data[2]);
			glm::vec3 geomag  = glm::vec3(sensor_data[3], sensor_data[4], sensor_data[5]);

			glm::vec3 h = glm::cross(gravity, geomag);
			float hlen = h.length();
			if (hlen > 0.1f)		// not in freefall
			{
				h = glm::normalize(h);
				glm::vec3 a = glm::normalize(gravity);
				glm::vec3 m = glm::cross(a, h);

				//m_engine->onRotate(h, m, a);

				float inv_e = 1.0f / sqrt(geomag.x*geomag.x + geomag.y*geomag.y + geomag.z*geomag.z);
				float c = (geomag.x*m.x + geomag.y*m.y + geomag.z*m.z) * inv_e;
				float s = (geomag.x*a.x + geomag.y*a.y + geomag.z*a.z) * inv_e;

				glm::vec3 x = glm::vec3(1.0f, 0.0f, 0.0f);
				glm::vec3 y = glm::vec3(0.0f, c, s);
				glm::vec3 z = glm::vec3(0.0f, -s, c);

				m_engine->onRotate(x, y, z);
			}
			*/

			glm::vec3 x = glm::vec3(sensor_data[0], sensor_data[1], sensor_data[2]);
			glm::vec3 y = glm::vec3(sensor_data[3], sensor_data[4], sensor_data[5]);

			float xxv = x.x * 0.1f;
			float yyv = x.y * 0.1f;
			float ang = atan2(yyv,xxv) * 180.0f / M_PI;
			glm::vec3 z = glm::vec3(ang,0,0);

			m_uiflip_angle = ang;

			m_engine->onRotate(x,y,z);
			break;
		}

		default:
			break;
	}
#endif

	glm::vec3 x = glm::vec3(sensor_data[0], sensor_data[1], sensor_data[2]);

	float xxv = x.x * 0.1f;
	float yyv = x.y * 0.1f;
	float ang = atan2(yyv,xxv) * 180.0f / M_PI;
	glm::vec3 z = glm::vec3(ang,0,0);

	if (sensor_data[2] <= 8.0f)	// deadzone, when >8.0f the phone is almost pointing up
	{
		//m_uiflip_angle = ang;

		if (m_gamestate == GAMESTATE_INGAME)
		{
			m_engine->setInputAngle(ang);
		}
	}
}
