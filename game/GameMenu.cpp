#include "GameMenu.h"

GameMenu::GameMenu()
{
	m_main_start = NULL;
	m_main_settings = NULL;
	m_main_quit = NULL;
	m_main_slidertest = NULL;
	m_main_uibg = NULL;

	m_main_menu = NULL;
	m_main_layer = NULL;
}

GameMenu::~GameMenu()
{

}

void GameMenu::init(GameMenu::Page page)
{
	float center_x = Screen::getWidthFraction(0.5f);
	float center_y = Screen::getHeightFraction(0.5f);

	m_main_layer = new UILayer();
	m_main_layer->setEventCallback(this);
	m_main_layer->setPosition(0, 0);

	m_menutexts = new TextureSet();
	m_menutexts->load("menutexts.atlas.xml");

	m_gameui_assets = new TextureSet();
	m_gameui_assets->load("gameui.atlas.xml");
	
	m_main_bg = new Texture();
	m_main_bg->load("blue_checker.png");

	m_menubuttons = new TextureSet();
	m_menubuttons->load("newui_buttons.atlas.xml");

	m_menu_bg = new UIStatic(center_x, center_y, Screen::getWidth(), Screen::getHeight(), m_main_bg);

	float menu_width = Screen::getWidthSlice(2.5f);

	m_main_menu = new UIMenu(center_x - (menu_width / 2), Screen::getHeightSlice(12));
	m_main_menu->setSpacing(3.0f);
	m_main_layer->addItem(m_main_menu);

	Texture* button1_up = m_menutexts->findTexture("button1_up");
	Texture* button2_up = m_menutexts->findTexture("button2_up");
	Texture* button3_up = m_menutexts->findTexture("button3_up");
	Texture* button1_down = m_menutexts->findTexture("button1_down");
	Texture* button2_down = m_menutexts->findTexture("button2_down");
	Texture* button3_down = m_menutexts->findTexture("button3_down");

	Texture* button_bg1mid = m_menubuttons->findTexture("button1");
	Texture* button_bg1dn = m_menubuttons->findTexture("button4");
	Texture* button_bg2 = m_menubuttons->findTexture("button2");

	UITextButton::Def button1_def = { "button1mid", "button1left", "button1right", "button1bmid", "button1bleft", "button1bright" };
	UITextButton::Def button2_def = { "button2mid", "button2left", "button2right", "button4mid", "button4left", "button4right" };
	
	// main menu

	m_test_font = new Font();
	m_test_font->load("newfont1.font.xml", Screen::getHeightSlice(12));
//	m_main_textbutton	= new UITextButton(256, 64, "Textbutton", m_test_font, m_main_bg);

	/*
	m_main_start			= new UIButton(MENUID_MAIN_START,		256, 64, button1_up, button1_down); 
	m_main_settings		= new UIButton(MENUID_MAIN_SETTINGS,	256, 64, button2_up, button2_down);
	m_main_quit			= new UIButton(MENUID_MAIN_QUIT,		256, 64, button3_up, button3_down);
	m_main_slidertest	= new UISlider(256, 64, 0, 5, m_menutexts, "slider_frame", "slider_bar");
	*/

	float menu_item_height = Screen::getHeightSlice(7.5f);
	float menu_item_side = menu_item_height / 2.0f;

	m_main_start		= new UITextButton(MENUID_MAIN_START,	 0, 0, menu_width*2, menu_item_height, menu_item_side, menu_width, "Start Game", m_test_font, m_menubuttons, button1_def);
	m_main_howto		= new UITextButton(-444,					 0, 0, menu_width*2, menu_item_height, menu_item_side, menu_width, "How to Play", m_test_font, m_menubuttons, button1_def);
	m_main_settings = new UITextButton(MENUID_MAIN_SETTINGS, 0, 0, menu_width*2, menu_item_height, menu_item_side, menu_width, "Settings", m_test_font, m_menubuttons, button1_def);
	m_main_scores	= new UITextButton(-555,					 0, 0, menu_width*2, menu_item_height, menu_item_side, menu_width, "High Scores", m_test_font, m_menubuttons, button1_def);
	m_main_quit		= new UITextButton(MENUID_MAIN_QUIT,     0, 0, menu_width*2, menu_item_height, menu_item_side, menu_width, "Quit Game", m_test_font, m_menubuttons, button1_def);
	//m_main_quit		= new UITextButton(MENUID_MAIN_QUIT,     0, 0, menu_width, menu_item_height, menu_item_side, menu_item_side, "!0123456789abcdefgh\nijklmnopqrstuvwxyz\nhawaiakavauazayafaeaxa", m_test_font, m_menubuttons, button1_def);



	m_main_menu->addItem(m_main_start);
	m_main_menu->addItem(m_main_howto);
	m_main_menu->addItem(m_main_settings);
	m_main_menu->addItem(m_main_scores);
	m_main_menu->addItem(m_main_quit);

	m_main_start->setTransition(new UITransitionLR(0.00f, 0.50f, Screen::getWidth()));
	m_main_howto->setTransition(new UITransitionLR(0.10f, 0.60f, Screen::getWidth()));
	m_main_settings->setTransition(new UITransitionLR(0.20f, 0.70f, Screen::getWidth()));
	m_main_scores->setTransition(new UITransitionLR(0.30f, 0.80f, Screen::getWidth()));
	m_main_quit->setTransition(new UITransitionLR(0.40f, 0.90f, Screen::getWidth()));
//	m_main_slidertest->setTransition(new UITransitionLR(0.30f, 0.80f, 800));
//	m_main_textbutton->setTransition(new UITransitionLR(0.40f, 0.90f, 800));


	// level menu

	m_level_layer = new UILayer();
	m_level_layer->setEventCallback(this);
	m_level_layer->setPosition(0, 0);

	m_level_title		= new UIText(64, m_test_font);
	m_level_title->setText("select level");
	m_level_title->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_TOP);
	m_level_title->setPosition(center_x, Screen::getHeightSlice(48));

	Texture* back_arrow = m_gameui_assets->findTexture("back_arrow");

	float level_item_width = Screen::getWidthSlice(3);
	float level_item_height = Screen::getHeightSlice(4.8f);
	float level_item_margin = Screen::getWidthSlice(80);

	m_level_left		= new UITextButton(MENUID_LEVEL_1,		0, 0, level_item_width, level_item_height, "Level 1", m_test_font, m_main_bg);
	m_level_right	= new UITextButton(MENUID_LEVEL_2,		0, 0, level_item_width, level_item_height, "Level 2", m_test_font, m_main_bg);
	m_level_left->setPosition(center_x - (level_item_width / 2) - level_item_margin, center_y - (level_item_height / 2));
	m_level_right->setPosition(center_x + (level_item_width / 2) + level_item_margin, center_y - (level_item_height / 2));

	// TODO: everything needs to scale!
	float arrow_width = 85.0f;
	float arrow_height = 64.0f;

	m_level_back		= new UIButton(MENUID_LEVEL_BACK, Screen::getWidth()-(arrow_width/2), Screen::getHeight()-(arrow_height/2), arrow_width, arrow_height, back_arrow, back_arrow);

	m_level_layer->addItem(m_level_title);
	m_level_layer->addItem(m_level_left);
	m_level_layer->addItem(m_level_right);
	m_level_layer->addItem(m_level_back);

	m_level_title->setTransition(new UITransitionUD(0.0f, 1.0f, Screen::getHeight()));
	m_level_left->setTransition(new UITransitionLR(0.0f, 0.5f, Screen::getWidth()));
	m_level_right->setTransition(new UITransitionLR(0.5f, 1.0f, Screen::getWidth()));
	m_level_back->setTransition(new UITransitionLR(1.0f, 1.0f, Screen::getWidth()));


	// settings menu
	m_settings_layer = new UILayer();
	m_settings_layer->setEventCallback(this);
	m_settings_layer->setPosition(0, 0);

	m_settings_menu = new UIMenu(center_x - (menu_width / 2), Screen::getHeightSlice(6));
	m_settings_menu->setSpacing(3.0f);

	m_settings_title = new UIText(14, m_test_font);
	m_settings_title->setText("options");
	m_settings_title->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_TOP);
	m_settings_title->setPosition(center_x, Screen::getHeightSlice(48));

	m_settings_b1 = new UITextButton(MENUID_SETTING_1,		0, 0, menu_width, menu_item_height, menu_item_side, menu_item_side, "setting 1", m_test_font, m_menubuttons, button2_def);
	m_settings_b2 = new UITextButton(MENUID_SETTING_2,		0, 0, menu_width, menu_item_height, menu_item_side, menu_item_side, "setting 2", m_test_font, m_menubuttons, button2_def);
	m_settings_b3 = new UITextButton(MENUID_SETTING_3,		0, 0, menu_width, menu_item_height, menu_item_side, menu_item_side, "setting 3", m_test_font, m_menubuttons, button2_def);
	m_settings_b4 = new UITextButton(MENUID_SETTING_4,		0, 0, menu_width, menu_item_height, menu_item_side, menu_item_side, "setting 4", m_test_font, m_menubuttons, button2_def);
	m_settings_b5 = new UITextButton(MENUID_SETTING_5,		0, 0, menu_width, menu_item_height, menu_item_side, menu_item_side, "setting 5", m_test_font, m_menubuttons, button2_def);
	m_settings_menu->addItem(m_settings_b1);
	m_settings_menu->addItem(m_settings_b2);
	m_settings_menu->addItem(m_settings_b3);
	m_settings_menu->addItem(m_settings_b4);
	m_settings_menu->addItem(m_settings_b5);

	m_settings_back = new UIButton(MENUID_SETTINGS_BACK, Screen::getWidth()-(arrow_width/2), Screen::getHeight()-(arrow_height/2), arrow_width, arrow_height, back_arrow, back_arrow);

	m_settings_layer->addItem(m_settings_menu);
	m_settings_layer->addItem(m_settings_title);
	m_settings_layer->addItem(m_settings_back);

	m_settings_title->setTransition(new UITransitionUD(0.0f, 1.0f, Screen::getHeight()));
	m_settings_b1->setTransition(new UITransitionLR(0.00f, 0.50f, Screen::getWidth()));
	m_settings_b2->setTransition(new UITransitionLR(0.10f, 0.60f, Screen::getWidth()));
	m_settings_b3->setTransition(new UITransitionLR(0.20f, 0.70f, Screen::getWidth()));
	m_settings_b4->setTransition(new UITransitionLR(0.30f, 0.80f, Screen::getWidth()));
	m_settings_b5->setTransition(new UITransitionLR(0.40f, 0.90f, Screen::getWidth()));
	m_settings_back->setTransition(new UITransitionLR(1.0f, 1.0f, Screen::getWidth()));



	m_status = GAMEMENU_STATUS_IDLE;
	m_selected_level = -1;

	m_transition = false;
	m_trans_in = NULL;
	m_trans_out = NULL;
	m_trans_in_start = 0;
	m_trans_in_duration = 0;
	m_trans_out_duration = 0;
	m_trans_timer = 0;

	m_current_page = NULL;


	switch (page)
	{
		case PAGE_MAIN_MENU:
		{
			m_current_page = m_main_layer;
			startTransition(NULL, 0, m_main_layer, 0, 800, PAGE_MAIN_MENU);
			break;
		}

		case PAGE_LEVEL_SELECT:
		{
			m_current_page = m_level_layer;
			startTransition(NULL, 0, m_level_layer, 0, 800, PAGE_LEVEL_SELECT);
			break;
		}

		case PAGE_SETTINGS:
		{
			m_current_page = m_settings_layer;
			startTransition(NULL, 0, m_settings_layer, 0, 800, PAGE_SETTINGS);
			break;
		}

		default:
			assert(0);
			break;
	}

	if (m_current_page != NULL)
		m_current_page->updateTransition(0, UITransition::STATE_IN);
}

