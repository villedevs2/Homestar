#include "Engine.h"

using namespace std;

#define ENABLE_PARTICLES 1

#define ROTATING_BG_TEST 0

Engine::Engine(void)
{
	m_last_ms = 0;
	m_last_fps = 0;

	// TODO: move object creation to load() ?
	m_dustcloud = new ParticleSys2(1000, NULL);
	m_dustcloud->enableGravity(false);
	m_dustcloud->setParticleSize(40.0f);

	m_sparkles = new ParticleSys2(1000, NULL);
	m_sparkles->enableGravity(true);
	m_sparkles->setParticleSize(20.0f);

	m_bg_width = 64.0f;
	m_bg_height = 32.0f;
#if ROTATION_BG_TEST
	m_bg_width = 20.0f;
	m_bg_height = 20.0f;
#endif

	m_player = new Player();

	m_game_shader = NULL;
	m_ui_shader = NULL;
	m_particle_shader = NULL;

	m_clock = new Clock();
	m_clock->reset();

	m_timer = new TimerSystem(this);

	m_testbox = new QuadMesh2(2);
	m_testbox->insert(glm::vec2(0, 0), glm::vec2(480, 0), glm::vec2(0, 480), glm::vec2(480, 480),
					  glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(0, 1), glm::vec2(1, 1));

	m_load_progress = 0;
	m_status = STATUS_RUNNING;

	m_render_game = true;

	m_vignette = NULL;
	m_dustcloud_tex = NULL;
	m_sparkle_tex = NULL;
	m_gameui_texture = NULL;
	
	m_coin_tex = NULL;
	m_goal_tex = NULL;
	m_level_texture = NULL;

	m_pause_bg = NULL;
	m_pausefont = NULL;
	m_uilayer = NULL;
	m_uiback = NULL;

	m_sensors = NULL;
	m_sensor_font = NULL;

	m_levelbg = NULL;
	m_levelbgtex = NULL;

	m_world = NULL;

	m_geometry = new StaticGeometry();

	m_affixes.affixes = 0;

	m_affixes.affixes |= LEVEL_AFFIX_TIMELIMIT;
	m_affixes.time_limit = 36;

	m_affixes.affixes |= LEVEL_AFFIX_COLLECT1;
	m_affixes.collect_num[0] = 100;
	//m_affixes.affixes |= LEVEL_AFFIX_COLLECT2;
	m_affixes.collect_num[1] = 5;
	m_affixes.affixes |= LEVEL_AFFIX_COLLECT3;
	m_affixes.collect_num[2] = 1;

	m_affixes.affixes |= LEVEL_AFFIX_FIND_EXIT;
}

Engine::~Engine(void)
{
	delete m_dustcloud;
	delete m_sparkles;

	delete m_player;

	delete m_uilayer;

	delete m_testbox;
	delete m_clock;
	delete m_timer;

	delete m_geometry;
}

void Engine::init(std::string levelname)
{
	m_load_progress = 0;
	m_level_name = levelname;
}

