#pragma once

#include "Shaders.h"

#include "UI.h"
#include "UIButton.h"
#include "UIStatic.h"
#include "UILayer.h"
#include "UIMenu.h"
#include "UISlider.h"
#include "UIText.h"
#include "UITextButton.h"
#include "Screen.h"

class GameMenu : public UI::UIEventCallback
{
public:
	typedef enum
	{
		GAMEMENU_STATUS_IDLE = 0,
		GAMEMENU_STATUS_STARTGAME,
		GAMEMENU_STATUS_QUIT,
	} Status;

	typedef enum
	{
		PAGE_MAIN_MENU = 0,
		PAGE_SETTINGS,
		PAGE_LEVEL_SELECT,
		PAGE_ENTER_LEVEL1,
		PAGE_ENTER_LEVEL2,
	} Page;

	GameMenu();
	virtual ~GameMenu();

	void init(GameMenu::Page page);
	void shutdown();
	void onUpdate(int time);
	void onRender(const Shaders::UIShaderContext* context);
	void onTouch(UI::UITouchEvent type, float x, float y);
	Status getStatus();
	int getSelectedLevel();

private:
	typedef enum
	{
		MENUID_MAIN_START			= 1,
		MENUID_MAIN_SETTINGS,
		MENUID_MAIN_QUIT,
		MENUID_LEVEL_1,
		MENUID_LEVEL_2,
		MENUID_LEVEL_BACK,
		MENUID_SETTING_1,
		MENUID_SETTING_2,
		MENUID_SETTING_3,
		MENUID_SETTING_4,
		MENUID_SETTING_5,
		MENUID_SETTINGS_BACK,
	} MenuID;

	void onUIEvent(int itemid, UI::UIItemEvent eventid);
	void startTransition(UILayer* layer_out, int out_duration, UILayer* layer_in, int in_start, int in_duration, GameMenu::Page destpage);

	UILayer* m_main_layer;
	UIMenu* m_main_menu;
	UITextButton* m_main_start;
	UITextButton* m_main_howto;
	UITextButton* m_main_settings;
	UITextButton* m_main_scores;
	UITextButton* m_main_quit;
	UISlider* m_main_slidertest;
	UIStatic* m_main_uibg;

	UILayer* m_level_layer;
	UIText* m_level_title;
	UITextButton* m_level_left;
	UITextButton* m_level_right;
	UIButton* m_level_back;

	UILayer* m_settings_layer;
	UIMenu* m_settings_menu;
	UIText* m_settings_title;
	UITextButton* m_settings_b1;
	UITextButton* m_settings_b2;
	UITextButton* m_settings_b3;
	UITextButton* m_settings_b4;
	UITextButton* m_settings_b5;
	UIButton* m_settings_back;

	UIStatic* m_menu_bg;

	Font* m_test_font;
	Texture* m_main_bg;
	TextureSet* m_menutexts;
	TextureSet* m_gameui_assets;
	TextureSet* m_menubuttons;

	Status m_status;
	int m_selected_level;

	bool m_transition;
	UILayer* m_trans_in;
	UILayer* m_trans_out;
	timeunit m_trans_in_duration;
	timeunit m_trans_in_start;
	timeunit m_trans_out_duration;
	timeunit m_trans_timer;
	GameMenu::Page m_trans_destpage;

	UILayer* m_current_page;
};