void GameMenu::shutdown()
{
	SAFE_DELETE(m_menutexts);
	SAFE_DELETE(m_main_bg);
	SAFE_DELETE(m_menubuttons);

	SAFE_DELETE(m_main_start);
	SAFE_DELETE(m_main_howto);
	SAFE_DELETE(m_main_settings);
	SAFE_DELETE(m_main_scores);
	SAFE_DELETE(m_main_quit);
	SAFE_DELETE(m_main_slidertest);

	SAFE_DELETE(m_test_font);

	SAFE_DELETE(m_main_menu);
	SAFE_DELETE(m_main_layer);

	SAFE_DELETE(m_level_layer);
	SAFE_DELETE(m_level_title);
	SAFE_DELETE(m_level_left);
	SAFE_DELETE(m_level_right);	

	SAFE_DELETE(m_menu_bg);
}

GameMenu::Status GameMenu::getStatus()
{
	return m_status;
}

int GameMenu::getSelectedLevel()
{
	return m_selected_level;
}

void GameMenu::startTransition(UILayer* layer_out, int out_duration, UILayer* layer_in, int in_start, int in_duration, GameMenu::Page destpage)
{
	m_transition = true;
	m_trans_in_duration = (timeunit)(in_duration) * 1000000;
	m_trans_in_start = (timeunit)(in_start) * 1000000;
	m_trans_out_duration = (timeunit)(out_duration) * 1000000;

	m_trans_out = layer_out;
	m_trans_in = layer_in;

	m_trans_timer = 0;
	m_trans_destpage = destpage;
}