int Engine::load()
{
	//Debug::log(str(fmt::Format("Engine::load() {:d}") << m_load_progress));

	switch (m_load_progress)
	{
		case LOAD_STATIC_SHADOWS:
		{
			m_vignette = new Texture();
			m_vignette->load("vignette.png");
			
			break;
		}

		case LOAD_PLAYER_MESHES:
		{
			// init lists
			m_dynobjs.clear();
			m_sprites.clear();
			m_movers.clear();


			m_levelbgtex = new Texture();
			m_levelbgtex->load("bgtest5.png");

#if ROTATION_BG_TEST
			m_levelbg = new PolySprite(glm::vec2(-m_bg_width, -m_bg_height), glm::vec2(-m_bg_width, m_bg_height), glm::vec2(m_bg_width, m_bg_height), glm::vec2(m_bg_width, -m_bg_height),
									   glm::vec2(m_levelbgtex->m_left, m_levelbgtex->m_top),
									   glm::vec2(m_levelbgtex->m_left, m_levelbgtex->m_bottom),
									   glm::vec2(m_levelbgtex->m_right, m_levelbgtex->m_bottom),
									   glm::vec2(m_levelbgtex->m_right, m_levelbgtex->m_top),
									   100.0f);
#endif
			m_levelbg = new PolyAnim(glm::vec2(-m_bg_width, -m_bg_height), glm::vec2(-m_bg_width, m_bg_height), glm::vec2(m_bg_width, m_bg_height), glm::vec2(m_bg_width, -m_bg_height),
									   glm::vec2(m_levelbgtex->m_left, m_levelbgtex->m_top),
									   glm::vec2(m_levelbgtex->m_left, m_levelbgtex->m_bottom * 2.0f),
									   glm::vec2(m_levelbgtex->m_right * 2.0f, m_levelbgtex->m_bottom * 2.0f),
									   glm::vec2(m_levelbgtex->m_right * 2.0f, m_levelbgtex->m_top),
									   100.0f);
			m_sprites.push_back(m_levelbg);


			// init collectables
			m_coin_tex = new Texture();
			m_coin_tex->load("coin2d.png");

			float xl = 0.0f - (GameParams::COIN_WIDTH);
			float xr = 0.0f + (GameParams::COIN_WIDTH);
			float yt = 0.0f - (GameParams::COIN_HEIGHT);
			float yb = 0.0f + (GameParams::COIN_HEIGHT);

			m_coin_sprite = new PolyAnim(glm::vec2(xl, yt), glm::vec2(xl, yb), glm::vec2(xr, yb), glm::vec2(xr, yt), 100.0f, m_coin_tex);
			m_sprites.push_back(m_coin_sprite);


			// init goal
			m_goal_tex = new Texture();
			m_goal_tex->load("goal.png");


			// init physics
			b2Vec2 gravity(0.0f, -10.0f);
			m_world = new b2World(gravity);
			m_world->SetContactListener(this);


			// load level
			m_player_start_pos = glm::vec2(0.0f, 0.0f);
			assert(!m_level_name.empty());
			loadLevel(m_level_name);


			// player
			m_bountyhunter_set = new TextureSet();
			m_bountyhunter_set->load("playerbounty.atlas.xml");

			m_bh_effect_set = new TextureSet();
			m_bh_effect_set->load("playerbountyeffect.atlas.xml");

			m_tire_set = new TextureSet();
			m_tire_set->load("playertire.atlas.xml");

			m_soccer_set = new TextureSet();
			m_soccer_set->load("playersoccer.atlas.xml");

			m_reactor_set = new TextureSet();
			m_reactor_set->load("playerreactor.atlas.xml");

			m_hamsterwheel_set = new TextureSet();
			m_hamsterwheel_set->load("playerhamsterwheel.atlas.xml");

			m_hamsterleft_set = new TextureSet();
			m_hamsterleft_set->load("playerhamsterleft.atlas.xml");

			m_hamsterright_set = new TextureSet();
			m_hamsterright_set->load("playerhamsterright.atlas.xml");


			float rad = GameParams::PLAYER_RADIUS;
			float z = 1000.0f;
			
			PolyAnim* bh_anim = new PolyAnim(glm::vec2(-rad, -rad), glm::vec2(-rad, rad), glm::vec2(rad, rad), glm::vec2(rad, -rad), m_bountyhunter_set->getNumTextures(), z, m_bountyhunter_set);
			m_sprites.push_back(bh_anim);
			PolyAnim* bh_effect_anim = new PolyAnim(glm::vec2(-rad, -rad), glm::vec2(-rad, rad), glm::vec2(rad, rad), glm::vec2(rad, -rad), m_bh_effect_set->getNumTextures(), z+100, m_bh_effect_set);
			m_sprites.push_back(bh_effect_anim);
			PolyAnim* tire_anim = new PolyAnim(glm::vec2(-rad, -rad), glm::vec2(-rad, rad), glm::vec2(rad, rad), glm::vec2(rad, -rad), m_tire_set->getNumTextures(), z, m_tire_set);
			m_sprites.push_back(tire_anim);
			PolyAnim* soccer_anim = new PolyAnim(glm::vec2(-rad, -rad), glm::vec2(-rad, rad), glm::vec2(rad, rad), glm::vec2(rad, -rad), m_soccer_set->getNumTextures(), z, m_soccer_set);
			m_sprites.push_back(soccer_anim);
			PolyAnim* reactor_anim = new PolyAnim(glm::vec2(-rad, -rad), glm::vec2(-rad, rad), glm::vec2(rad, rad), glm::vec2(rad, -rad), m_reactor_set->getNumTextures(), z, m_reactor_set);
			m_sprites.push_back(reactor_anim);
			PolyAnim* hamsterwheel_anim = new PolyAnim(glm::vec2(-rad, -rad), glm::vec2(-rad, rad), glm::vec2(rad, rad), glm::vec2(rad, -rad), m_hamsterwheel_set->getNumTextures(), z, m_hamsterwheel_set);
			m_sprites.push_back(hamsterwheel_anim);
			PolyAnim* hamsterleft_anim = new PolyAnim(glm::vec2(-rad, -rad), glm::vec2(-rad, rad), glm::vec2(rad, rad), glm::vec2(rad, -rad), m_hamsterleft_set->getNumTextures(), z, m_hamsterleft_set);
			m_sprites.push_back(hamsterleft_anim);
			PolyAnim* hamsterright_anim = new PolyAnim(glm::vec2(-rad, -rad), glm::vec2(-rad, rad), glm::vec2(rad, rad), glm::vec2(rad, -rad), m_hamsterright_set->getNumTextures(), z, m_hamsterright_set);
			m_sprites.push_back(hamsterright_anim);



			PlayerTire* player_tire = new PlayerTire(tire_anim, m_tire_set->getAtlas());
			m_dynobjs.push_back(dynamic_cast<DynamicObject*>(player_tire));

			PlayerSoccerBall* player_soccer = new PlayerSoccerBall(soccer_anim, m_soccer_set->getAtlas());
			m_dynobjs.push_back(dynamic_cast<DynamicObject*>(player_soccer));

			PlayerBountyHunter* player_bh = new PlayerBountyHunter(bh_anim, bh_effect_anim, m_bountyhunter_set->getAtlas());
			m_dynobjs.push_back(dynamic_cast<DynamicObject*>(player_bh));

			PlayerReactor* player_reactor = new PlayerReactor(reactor_anim, m_reactor_set->getAtlas());
			m_dynobjs.push_back(dynamic_cast<DynamicObject*>(player_reactor));

			PlayerHamsterWheel* player_hamsterwheel = new PlayerHamsterWheel(hamsterwheel_anim, hamsterleft_anim, hamsterright_anim, m_hamsterleft_set->getAtlas());
			m_dynobjs.push_back(dynamic_cast<DynamicObject*>(player_hamsterwheel));


			insertPlayerObject(player_hamsterwheel);


			//tesselateStaticGeo();
			m_geometry->process();
			m_geometry->tesselate();
			break;
		}

		case LOAD_INIT_PARTICLES:
		{
			// init particles
			m_dustcloud_tex = new TextureSet();
			m_dustcloud_tex->load("dustcloud.atlas.xml");
			m_dustcloud->setTexture(m_dustcloud_tex);

			m_sparkle_tex = new TextureSet();
			m_sparkle_tex->load("sparkles.atlas.xml");
			m_sparkles->setTexture(m_sparkle_tex);


			ParticleSys2::Emitter emitter;
			emitter.position = glm::vec2(0.0f, 0.0f);
			emitter.angle = -90.0f;
			emitter.angle_range = 90.0f;
			emitter.width = 1.0f;
			emitter.height = 1.0f;
			emitter.particle_speed = 5.0f;
			emitter.particle_speed_range = 2.5f;
			emitter.time = 5000;
			emitter.time_range = 2000;
			m_dustcloud->setEmitter(emitter);
			m_sparkles->setEmitter(emitter);

			break;
		}

		case LOAD_INIT_UI:
		{
			UITextButton::Def positive_button_def = { "button2mid", "button2left", "button2right", "button4mid", "button4left", "button4right" };
			UITextButton::Def negative_button_def = { "button3mid", "button3left", "button3right", "button4mid", "button4left", "button4right" };


			float button_w = Screen::getWidthSlice(3);
			float button_h = button_w / 4;
			float button_x = Screen::getWidthFraction(0.5f);
			float button_y1 = Screen::getHeightFraction(0.6f);
			float button_y2 = button_y1 + button_h + (button_h/4);
			float button_sw = button_h / 2;


			// init UI interactive parts
			m_uilayer = new UILayer();
			m_uilayer->setEventCallback(this);

			m_gameui_texture = new TextureSet();
			m_gameui_texture->load("gameui.atlas.xml");
			Texture* back_arrow = m_gameui_texture->findTexture("back_arrow");			

			float arrow_width = 85.0f;
			float arrow_height = 64.0f;
	
			m_uiback = new UIButton(GAMEUIID_BACK, Screen::getWidth() - (arrow_width/2), Screen::getHeight() - (arrow_height/2), arrow_width, arrow_height, back_arrow, back_arrow);
			m_uilayer->addItem(m_uiback);


			m_bigfont = new Font();
			m_bigfont->load("newfont1.font.xml", Screen::getHeightSlice(12));

			m_smallfont = new Font();
			m_smallfont->load("newfont1.font.xml", Screen::getHeightSlice(24));		// new font?

			m_buttonfont = new Font();
			m_buttonfont->load("newfont1.font.xml", Screen::getHeightSlice(24));		// TODO new font

			m_messagefont = new Font();
			m_messagefont->load("newfont1.font.xml", Screen::getHeightSlice(8));		// TODO new font


			// clock

			m_clockicon_tex = new Texture();
			m_clockicon_tex->load("clock.png");

			float clock_size = Screen::getWidthSlice(15);
			float clock_x = clock_size / 1.5f;
			float clock_y = clock_x;

			m_clockicon = new UIStatic(clock_x, clock_y, clock_size, clock_size, m_clockicon_tex);
			m_clocktext = new UIText(64, m_bigfont);
			m_clocktext->setAnchor(Text::ANCHOR_X_LEFT | Text::ANCHOR_Y_CENTER);
			m_clocktext->setPosition((clock_x / 2) + clock_size, clock_y);

			m_uilayer->addItem(m_clockicon);
			m_uilayer->addItem(m_clocktext);

			// collect texts

			m_collect1_text = new UIText(64, m_bigfont);
			m_collect1_text->setAnchor(Text::ANCHOR_X_RIGHT | Text::ANCHOR_Y_CENTER);
			m_collect1_text->setPosition(Screen::getWidth() - (clock_x/2), clock_y);
			m_collect1_text->setText("100/100");

			m_collect2_text = new UIText(64, m_bigfont);
			m_collect2_text->setAnchor(Text::ANCHOR_X_RIGHT | Text::ANCHOR_Y_CENTER);
			m_collect2_text->setPosition(Screen::getWidth() - (clock_x/2), clock_y + clock_size);
			m_collect2_text->setText("5/5");

			m_uilayer->addItem(m_collect1_text);
			m_uilayer->addItem(m_collect2_text);

			// intro box
			// ----------------------------------------------------------------
			UIBox::Definition introbox_def;
			introbox_def.border_b = "box_bottom";
			introbox_def.border_l = "box_left";
			introbox_def.border_r = "box_right";
			introbox_def.border_t = "box_top";
			introbox_def.corner_bl = "box_bl";
			introbox_def.corner_br = "box_br";
			introbox_def.corner_tl = "box_tl";
			introbox_def.corner_tr = "box_tr";
			introbox_def.contents = "box_mid";

			m_introbox_texture = new TextureSet();
			m_introbox_texture->load("newui_buttons.atlas.xml");

			m_introbox = new UIBox(400-275, 200-100, 550, 220, 32, 32, m_introbox_texture, introbox_def);

			m_intro_titlefont = new Font();
			m_intro_titlefont->load("newfont1.font.xml", Screen::getHeightSlice(20));

			// intro box start button
			m_introbox_start = new UITextButton(GAMEUIID_INTRO_START, button_x, button_y1, button_w, button_h, button_sw, button_sw, "start", m_buttonfont, m_introbox_texture, positive_button_def);

			// intro title
			m_intro_title = new UIText(64, m_intro_titlefont);
			m_intro_title->setText("Objectives");

			// intro box affixes
			std::string affix_text = std::string();
			if (m_affixes.affixes & LEVEL_AFFIX_COLLECT1)
			{
				affix_text.append(str(fmt::Format("* Collect {:d} coins") << m_affixes.collect_num[0]));
				affix_text.append("\n");
			}
			if (m_affixes.affixes & LEVEL_AFFIX_COLLECT2)
			{
				affix_text.append(str(fmt::Format("* Collect {:d} diamonds") << m_affixes.collect_num[1]));
				affix_text.append("\n");
			}
			if (m_affixes.affixes & LEVEL_AFFIX_COLLECT3)
			{
				affix_text.append(str(fmt::Format("* Collect {:d} trophies") << m_affixes.collect_num[2]));
				affix_text.append("\n");
			}
			if (m_affixes.affixes & LEVEL_AFFIX_FIND_EXIT)
			{
				affix_text.append(str(fmt::Format("* Find the exit")));
				affix_text.append("\n");
			}
			if (m_affixes.affixes & LEVEL_AFFIX_TIMELIMIT)
			{
				affix_text.append(str(fmt::Format("* Time limit: ")));
				if (m_affixes.time_limit > 60)
					affix_text.append(str(fmt::Format("{:d} minutes, ") << (m_affixes.time_limit / 60)));
				affix_text.append(str(fmt::Format("{:d} seconds") << (m_affixes.time_limit % 60)));
				affix_text.append("\n");
			}

			m_intro_affixes = new UIText(512, m_smallfont);
			m_intro_affixes->setText(affix_text.c_str());
			m_intro_affixes->setPosition(400, 250 - 105);
			m_intro_affixes->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_TOP);

			m_intro_title->setPosition(400, 110);
			m_intro_title->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_TOP);

			m_intro_layer = new UILayer();
			m_intro_layer->addItem(m_introbox);
			m_intro_layer->addItem(m_intro_title);
			m_intro_layer->addItem(m_intro_affixes);
			m_intro_layer->addItem(m_introbox_start);

			m_uilayer->addItem(m_intro_layer);

			// ----------------------------------------------------------------

			// new win
			m_win_title = new UIText(64, m_messagefont);
			m_win_title->setText("LEVEL COMPLETE!");
			m_win_title->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_TOP);
			m_win_title->setPosition(Screen::getWidthFraction(0.5f), Screen::getHeightFraction(0.2f));

			m_win_continue = new UITextButton(GAMEUIID_WIN_CONTINUE, button_x, button_y1, button_w, button_h, button_sw, button_sw, "Continue", m_buttonfont, m_introbox_texture, positive_button_def);
			m_win_quit = new UITextButton(GAMEUIID_WIN_QUIT, button_x, button_y2, button_w, button_h, button_sw, button_sw, "Quit", m_buttonfont, m_introbox_texture, negative_button_def);

			m_win_layer = new UILayer();
			m_win_layer->addItem(m_win_title);
			m_win_layer->addItem(m_win_continue);
			m_win_layer->addItem(m_win_quit);

			m_uilayer->addItem(m_win_layer);


			// failbox
			// ----------------------------------------------------------------
			m_failbox_tryagain = new UITextButton(GAMEUIID_FAIL_TRYAGAIN, button_x, button_y1, button_w, button_h, button_sw, button_sw, "Try Again", m_buttonfont, m_introbox_texture, positive_button_def);
			m_failbox_quit = new UITextButton(GAMEUIID_FAIL_QUIT, button_x, button_y2, button_w, button_h, button_sw, button_sw, "Quit Level", m_buttonfont, m_introbox_texture, negative_button_def);

			m_failbox_title = new UIText(64, m_messagefont);
			m_failbox_title->setText("FAILED");
			m_failbox_title->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_TOP);
			m_failbox_title->setPosition(400, 150);

			m_failbox_layer = new UILayer();
			m_failbox_layer->addItem(m_failbox_title);
			m_failbox_layer->addItem(m_failbox_tryagain);
			m_failbox_layer->addItem(m_failbox_quit);

			m_uilayer->addItem(m_failbox_layer);

			// ----------------------------------------------------------------


			// show/hide UI items based on affixes
			if (m_affixes.affixes & LEVEL_AFFIX_TIMELIMIT ||
				m_affixes.affixes & LEVEL_AFFIX_SPEEDRUN)
			{
				m_clockicon->enable();
			}
			else
			{
				m_clockicon->enable();
			}

			if (m_affixes.affixes & LEVEL_AFFIX_COLLECT1)
				m_collect1_text->enable();
			else
				m_collect1_text->disable();

			if (m_affixes.affixes & LEVEL_AFFIX_COLLECT2)
				m_collect2_text->enable();
			else
				m_collect2_text->disable();


			break;
		}

		case LOAD_PAUSE_SCREEN:
		{
			float center_x = Screen::getWidthFraction(0.5f);

			m_pausefont = new Font();
			m_pausefont->load("newfont1.font.xml", Screen::getHeightSlice(12));

			m_pause_title = new UIText(64, m_pausefont);
			m_pause_title->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_CENTER);
			m_pause_title->setText("GAME PAUSED");
			m_pause_title->setPosition(center_x, Screen::getHeightSlice(4.8f));			

			float button_w = Screen::getWidthSlice(3);
			float button_h = button_w / 4;			
			float button_sw = button_h / 2;

			m_pause_bg = new QuadMesh2(1);
			m_pause_bg->insert(glm::vec2(0, 0),
							   glm::vec2(Screen::getWidth(), 0),
							   glm::vec2(0, Screen::getHeight()),
							   glm::vec2(Screen::getWidth(), Screen::getHeight()),
							   m_vignette);

			m_pause_bgg = new UIStatic(0, 0, Screen::getWidth(), Screen::getHeight(), m_vignette);
			m_pause_bgg->setPosition(Screen::getWidthFraction(0.5f), Screen::getHeightFraction(0.5f));

			UITextButton::Def resume_button_def = { "button2mid", "button2left", "button2right", "button4mid", "button4left", "button4right" };
			UITextButton::Def quit_button_def = { "button3mid", "button3left", "button3right", "button4mid", "button4left", "button4right" };

			m_pause_resume = new UITextButton(GAMEUIID_PAUSE_RESUME, center_x, Screen::getHeightSlice(2), button_w, button_h, button_sw, button_sw, "Continue", m_buttonfont, m_introbox_texture, resume_button_def);
			m_pause_quit = new UITextButton(GAMEUIID_PAUSE_QUIT, center_x, Screen::getHeightSlice(1.5f), button_w, button_h, button_sw, button_sw, "Quit Level", m_buttonfont, m_introbox_texture, quit_button_def);

			m_pause_layer = new UILayer();
			m_pause_layer->addItem(m_pause_bgg);
			m_pause_layer->addItem(m_pause_title);
			m_pause_layer->addItem(m_pause_resume);
			m_pause_layer->addItem(m_pause_quit);

			m_uilayer->addItem(m_pause_layer);
			break;
		}

		case 98:
		{
			m_sensor_font = new Font();
			m_sensor_font->load("font10.font.xml", Screen::getHeightSlice(18));

			m_sensors = new UIText(300, m_sensor_font);
			m_sensors->setText("Accel: \nGyro: \nMag:  ");
			m_sensors->setPosition(40, 300);
			m_sensors->setAnchor(Text::ANCHOR_X_LEFT | Text::ANCHOR_Y_TOP);

			break;
		}

		case LOAD_FINISH:
		{
			reset();
			setGameState(GAMESTATE_INTRO);
			onUpdate(0);
			break;
		}
	}

	m_load_progress++;

	if (m_load_progress > 100)
		m_load_progress = 100;

	return m_load_progress;
}

void Engine::shutdown()
{
	SAFE_DELETE(m_vignette);
	SAFE_DELETE(m_dustcloud_tex);
	SAFE_DELETE(m_sparkle_tex);
	SAFE_DELETE(m_gameui_texture);

	SAFE_DELETE(m_coin_tex);
	SAFE_DELETE(m_goal_tex);
	SAFE_DELETE(m_level_texture);

	SAFE_DELETE(m_bountyhunter_set);
	SAFE_DELETE(m_bh_effect_set);
	SAFE_DELETE(m_tire_set);
	SAFE_DELETE(m_soccer_set);

	SAFE_DELETE(m_pause_bg);
	SAFE_DELETE(m_pausefont);
	SAFE_DELETE(m_uilayer);
	SAFE_DELETE(m_uiback);

	SAFE_DELETE(m_sensors);
	SAFE_DELETE(m_sensor_font);

	SAFE_DELETE(m_levelbgtex);

	// free dynamic objects
	std::list<DynamicObject*>::iterator dyn_it;
	for (dyn_it = m_dynobjs.begin(); dyn_it != m_dynobjs.end(); ++dyn_it)
	{
		DynamicObject* dyn = *dyn_it;
		delete dyn;
	}
	m_dynobjs.clear();

	// free sprites
	std::list<PolyAnim*>::iterator spr_it;
	for (spr_it = m_sprites.begin(); spr_it != m_sprites.end(); ++spr_it)
	{
		PolyAnim* sprite = *spr_it;
		delete sprite;
	}
	m_sprites.clear();

	// free particle gens
	std::list<ParticleGen*>::iterator part_it;
	for (part_it = m_particle_gen.begin(); part_it != m_particle_gen.end(); ++part_it)
	{
		ParticleGen* part = *part_it;
		delete part;
	}
	m_particle_gen.clear();

	// free movers
	std::list<MoverObject*>::iterator mover_it;
	for (mover_it = m_movers.begin(); mover_it != m_movers.end(); ++mover_it)
	{
		MoverObject* mover = *mover_it;
		delete mover;
	}
	m_movers.clear();

	SAFE_DELETE(m_world);
}

void Engine::reset()
{
	// TODO: reset all the destroyed/collected items!!!
	setPlayerPosition(m_player_start_pos);

	m_clock->reset();
	if (m_affixes.affixes & LEVEL_AFFIX_TIMELIMIT)
	{
		int mins = m_affixes.time_limit / 60;
		int secs = m_affixes.time_limit % 60;
		m_clock->set(mins, secs, 0);
	}

	m_player->reset();

	m_render_game = true;

	setFail(false, std::string("FAIL PLACEHOLDER"));

	m_player_angle = 0;

	m_winscreen_timer = 0;

	m_status = STATUS_RUNNING;
	
	m_vis_dynobjs.clear();
	m_destroy_list.clear();
}

void Engine::setShaders(const Shaders::GameShaderContext* default_shader,
						const Shaders::UIShaderContext* ui_shader,
						const Shaders::ParticleShaderContext* particle_shader)
{
	m_game_shader = default_shader;
	m_ui_shader = ui_shader;
	m_particle_shader = particle_shader;
}

/*
void Engine::onViewportChange(int width, int height)
{
	glViewport(0, 0, width, height);
	glScissor(0, 0, width, height);

	m_viewport_width = width;
	m_viewport_height = height;
	m_viewport_aspect = (float)(width) / (float)(height);
}
*/

Engine::Status Engine::getStatus()
{
	return m_status;
}

void Engine::reportMS(int framems)
{
	m_last_ms = framems;
}

void Engine::reportF2FMS(int ms)
{
	m_last_f2fms = ms;
}

void Engine::reportFPS(int fps)
{
	m_last_fps = fps;
}



void Engine::setFail(bool enable, const std::string& reason)
{
	setGameState(GAMESTATE_FAIL);

	m_failbox_title->setText(reason.c_str());
}

void Engine::setGameState(GameState gamestate)
{
	m_gamestate = gamestate;

	switch (gamestate)
	{
		case GAMESTATE_NORMAL:
		{
			m_intro_layer->disable();
			m_failbox_layer->disable();
			m_pause_layer->disable();
			m_win_layer->disable();
			
			m_uiback->enable();
			break;
		}
		case GAMESTATE_INTRO:
		{
			// TODO: setup screen?
			reset();

			m_intro_layer->enable();
			m_failbox_layer->disable();
			m_pause_layer->disable();
			m_win_layer->disable();

			m_uiback->disable();
			break;
		}
		case GAMESTATE_FAIL:
		{
			// TODO: setup fail message?

			m_intro_layer->disable();
			m_failbox_layer->enable();
			m_pause_layer->disable();
			m_win_layer->disable();

			m_uiback->disable();
			break;
		}
		case GAMESTATE_WIN:
		{
			// TODO: setup win message?

			m_intro_layer->disable();
			m_failbox_layer->disable();
			m_pause_layer->disable();
			m_win_layer->enable();

			m_uiback->disable();
			break;
		}
		case GAMESTATE_PAUSE:
		{
			m_intro_layer->disable();
			m_failbox_layer->disable();
			m_pause_layer->enable();
			m_win_layer->disable();

			m_uiback->disable();
			break;
		}
		case GAMESTATE_PLAYER_RESTORE:
		{
			m_intro_layer->disable();
			m_failbox_layer->disable();
			m_pause_layer->disable();

			m_uiback->disable();

			b2Vec2 pos = m_player_body->GetPosition();
			m_player_restore_start = glm::vec2(pos.x, pos.y);
			m_player_restore_end = m_player_start_pos;

			m_timer->add(TIMER_PLAYER_RESTORE, 2000, 0, TimerSystem::EVENT_EXPIRE);
			m_timer->add(TIMER_PLAYER_RESTORE_ANIM, 2000, 0, TimerSystem::EVENT_ANIMATION_UPDATE);
			break;
		}
	}
}