void GameMenu::onUIEvent(int itemid, UI::UIItemEvent eventid)
{
	switch (itemid)
	{
		case MENUID_MAIN_START:
		{
			startTransition(m_main_layer, 800, m_level_layer, 400, 800, PAGE_LEVEL_SELECT);
			break;
		}
		case MENUID_MAIN_SETTINGS:
		{
			startTransition(m_main_layer, 800, m_settings_layer, 400, 800, PAGE_SETTINGS);
			break;
		}
		case MENUID_MAIN_QUIT:
		{
			m_status = GAMEMENU_STATUS_QUIT;
			break;
		}
		case MENUID_LEVEL_1:
		{
			startTransition(m_level_layer, 800, NULL, 0, 0, PAGE_ENTER_LEVEL1);
			break;
		}
		case MENUID_LEVEL_2:
		{
			startTransition(m_level_layer, 800, NULL, 0, 0, PAGE_ENTER_LEVEL2);
			break;
		}
		case MENUID_LEVEL_BACK:
		{
			startTransition(m_level_layer, 800, m_main_layer, 400, 800, PAGE_MAIN_MENU);
			break;
		}
		case MENUID_SETTINGS_BACK:
		{
			startTransition(m_settings_layer, 800, m_main_layer, 400, 800, PAGE_MAIN_MENU);
			break;
		}
	}
}