bool Engine::loadLevel(std::string filename)
{
	BinaryFile input;

	glm::vec2 pos_points[8];
	glm::vec2 uv_points[8];

	try
	{
		unsigned int id;

		input.open(filename);

		// HSLX ID
		id = input.read_dword();
		if (id != HSLX_ID)
		{
			throw ios_base::failure("Engine::loadLevel(): HSLX ID not found.");
		}

		// HSLX version
		int hslx_version = input.read_dword();
		if (hslx_version != HSLX_VERSION)
		{
			throw ios_base::failure("Engine::loadLevel(): unsupported HSLX version.");
		}

		// texture name
		char texname_array[256];
		char b = 0;
		int index = 0;
		do
		{
			b = input.read_byte();
			texname_array[index++] = b;
		} while (b != 0);

		std::string texname(texname_array);

		m_level_texture = new Texture();
		m_level_texture->load(texname);



		/*
		// load level bounds
		m_level_min_x = input.read_float();
		m_level_max_x = input.read_float();
		m_level_min_y = input.read_float();
		m_level_max_y = input.read_float();

		// level normalization value
		glm::vec2 level_norm = glm::vec2(-m_level_min_x, -m_level_min_y);

		m_level_min_x += level_norm.x;
		m_level_min_y += level_norm.y;
		m_level_max_x += level_norm.x;
		m_level_max_y += level_norm.y;


		m_geometry->init(m_level_min_x, m_level_max_x, m_level_min_y, m_level_max_y);


		// create level container body
		b2Vec2 physverts[4];	
		physverts[0].Set(m_level_min_x, m_level_min_y);
		physverts[1].Set(m_level_min_x, m_level_max_y);
		physverts[2].Set(m_level_max_x, m_level_max_y);
		physverts[3].Set(m_level_max_x, m_level_min_y);

		b2ChainShape shape;
		shape.CreateLoop(physverts, 4);

		b2BodyDef body_def;
		body_def.position.Set(0, 0);

		m_level_container_body = m_world->CreateBody(&body_def);
		m_level_container_body->CreateFixture(&shape, 0.0f);
		*/

		// num of objects
		int num_objects = input.read_dword();

		// load objects
		for (int i=0; i < num_objects; i++)
		{
			int object_type;

			object_type = input.read_dword();
			int num_points = input.read_dword();
			int object_z = input.read_dword();
			unsigned int color = input.read_dword();

			// load points
			for (int p=0; p < num_points; p++)
			{
				float x = input.read_float();
				float y = input.read_float();
				float u = input.read_float();
				float v = input.read_float();

				pos_points[p] = glm::vec2(x, y);
				uv_points[p] = glm::vec2(u, v);
			}


			// calculate center point
			float minx = pos_points[0].x;
			float maxx = minx;
			float miny = pos_points[0].y;
			float maxy = miny;

			glm::vec2 center_point;

			for (int p=1; p < num_points; p++)
			{
				float px = pos_points[p].x;
				float py = pos_points[p].y;
				if (px < minx)
					minx = px;
				if (px > maxx)
					maxx = px;
				if (py < miny)
					miny = py;
				if (py > maxy)
					maxy = py;					
			}

			center_point = glm::vec2(minx + ((maxx - minx) * 0.5f), miny + ((maxy - miny) * 0.5f));


			float z = ((float)(object_z) * 10.0f) + 1000.0f;


			// load parameters and insert object
			switch (object_type)
			{
				case OBJECT_TYPE_DESTRUCTIBLE:
				{
					insertDestructible(pos_points, uv_points, num_points, z);
					break;
				}
				case OBJECT_TYPE_TRIGGER:
				{
					int trigger_type = input.read_dword();
					float trigger_angle = input.read_float();
					int param1 = input.read_dword();
					int param2 = input.read_dword();

					switch (trigger_type)
					{
						case TRIGGER_TYPE_PLAYER_START:
						{
							m_player_start_pos = center_point;
							break;
						}
						case TRIGGER_TYPE_PARTICLE_GEN:
						{
							ParticleGen* gen = new ParticleGen();

							gen->position = center_point;
							gen->angle = trigger_angle;

							m_particle_gen.push_back(gen);
							break;
						}
						case TRIGGER_TYPE_COIN:
						{
							insertCollectable(center_point, COLLECTABLE_COIN);
							break;
						}
						case TRIGGER_TYPE_LEVEL_END:
						{
							setEndZone(center_point, pos_points, uv_points, num_points, m_goal_tex, z);
							break;
						}

						default:
							break;
					}
					
					break;
				}
				case OBJECT_TYPE_MOVER:
				{
					float angle = input.read_float();
					float axis_x = (float) -cos(angle * M_PI / 180.0);
					float axis_y = (float) -sin(angle * M_PI / 180.0);

					float length = input.read_float();
					float uptime = input.read_float();
					float downtime = input.read_float();
					float time = input.read_float();

					insertMover(pos_points, uv_points, num_points, glm::vec2(axis_x, axis_y), length, uptime, downtime, time, z);
					break;
				}

				default:
					throw ios_base::failure("Engine::loadLevel(): unknown object type.");
					break;
			}
		}

		// num of tiles
		int num_tiles = input.read_dword();

		// load tiles
		for (int i = 0; i < num_tiles; i++)
		{
			StaticGeometry::Tile tile;
			tile.color = input.read_dword();
			tile.type = (StaticGeometry::TileType)input.read_dword();

			int num_top_points = 0;

			switch (tile.type)
			{
				case StaticGeometry::TILE_FULL:			num_top_points = 6; break;
				case StaticGeometry::TILE_LEFT:			num_top_points = 4; break;
				case StaticGeometry::TILE_RIGHT:		num_top_points = 4; break;
				case StaticGeometry::TILE_TOP:			num_top_points = 3; break;
				case StaticGeometry::TILE_BOTTOM:		num_top_points = 3; break;
				case StaticGeometry::TILE_MID:			num_top_points = 4; break;
				case StaticGeometry::TILE_CORNER_BL:	num_top_points = 3; break;
				case StaticGeometry::TILE_CORNER_BR:	num_top_points = 3; break;
				case StaticGeometry::TILE_CORNER_TL:	num_top_points = 3; break;
				case StaticGeometry::TILE_CORNER_TR:	num_top_points = 3; break;
			}

			// top uvs
			for (int uv = 0; uv < num_top_points; uv++)
			{
				float u = input.read_float();
				float v = input.read_float();

				tile.top_uv[uv] = glm::vec2(u, v);
			}

			// side uvs
			for (int uv = 0; uv < 4; uv++)
			{
				float u = input.read_float();
				float v = input.read_float();

				tile.side_uv[uv] = glm::vec2(u, v);
			}

			tile.shading = input.read_dword();

			m_geometry->insertTile(tile);
		}

		// wall tilemap
		{
			int num_buckets = input.read_dword();
			for (int b = 0; b < num_buckets; b++)
			{
				StaticGeometry::WallBucket bucket;

				bucket.x = input.read_dword();
				bucket.y = input.read_dword();

				for (int m = 0; m < 64; m++)
				{
					bucket.map[m] = input.read_dword();
				}

				m_geometry->insertWallBucket(bucket);
			}
		}

		// floor tilemap
		{
			int num_buckets = input.read_dword();
			for (int b = 0; b < num_buckets; b++)
			{
				StaticGeometry::FloorBucket bucket;

				bucket.x = input.read_dword();
				bucket.y = input.read_dword();

				for (int m = 0; m < 64; m++)
				{
					bucket.map[m] = input.read_word();
				}

				m_geometry->insertFloorBucket(bucket);
			}
		}

		// number of edges
		int num_edges = input.read_dword();

		// edges
		{
			for (int e = 0; e < num_edges; e++)
			{
				int num_points = input.read_dword();
				b2Vec2* verts = new b2Vec2[num_points];
				
				for (int p = 0; p < num_points; p++)
				{
					int ep = input.read_dword();
					glm::vec2 cp = m_geometry->getPoint(ep);
					verts[p] = b2Vec2(cp.x, cp.y);
				}

				b2ChainShape shape;
				shape.CreateLoop(verts, num_points);

				b2BodyDef body_def;
				body_def.position.Set(0, 0);

				b2Body* body = m_world->CreateBody(&body_def);
				body->CreateFixture(&shape, 0.0f);

				m_edges.push_back(body);
			}
		}



		input.close();
	}
	catch (ios_base::failure failure)
	{
		Debug::log(failure.what());

		const char* error = failure.what();

		input.close();
		return false;
	}

	return true;
}


glm::vec2 Engine::calculateObjectCenter(const glm::vec2* p, int num_points)
{
	// calculate extents
	float minx = p[0].x;
	float miny = p[0].y;
	float maxx = minx;
	float maxy = miny;

	for (int i=0; i < num_points; i++)
	{
		if (p[i].x < minx)
			minx = p[i].x;
		if (p[i].x > maxx)
			maxx = p[i].x;

		if (p[i].y < miny)
			miny = p[i].y;
		if (p[i].y > maxy)
			maxy = p[i].y;
	}

	float centerx = minx + ((maxx - minx) / 2.0f);
	float centery = miny + ((maxy - miny) / 2.0f);

	return glm::vec2(centerx, centery);
}


void Engine::setPlayerPosition(const glm::vec2& position)
{
	DynamicObject* dynobj = static_cast<DynamicObject*>(m_player_body->GetUserData());

	dynobj->setPosition(position);
	m_player_body->SetTransform(b2Vec2(position.x, position.y), 0);
}


void Engine::insertPlayerObject(DynamicObject* object)
{
	float rad = GameParams::PLAYER_RADIUS;
	float z = 1000.0f;

	object->setPosition(0, 0);
	object->setAngle(0);
	object->setZ(z);	

	// body
	b2BodyDef player_def;
	player_def.type = b2_dynamicBody;
	player_def.position.Set(0, 0);
	player_def.bullet = true;
	player_def.userData = object;

	m_player_body = m_world->CreateBody(&player_def);

	b2CircleShape player_shape;
	player_shape.m_p.Set(0.0f, 0.0f);
	player_shape.m_radius = GameParams::PLAYER_RADIUS;

	b2FixtureDef player_fixture;
	player_fixture.shape = &player_shape;
	player_fixture.density = GameParams::PLAYER_MASS;
	player_fixture.friction = GameParams::PLAYER_FRICTION;
	player_fixture.restitution = GameParams::PLAYER_RESTITUTION;

	m_player_body->CreateFixture(&player_fixture);
}

void Engine::insertCollectable(glm::vec2 pos, CollectableType type)
{
	float width, height;

	PolyAnim* sprite = NULL;
	Texture* texture = NULL;

	float z = 100.0f;		// TODO?

	switch (type)
	{
		case COLLECTABLE_COIN:
		{
			width = GameParams::COIN_WIDTH;
			height = GameParams::COIN_HEIGHT;
			sprite = m_coin_sprite;
			texture = m_coin_tex;
			break;
		}
	}

	assert(sprite != NULL);
	assert(texture != NULL);
	
	DynamicObject* dyn = new DynamicObject(DynamicObject::COLLIDER_COLLECTABLE, sprite, texture->getAtlas());
	dyn->setPosition(0, 0);
	dyn->setAngle(0);
	dyn->setZ(z);

	m_dynobjs.push_back(dyn);

	// body
	b2BodyDef body_def;
	body_def.type = b2_kinematicBody;
	body_def.position.Set(pos.x, pos.y);
	body_def.userData = dyn;

	b2Body* body = m_world->CreateBody(&body_def);

	b2PolygonShape shape;
	shape.SetAsBox(width, height);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.isSensor = true;

	body->CreateFixture(&fixture);
}



void Engine::insertSlider(const glm::vec2* p, const glm::vec2* uv, int num_points, glm::vec2 axis, float z)
{
	glm::vec2 center = calculateObjectCenter(p, num_points);

	b2Vec2 physverts[8];
	glm::vec2 ppoints[8];
	
	for (int i=0; i < num_points; i++)
	{
		physverts[i].Set(p[i].x - center.x, p[i].y - center.y);
		ppoints[i] = glm::vec2(p[i].x - center.x, p[i].y - center.y);
	}

	PolyAnim* sprite = new PolyAnim(ppoints, uv, num_points, z);
	m_sprites.push_back(sprite);

	DynamicObject* dyn = new DynamicObject(DynamicObject::COLLIDER_NEUTRAL, sprite, m_level_texture->getAtlas());
	dyn->setPosition(center.x, center.y);
	dyn->setAngle(0);
	dyn->setZ(z);

	m_dynobjs.push_back(dyn);

	b2BodyDef bodydef;
	bodydef.type = b2_dynamicBody;
	bodydef.position.Set(center.x, center.y);
	bodydef.userData = dyn;
	bodydef.fixedRotation = true;

	b2PolygonShape shape;
	shape.Set(physverts, num_points);

	b2Body* body = m_world->CreateBody(&bodydef);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 200.0f;
	fixture.friction = 0.2f;
	fixture.restitution = 0.5f;

	body->CreateFixture(&fixture);

	// joint
	b2PrismaticJointDef joint;
	joint.Initialize(body, m_level_container_body, body->GetPosition(), b2Vec2(axis.x, axis.y));
	joint.enableLimit = false;
	joint.enableMotor= false;

	m_world->CreateJoint(&joint);
}


void Engine::insertMover(const glm::vec2* p, const glm::vec2* uv, int num_points, glm::vec2 axis,
						 float length, float uptime, float downtime, float time, float z)
{
	glm::vec2 center = calculateObjectCenter(p, num_points);

	b2Vec2 physverts[8];
	glm::vec2 ppoints[8];
	
	for (int i=0; i < num_points; i++)
	{
		physverts[i].Set(p[i].x - center.x, p[i].y - center.y);
		ppoints[i] = glm::vec2(p[i].x - center.x, p[i].y - center.y);
	}

	PolyAnim* sprite = new PolyAnim(ppoints, uv, num_points, z);
	m_sprites.push_back(sprite);

	DynamicObject* dyn = new DynamicObject(DynamicObject::COLLIDER_NEUTRAL, sprite, m_level_texture->getAtlas());
	dyn->setPosition(center.x, center.y);
	dyn->setAngle(0);
	dyn->setZ(z);

	m_dynobjs.push_back(dyn);

	b2BodyDef bodydef;
	bodydef.type = b2_kinematicBody;
	bodydef.position.Set(center.x, center.y);
	bodydef.userData = dyn;
	bodydef.gravityScale = 0.0f;
	bodydef.fixedRotation = true;

	b2PolygonShape shape;
	shape.Set(physverts, num_points);

	b2Body* body = m_world->CreateBody(&bodydef);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	body->CreateFixture(&fixture);


	MoverObject* mover = new MoverObject();
	mover->body = body;
	mover->uptime = (timeunit)(uptime * 1000000000.0f);
	mover->downtime = (timeunit)(downtime * 1000000000.0f);
	mover->movetime = (timeunit)(time * 1000000000.0f);
	mover->startpos = glm::vec2(center.x, center.y);
	mover->endpos = mover->startpos - (axis * length);
	mover->timer = 0;
	mover->state = MOVER_DOWN;
	mover->axis = axis;
	mover->length = glm::length(glm::vec2(mover->endpos - mover->startpos));

	m_movers.push_back(mover);
}



void Engine::insertDestructible(const glm::vec2* p, const glm::vec2* uv, int num_points, float z)
{
	glm::vec2 center = calculateObjectCenter(p, num_points);

	b2Vec2 physverts[8];
	glm::vec2 ppoints[8];
	
	for (int i=0; i < num_points; i++)
	{
		physverts[i].Set(p[i].x - center.x, p[i].y - center.y);
		ppoints[i] = glm::vec2(p[i].x - center.x, p[i].y - center.y);
	}

	PolyAnim* sprite = new PolyAnim(ppoints, uv, num_points, z);
	m_sprites.push_back(sprite);

	DynamicObject* dyn = new DynamicObject(DynamicObject::COLLIDER_DESTRUCTIBLE, sprite, m_level_texture->getAtlas());
	dyn->setPosition(center.x, center.y);
	dyn->setAngle(0);
	dyn->setZ(z);

	m_dynobjs.push_back(dyn);

	b2BodyDef bodydef;
	bodydef.type = b2_kinematicBody;
	bodydef.position.Set(center.x, center.y);
	bodydef.userData = dyn;

	b2PolygonShape shape;
	shape.Set(physverts, num_points);

	b2Body* body = m_world->CreateBody(&bodydef);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.isSensor = false;

	body->CreateFixture(&fixture);
}


void Engine::insertStaticCollision(const glm::vec2* p, const glm::vec2* uv, int num_points, float z)
{
	glm::vec2 center = calculateObjectCenter(p, num_points);

	b2Vec2 physverts[8];
	
	for (int i=0; i < num_points; i++)
	{
		physverts[i].Set(p[i].x - center.x, p[i].y - center.y);
	}

	b2BodyDef bodydef;
	b2PolygonShape shape;

	shape.Set(physverts, num_points);

	bodydef.position.Set(center.x, center.y);
	b2Body* body = m_world->CreateBody(&bodydef);
	body->CreateFixture(&shape, 0.0f);
}


void Engine::setEndZone(glm::vec2 position, const glm::vec2* p, const glm::vec2* uvs, int num_points, Texture* texture, float z)
{
	glm::vec2 points[8];
	b2Vec2 physverts[8];
	for (int i=0; i < num_points; i++)
	{
		points[i] = p[i] - position;
		physverts[i] = b2Vec2(points[i].x, points[i].y);
	}

	PolyAnim* sprite = new PolyAnim(points, uvs, num_points, z);
	m_sprites.push_back(sprite);

	DynamicObject* dyn = new DynamicObject(DynamicObject::COLLIDER_ENDZONE, sprite, texture->getAtlas());
	dyn->setPosition(position);
	dyn->setAngle(0);
	dyn->setZ(z);

	m_dynobjs.push_back(dyn);

	// body
	b2BodyDef body_def;
	body_def.type = b2_kinematicBody;
	body_def.position.Set(position.x, position.y);
	body_def.userData = dyn;

	b2Body* body = m_world->CreateBody(&body_def);

	b2PolygonShape shape;
	shape.Set(physverts, num_points);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.isSensor = true;

	body->CreateFixture(&fixture);
}