void GameMenu::onUpdate(int time)
{
	if (!m_transition)
	{
		m_current_page->updateTransition(0, UITransition::STATE_OFF);
		m_current_page->onUpdate(time);
	}
	else
	{
		m_trans_timer += time;

		timeunit full_duration = max(m_trans_in_start + m_trans_in_duration, m_trans_out_duration);

		float in_phase;
		float out_phase;

		if (m_trans_timer < m_trans_out_duration)
			out_phase = (float)(m_trans_timer) / (float)(m_trans_out_duration);
		else		
			out_phase = 1.0f;

		if (m_trans_timer < m_trans_in_start)
			in_phase = 0.0f;
		else
			in_phase = (float)(m_trans_timer - m_trans_in_start) / (float)(m_trans_in_duration);
		
		
		if (m_trans_in != NULL)
			m_trans_in->updateTransition(in_phase, UITransition::STATE_IN);
		
		if (m_trans_out != NULL)
			m_trans_out->updateTransition(out_phase, UITransition::STATE_OUT);


		if (m_trans_timer >= full_duration)
		{
			m_transition = false;

			switch (m_trans_destpage)
			{
				case PAGE_MAIN_MENU:		m_current_page = m_main_layer; break;
				case PAGE_LEVEL_SELECT:		m_current_page = m_level_layer; break;
				case PAGE_SETTINGS:			m_current_page = m_settings_layer; break;

				case PAGE_ENTER_LEVEL1:
				{
					m_status = GAMEMENU_STATUS_STARTGAME;
					m_selected_level = 0;
					break;
				}
				case PAGE_ENTER_LEVEL2:
				{
					m_status = GAMEMENU_STATUS_STARTGAME;
					m_selected_level = 1;
					break;
				}
				

				default:					assert(0); break;
			}
		}
	}
}

void GameMenu::onRender(const Shaders::UIShaderContext* context)
{
	float parent_x = 0.0f;
	float parent_y = 0.0f;

	float viewport_mat[4] = {1.0f / Screen::getWidthFraction(0.5f), 0.0f, 0.0f, 1.0f / Screen::getHeightFraction(0.5f)};
	float rot_mat[4] = {1.0f, 0.0f, 0.0f, 1.0f};

	glUniformMatrix2fv(context->vp_matrix, 1, false, viewport_mat);
	glUniformMatrix2fv(context->rot_matrix, 1, false, rot_mat);
	glUniform4f(context->color, 1.0f, 1.0f, 1.0f, 1.0f);

	m_menu_bg->onRender(context, 0, 0);

	if (!m_transition)
	{
		m_current_page->onRender(context, parent_x, parent_y);
	}
	else
	{
		if (m_trans_in != NULL)
			m_trans_in->onRender(context, parent_x, parent_y);

		if (m_trans_out != NULL)
			m_trans_out->onRender(context, parent_x, parent_y);
	}
}

void GameMenu::onTouch(UI::UITouchEvent type, float x, float y)
{
	if (!m_transition)
	{
		m_current_page->onTouch(type, x, y);
	}
}