// Box2D visibility cull callback
bool Engine::ReportFixture(b2Fixture* fixture)
{
	b2Body* body = fixture->GetBody();

	body->SetAwake(true);

	b2BodyType type = body->GetType();

	if (type == b2_dynamicBody || type == b2_kinematicBody)
	{
		b2Vec2 pos = body->GetPosition();
		float angle = body->GetAngle();
		float angular_velocity = body->GetAngularVelocity();
		b2Vec2 velocity = body->GetLinearVelocity();

		angle = angle / M_PI * 180.0f;

		DynamicObject* dyn = (DynamicObject*)body->GetUserData();

		dyn->setPosition(pos.x, pos.y);
		dyn->setAngle(angle);
		dyn->setAngularVelocity(angular_velocity);
		dyn->setVelocity(glm::vec2(velocity.x, velocity.y));
		dyn->setGravityAngle(m_player_angle);

		m_vis_dynobjs.push_back(dyn);
	}

	return true;
}

// Box2D begin contact callback
void Engine::BeginContact(b2Contact* contact)
{
	// only handle if we haven't reached the goal yet!
	if (m_gamestate == GAMESTATE_NORMAL)
	{
		b2Body* body_a = contact->GetFixtureA()->GetBody();
		b2Body* body_b = contact->GetFixtureB()->GetBody();

		DynamicObject::ColliderType collider_a = DynamicObject::COLLIDER_NEUTRAL;
		DynamicObject::ColliderType collider_b = DynamicObject::COLLIDER_NEUTRAL;

		b2BodyType type_a = body_a->GetType();
		b2BodyType type_b = body_b->GetType();

		if (type_a == b2_dynamicBody || type_a == b2_kinematicBody)
		{
			collider_a = ((DynamicObject*)body_a->GetUserData())->getColliderType();
		}
		if (type_b == b2_dynamicBody || type_b == b2_kinematicBody)
		{
			collider_b = ((DynamicObject*)body_b->GetUserData())->getColliderType();
		}


		if (collider_b == DynamicObject::COLLIDER_PLAYER)
		{
			// swap
			b2Body* tmpb = body_a;
			body_a = body_b;
			body_b = tmpb;

			DynamicObject::ColliderType tmpt = collider_a;
			collider_a = collider_b;
			collider_b = tmpt;
		}
		
		// collider A should now be player, skip if not
		if (collider_a != DynamicObject::COLLIDER_PLAYER)
		{
			return;
		}

		switch (collider_b)
		{
			case DynamicObject::COLLIDER_COLLECTABLE:
			{
				b2Vec2 position = body_b->GetPosition();

				// mark for removal
				m_destroy_list.push_back(body_b);

#if ENABLE_PARTICLES
				ParticleSys2::Emitter emitter;
				emitter.position = glm::vec2(position.x, position.y);
				emitter.angle = 270.0f;
				emitter.angle_range = 10.0f;
				emitter.width = 1.0f;
				emitter.height = 1.0f;
				emitter.particle_speed = 4.0f;
				emitter.particle_speed_range = 2.5f;
				emitter.time = 1000;
				emitter.time_range = 400;
				m_sparkles->setEmitter(emitter);

				m_sparkles->emit(15);
#endif

				// add points
				// TODO COLLECTABLE TYPE?
				m_player->add_points(100);
				break;
			}
		
			case DynamicObject::COLLIDER_ENDZONE:
			{
				setGameState(GAMESTATE_WIN);

				//m_ui->addUIEffect(GameUI::UI_COURSE_COMPLETE);
				m_winscreen_timer = WINSCREEN_ANIM_DURATION;
	
				// BIG TODO DOOT DOOT
				/*
				m_ui_winscreen->setPosition(0.0f, -(float)(Screen::getHeight()));
	
				m_ui_winscore->setText(str(fmt::Format("SCORE: {:d}") << m_player->get_score()).c_str());
	
				const std::string& curr_time = m_clock->format();
				m_ui_wintime->setText(str(fmt::Format("TIME: {:s}") << curr_time).c_str());

				// start animation
				m_timer->add(TIMER_WINSCREEN_IN_ANIM, WINSCREEN_ANIM_DURATION, 0, TimerSystem::EVENT_ANIMATION_UPDATE);
				m_timer->add(TIMER_WINSCREEN_IN, WINSCREEN_ANIM_DURATION, 0, TimerSystem::EVENT_EXPIRE);
				
				// hide continue button until anim is done
				m_ui_wincontinue->disable();
				*/

				// make sparkle animation
#if ENABLE_PARTICLES
				b2Vec2 goal_position = body_b->GetPosition();

				ParticleSys2::Emitter emitter;
				emitter.position = glm::vec2(goal_position.x, goal_position.y);
				emitter.angle = 270.0f;
				emitter.angle_range = 180.0f;
				emitter.width = 1.0f;
				emitter.height = 1.0f;
				emitter.particle_speed = 8.0f;
				emitter.particle_speed_range = 5.5f;
				emitter.time = 5000;
				emitter.time_range = 4000;
				m_sparkles->setEmitter(emitter);
				
				m_sparkles->emit(95);
#endif
				break;
			}

			case DynamicObject::COLLIDER_DESTRUCTIBLE:
			{
				b2WorldManifold world_manifold;
				contact->GetWorldManifold(&world_manifold);

				b2Vec2 point = world_manifold.points[0];
				b2Vec2 va = body_a->GetLinearVelocityFromWorldPoint(point);
				b2Vec2 vb = body_b->GetLinearVelocityFromWorldPoint(point);
				b2Vec2 vc = vb - va;
				float32 approach_velocity = b2Dot(vc, world_manifold.normal);
			
				// break if impact is big enough
				if (approach_velocity > 15.0f)
				{
					// mark for removal
					m_destroy_list.push_back(body_b);

					// TODO FIX
					// TODO FIX
					// TODO FIX

					/*
					b2Vec2 sprite_position = body_b->GetPosition();
					PolySprite* sprite = ((DynamicObject*)body_b->GetUserData())->sprite;				

#if ENABLE_PARTICLES
					const glm::vec2* sprite_points = sprite->getPoints();
					int num_sprite_points = sprite->getNumPoints();

					ParticleSys2::Emitter emitter;
					emitter.position = glm::vec2(sprite_position.x, sprite_position.y);
					emitter.angle = 0.0f;
					emitter.angle_range = 360.0f;
					emitter.particle_speed = 0.5f;
					emitter.particle_speed_range = 0.25f;
					emitter.time = 1000;
					emitter.time_range = 600;
					m_dustcloud->setEmitter(emitter);
					m_dustcloud->emitArea(sprite_points, num_sprite_points, 100);
#endif
					*/
				}
				break;
			}

			case DynamicObject::COLLIDER_NEUTRAL:			// Player collision vs. world geometry
			{
				b2WorldManifold world_manifold;
				contact->GetWorldManifold(&world_manifold);

				b2Vec2 point = world_manifold.points[0];
				b2Vec2 va = body_a->GetLinearVelocityFromWorldPoint(point);
				b2Vec2 vb = body_b->GetLinearVelocityFromWorldPoint(point);
				b2Vec2 vc = vb - va;
				float32 approach_velocity = b2Dot(vc, world_manifold.normal);
			
				// spawn dust if the impact is big enough
				if (approach_velocity > 15.0f)
				{
#if ENABLE_PARTICLES
					float angle = (float)(atan2(-va.y, -va.x) / M_PI * 180.0f);

					ParticleSys2::Emitter emitter;
					emitter.position = glm::vec2(point.x, point.y);
					emitter.angle = angle;
					emitter.angle_range = 30.0f;
					emitter.width = 1.0;
					emitter.height = 1.0f;
					emitter.particle_speed = 2.0f;
					emitter.particle_speed_range = 0.5f;
					emitter.time = 800;
					emitter.time_range = 300;
					m_dustcloud->setEmitter(emitter);
					m_dustcloud->emit(10);
#endif
				}			
			}

			default:
				break;
		}
	}
}

void Engine::onRender()
{
	assert(m_game_shader != NULL);
	assert(m_ui_shader != NULL);
	assert(m_particle_shader != NULL);

	if (m_render_game)
	{
		// sort dynamic objects by z
		m_vis_dynobjs.sort([](DynamicObject* obja, DynamicObject* objb ){ return objb->getZ() < obja->getZ(); });

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glViewport(0, 0, Screen::getWidth(), Screen::getHeight());
		glScissor(0, 0, Screen::getWidth(), Screen::getHeight());

		glClearDepthf(1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUseProgram(m_game_shader->shader);


		// screen half width and height in game units
		float halfw = GameParams::VISIBLE_LEVEL_WIDTH * 0.5f;
		float halfh = (GameParams::VISIBLE_LEVEL_WIDTH / Screen::getAspect()) * 0.5f;

		float scrollx = -m_scrollpos_x + halfw;
		float scrolly = -m_scrollpos_y + halfh;


		// setup matrices
		float level_vp_mat[4] = {1.0f / halfw, 0.0f,
								 0.0f, 1.0f / halfh };
		glUniformMatrix2fv(m_game_shader->vp_matrix, 1, false, level_vp_mat);

		float level_rot_mat[4] = {1.0f, 0.0f,
								  0.0f, 1.0f};
		glUniformMatrix2fv(m_game_shader->rot_matrix, 1, false, level_rot_mat);

		// render playfield
		m_level_texture->apply(0);

		glUniform2f(m_game_shader->location, scrollx, scrolly);
		glUniform2f(m_game_shader->scale, 1.0f, 1.0f);
		glUniform4f(m_game_shader->color, 1.0f, 1.0f, 1.0f, 1.0f);

		m_geometry->render(m_game_shader);


		// render background
		m_levelbgtex->apply(0);

		float bgxint;
		float bgxfrac = modf((scrollx/2.0f) / m_bg_width, &bgxint);
		//float bgx = bgxfrac * (float)(GameParams::VISIBLE_LEVEL_WIDTH);
		float bgx = bgxfrac * m_bg_width;

		float bgyint;
		float bgyfrac = modf((scrolly/2.0f) / m_bg_height, &bgyint);
		float bgy = bgyfrac * m_bg_height;

		//glUniform2f(m_game_shader->location, bgx + m_bg_width, -1.0f);
		glUniform2f(m_game_shader->location, halfw, halfh);
		glUniform2f(m_game_shader->scale, 1.0f, 1.0f);

		float bg_pulse = 0.8f + (cos((m_timer->getHzTime() * 360.0f / 5.0f) * M_PI / 180.0f) * 0.2f);
		glUniform4f(m_game_shader->color, bg_pulse, bg_pulse, bg_pulse, 1.0f);

		glUniform2f(m_game_shader->location, bgx + m_bg_width, bgy);

#if ROTATION_BG_TEST
		{
			float rot_mat[4];
			float ca = cos(m_player_angle * M_PI / 180.0f);
			float sa = sin(m_player_angle * M_PI / 180.0f);
			rot_mat[0] = ca;
			rot_mat[1] = sa;
			rot_mat[2] = -sa;
			rot_mat[3] = ca;

			glUniformMatrix2fv(m_game_shader->rot_matrix, 1, false, rot_mat);
		}
#endif

		m_levelbg->render(m_game_shader);


		// disable depth writes, since dynamic objects are sorted
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);

		glUniform4f(m_game_shader->color, 1.0f, 1.0f, 1.0f, 1.0f);

		// render dynamic objects
		std::list<DynamicObject*>::iterator dyn_it = m_vis_dynobjs.begin();
		while (dyn_it != m_vis_dynobjs.end())
		{
			float rot_mat[4];
			DynamicObject* dyn = *dyn_it;

			glUniform2f(m_game_shader->location, scrollx + dyn->getX(), scrolly + dyn->getY());
			glUniform2f(m_game_shader->scale, 1.0f, 1.0f);
			dyn->render(m_game_shader);

			++dyn_it;
		}



		// render particle systems
#if ENABLE_PARTICLES
		glDisable(GL_DEPTH_TEST);
		glUseProgram(m_particle_shader->shader);
		glUniformMatrix2fv(m_particle_shader->vp_matrix, 1, false, level_vp_mat);
		glUniform2f(m_particle_shader->location, scrollx + 0.0f, scrolly + 0.0f);

		m_dustcloud->render(m_particle_shader);
		m_sparkles->render(m_particle_shader);
#endif




		// render HUD
		// ------------------------------------------------------
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(m_ui_shader->shader);

		float ui_vp_mat[4] = {1.0f / Screen::getWidthFraction(0.5f), 0.0f, 0.0f, 1.0f / Screen::getHeightFraction(0.5f)};
		glUniformMatrix2fv(m_ui_shader->vp_matrix, 1, false, ui_vp_mat);

		float ui_rot_mat[4] = {1.0f, 0.0f, 0.0f, 1.0};
		glUniformMatrix2fv(m_ui_shader->rot_matrix, 1, false, ui_rot_mat);

		// render interactive UI
		glUniform4f(m_ui_shader->color, 1.0f, 1.0f, 1.0f, 1.0f);
		m_uilayer->onRender(m_ui_shader, 0.0f, 0.0f);


		// TODO REMOVE
		m_sensors->onRender(m_ui_shader, 0.0f, 0.0f);
	}
	else		// win screen fade out background
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		glUseProgram(m_ui_shader->shader);

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClearDepthf(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	// render win screen
	// TODO !!!
	//if (m_goal_reached)
	//{
	//	m_ui_winscreen->onRender(m_ui_shader, 0.0f, 0.0f);
	//}

	// TODO !?
	/*
	if (m_paused)
	{
		float ui_vp_mat[4] = {1.0f / Screen::getWidthFraction(0.5f), 0.0f, 0.0f, 1.0f / Screen::getHeightFraction(0.5f)};		
		glUniformMatrix2fv(m_ui_shader->vp_matrix, 1, false, ui_vp_mat);

		float ui_rot_mat[4] = {1.0f, 0.0f, 0.0f, 1.0};
		glUniformMatrix2fv(m_ui_shader->rot_matrix, 1, false, ui_rot_mat);

		glUniform2f(m_ui_shader->location, 0.0f, 0.0f);
		glUniform2f(m_ui_shader->scale, 1.0f, 1.0f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glActiveTexture(GL_TEXTURE0);
		m_vignette->apply(0);

		m_pause_bg->render(m_ui_shader);

		glDisable(GL_BLEND);
	}
	*/

	// TESTBOX
#if 0
	{
		//TextureManager::Atlas* shadow = TextureManager::findAtlas("static_shadow");
		Texture* shadow = m_renderer->findTexture("coin100.png");

		if (shadow)
		{
			glUniform2f(m_ui_shader->location, 0, 128);
			glUniform2f(m_ui_shader->scale, 1.0f, 1.0f);
			shadow->apply(0);
			m_testbox->render(m_ui_shader);
		}
	}
#endif

#if 0
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		char str[100];
		sprintf_s(str, "ERROR: %d", error);
		const GLubyte* es = glewGetErrorString(error);
		m_error_string = str;
	}
#endif
}


void Engine::onUIEvent(int itemid, UI::UIItemEvent eventid)
{
	switch (itemid)
	{
		case GAMEUIID_BACK:
		{
			setGameState(GAMESTATE_PAUSE);
			break;
		}

		case GAMEUIID_PAUSE_RESUME:
		{
			setGameState(GAMESTATE_NORMAL);
			break;
		}

		case GAMEUIID_PAUSE_QUIT:
		{
			m_status = STATUS_EXIT;
			break;
		}

		case GAMEUIID_WIN_CONTINUE:
		{
			m_timer->add(TIMER_WINSCREEN_OUT, WINSCREEN_ANIM_DURATION, 0, TimerSystem::EVENT_EXPIRE);
			m_timer->add(TIMER_WINSCREEN_OUT_ANIM, WINSCREEN_ANIM_DURATION, 0, TimerSystem::EVENT_ANIMATION_UPDATE);

			// hide game rendering
			m_render_game = false;
			break;
		}

		case GAMEUIID_WIN_QUIT:
		{
			m_status = STATUS_EXIT;
			break;
		}

		case GAMEUIID_INTRO_START:
		{
			setGameState(GAMESTATE_NORMAL);
			break;
		}

		case GAMEUIID_FAIL_TRYAGAIN:
		{
			setGameState(GAMESTATE_PLAYER_RESTORE);
			break;
		}

		case GAMEUIID_FAIL_QUIT:
		{
			m_status = STATUS_EXIT;
			break;
		}
	}
}


void Engine::onTimerEvent(int id, timeunit count, timeunit duration)
{
	switch (id)
	{
		case TIMER_WINSCREEN_IN_ANIM:
		{
			timeunit timeleft = duration - count;
			if (timeleft <= 1000000000)
			{
				/*
				float phase = (float)(1000000000 - timeleft) / 1000000000.0f;
				float y = (sin((phase * 90.0f) * M_PI / 180.0f) * (float)(m_viewport_height)) - m_viewport_height;

				m_ui_winscreen->setPosition(0.0f, y);
				*/

				float phase = (float)(1000000000 - timeleft) / 1000000000.0f;

//				m_ui_winscreen->setPosition(0.0f, 0.0f);
//				m_ui_winscreen->updateTransition(phase, UITransition::STATE_IN);
			}
			break;
		}

		case TIMER_WINSCREEN_IN:
		{
//			m_ui_winscreen->updateTransition(0, UITransition::STATE_OFF);
//			m_ui_wincontinue->enable();
			break;
		}

		case TIMER_WINSCREEN_OUT_ANIM:
		{
			/*
			float y;
			if (count <= 1000000000)
			{
				float phase = (float)(count) / 1000000000.0f;
				y = (sin((phase * 90.0f) * M_PI / 180.0f) * (float)(m_viewport_height));
			}
			else
			{
				y = m_viewport_height + 10;
			}

			m_ui_winscreen->setPosition(0.0f, y);
			*/

			float phase = (float)(count) / 1000000000.0f;
//			m_ui_winscreen->updateTransition(phase, UITransition::STATE_OUT);
			break;
		}

		case TIMER_WINSCREEN_OUT:
		{
			m_status = STATUS_EXIT;
			break;
		}

		case TIMER_PARTICLE_SPAWN:
		{
/*
			// update particle gens		// TODO change to world objects
			int num_partgens = m_particle_gen.size();
			for (std::list<ParticleGen*>::iterator it=m_particle_gen.begin(); it != m_particle_gen.end(); ++it)
			{
				ParticleGen* gen = *it;

				ParticleSys2::Emitter emitter;
				emitter.position = gen->position;
				emitter.angle = gen->angle;
				emitter.angle_range = 5.0f;
				emitter.width = 1.0f;
				emitter.height = 1.0f;
				emitter.particle_speed = 9.0f;
				emitter.particle_speed_range = 2.5f;
				emitter.time = 5000;
				emitter.time_range = 1000;
				m_sparkles->setEmitter(emitter);

				m_sparkles->emit(10);
			}
*/

			break;
		}

		case TIMER_PLAYER_RESTORE:
		{
			setGameState(GAMESTATE_INTRO);
			break;
		}

		case TIMER_PLAYER_RESTORE_ANIM:
		{
			float phase = (float)count / (float)duration;
			m_player_restore_pos = m_player_restore_start + ((m_player_restore_end - m_player_restore_start) * phase);
			break;
		}

		default:
			break;
	}
}

void Engine::updateMovers(int time)
{
	std::list<MoverObject*>::iterator it;
	for (it = m_movers.begin(); it != m_movers.end(); ++it)
	{
		MoverObject* obj = *it;

		obj->timer += time;

		switch (obj->state)
		{
			case MOVER_DOWN:
			{
				timeunit diff_time = obj->downtime - obj->timer;
				if (diff_time < time)
				{
					obj->timer = 0;
					obj->body->SetTransform(b2Vec2(obj->startpos.x, obj->startpos.y), 0);
					obj->state = MOVER_GOING_UP;

					float l = obj->length * (1000000000.0f / (float)(obj->movetime));
					glm::vec2 vel = obj->axis * -l;
					obj->body->SetLinearVelocity(b2Vec2(vel.x, vel.y));					
				}
				break;
			}
			case MOVER_UP:
			{
				timeunit diff_time = obj->uptime - obj->timer;
				if (diff_time < time)
				{
					obj->timer = 0;
					obj->body->SetTransform(b2Vec2(obj->endpos.x, obj->endpos.y), 0);
					obj->state = MOVER_GOING_DOWN;

					float l = obj->length * (1000000000.0f / (float)(obj->movetime));
					glm::vec2 vel = obj->axis * l;
					obj->body->SetLinearVelocity(b2Vec2(vel.x, vel.y));
				}
				break;
			}
			case MOVER_GOING_UP:
			{
				timeunit diff_time = obj->movetime - obj->timer;
				if (diff_time < time)
				{
					obj->timer = 0;
					obj->body->SetTransform(b2Vec2(obj->endpos.x, obj->endpos.y), 0);
					obj->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
					obj->state = MOVER_UP;
				}
				break;
			}
			case MOVER_GOING_DOWN:
			{
				timeunit diff_time = obj->movetime - obj->timer;
				if (diff_time < time)
				{
					obj->timer = 0;
					obj->body->SetTransform(b2Vec2(obj->startpos.x, obj->startpos.y), 0);
					obj->body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
					obj->state = MOVER_DOWN;
				}
				break;
			}
		}
	}
}


void Engine::onUpdate(int time)
{
	// do nothing if we're paused...
	if (m_gamestate == GAMESTATE_PAUSE)
		return;

	m_timer->update(time);

	float gx = 0.0f;
	float gy = 0.0f;

	switch (m_gamestate)
	{
		case GAMESTATE_NORMAL:
		{
			// if speedrun, add time
			if (m_affixes.affixes & LEVEL_AFFIX_SPEEDRUN)
			{
				m_clock->add(time);
			}

			// if timelimit, subtract time
			if (m_affixes.affixes & LEVEL_AFFIX_TIMELIMIT)
			{
				bool expired = m_clock->sub(time);
				if (expired)
				{
					setFail(true, std::string("TIME OUT!"));
				}
			}

			// update clock display
			if (m_affixes.affixes & (LEVEL_AFFIX_TIMELIMIT | LEVEL_AFFIX_SPEEDRUN))
			{
				//m_ui->setClock(m_clock->format());
				m_clocktext->setText(m_clock->format().c_str());

				// if time limit remaining is below 5 seconds, fade red
				if (m_affixes.affixes & LEVEL_AFFIX_TIMELIMIT)
				{
					timeunit remaining_time = m_clock->get();
					if (remaining_time < 5000)
					{
						int phase = (int)(remaining_time) % 1000;
						float timer_color_fade = (float)(phase) / 1000.0f;
						timer_color_fade = fabs(cosf(timer_color_fade * M_PI / 0.5f));
						m_clocktext->setColor(1.0f, timer_color_fade, timer_color_fade, 1.0f);
					}
					else
					{
						m_clocktext->setColor(1.0f, 1.0f, 1.0f, 1.0f);
					}
				}
			}

			// update interactive UI
			m_uilayer->onUpdate(time);

			updateMovers(time);

			// update physics
			float gf = 100.0f;
			gx = (float) sin(m_player_angle * M_PI / 180.0f) * gf;
			gy = (float) cos(m_player_angle * M_PI / 180.0f) * gf;
	
			b2Vec2 gravity_vector;
			gravity_vector = b2Vec2(gx, gy);
			
			//gravity_vector = b2Vec2(0.0f, gf);		// always down grav after goal

			m_world->SetGravity(gravity_vector);
			m_world->Step(1.0f/60.0f, 6, 2);

			// calculate screen scroll position
			b2Vec2 pos = m_player_body->GetPosition();
			m_player_pos = glm::vec2(pos.x, pos.y);
			break;
		}

		case GAMESTATE_WIN:
		{
			// calculate screen scroll position
			b2Vec2 pos = m_player_body->GetPosition();
			m_player_pos = glm::vec2(pos.x, pos.y);
			break;
		}

		case GAMESTATE_FAIL:
		{
			// calculate screen scroll position
			b2Vec2 pos = m_player_body->GetPosition();
			m_player_pos = glm::vec2(pos.x, pos.y);
			break;
		}

		case GAMESTATE_INTRO:
		{
			// calculate screen scroll position
			b2Vec2 pos = m_player_body->GetPosition();
			m_player_pos = glm::vec2(pos.x, pos.y);
			break;
		}

		case GAMESTATE_PAUSE:
		{
			// calculate screen scroll position
			b2Vec2 pos = m_player_body->GetPosition();
			m_player_pos = glm::vec2(pos.x, pos.y);
			break;
		}

		case GAMESTATE_PLAYER_RESTORE:
		{
			m_player_pos = m_player_restore_pos;
			break;
		}
	}

	// screen half width and height for scrolling
	float halfw = GameParams::VISIBLE_LEVEL_WIDTH * 0.5f;
	float halfh = (GameParams::VISIBLE_LEVEL_WIDTH / Screen::getAspect()) * 0.5f;

	m_scrollpos_x = m_player_pos.x;
	if (m_scrollpos_x < (m_level_min_x + halfw))
		m_scrollpos_x = m_level_min_x + halfw;
	if (m_scrollpos_x > (m_level_max_x - halfw))
		m_scrollpos_x = m_level_max_x - halfw;

	m_scrollpos_y = m_player_pos.y;
	if (m_scrollpos_y < (m_level_min_y + halfh))
		m_scrollpos_y = m_level_min_y + halfh;
	if (m_scrollpos_y > (m_level_max_y - halfh))
		m_scrollpos_y = m_level_max_y - halfh;


	// calculate visible buckets
	m_geometry->update(m_scrollpos_x - halfw, m_scrollpos_x + halfw, m_scrollpos_y - halfh, m_scrollpos_y + halfh);

	// TODO REMOVE
	int num_buckets, bucket_tris;
	m_geometry->get_stats(&num_buckets, &bucket_tris);



	// destroy physics bodies that need destroying
	std::list<b2Body*>::iterator body_iterator = m_destroy_list.begin();
	while (body_iterator != m_destroy_list.end())
	{
		b2Body* body = *body_iterator;
		m_world->DestroyBody(body);
	}
	m_destroy_list.clear();


	// find visible objects
	b2AABB queryaabb;
	queryaabb.lowerBound.Set(m_scrollpos_x - halfw, m_scrollpos_y - halfh);
	queryaabb.upperBound.Set(m_scrollpos_x + halfw, m_scrollpos_y + halfh);
	m_vis_dynobjs.clear();
	m_world->QueryAABB(this, queryaabb);


	// update visible objects
	std::list<DynamicObject*>::iterator dyn_it = m_vis_dynobjs.begin();
	while (dyn_it != m_vis_dynobjs.end())
	{
		DynamicObject* dyn = *dyn_it;
		dyn->update(time);
		++dyn_it;
	}


	// update particles
#if ENABLE_PARTICLES
	glm::vec2 particle_gravity = glm::vec2(gx, gy) * 0.1f;
	m_dustcloud->setGravity(particle_gravity);
	m_dustcloud->update(time);
	m_sparkles->setGravity(particle_gravity);
	m_sparkles->update(time);
#endif


	// diagnostics
	int parts = 0;
	parts += m_dustcloud->getNumActive();
	parts += m_sparkles->getNumActive();

	float x = m_player_pos.x;
	float y = m_player_pos.y;
	float a = m_player_body->GetAngle();
	float aa = m_player_body->GetAngle();


	
	/*
	m_sensors->setText(str(fmt::Format("Angle: {:f}\nPX: {:f}, PY: {:f}\nX: {:f}, Y: {:f}, A: {:f}\nScrollX: {:f}, Y: {:f}\nVis bodies: {:d}, particles: {:d}\nBuckets: {:d}, Bucket tris: {:d}\n, MS: {:d}, FPS: {:d}")
		<< m_player_angle
		<< gx << gy
		<< x << y << a
		<< m_scrollpos_x << m_scrollpos_y
		<< m_visbodies << parts
		<< num_buckets << bucket_tris
		<< m_last_ms << m_last_fps
		).c_str());
	*/
	
	m_sensors->setText(str(fmt::Format("Angle: {:f}\nVis bodies: {:d}, particles: {:d}\nBuckets: {:d}, Bucket tris: {:d}\nPlayer angle: {:f}\nMS: {:d}, F2FMS: {:d}, FPS: {:d}")
		<< m_player_angle
		<< m_vis_dynobjs.size() << parts
		<< num_buckets << bucket_tris
		<< aa 
		<< m_last_ms << m_last_f2fms << m_last_fps
		).c_str());
		
}

void Engine::onTouch(UI::UITouchEvent type, float x, float y)
{
	switch (m_gamestate)
	{
		case GAMESTATE_NORMAL:
		case GAMESTATE_WIN:
		case GAMESTATE_FAIL:
		case GAMESTATE_INTRO:
		case GAMESTATE_PAUSE:
			m_uilayer->onTouch(type, x, y);
			break;
	}
}

void Engine::setInputAngle(float angle)
{
	switch (m_gamestate)
	{
		// handle game controls if we haven't reached the goal and we're not in pause screen
		case GAMESTATE_NORMAL:
			m_player_angle = angle;
			break;
	}
}
