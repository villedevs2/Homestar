#pragma once

#define _USE_MATH_DEFINES

#include "Platform.h"
#include "Debug.h"
#include "Shaders.h"
#include "Engine.h"
#include "Loader.h"
#include "GameMenu.h"
#include "Screen.h"

#include "UI.h"
#include "UILayer.h"
#include "UIButton.h"
#include "UIStatic.h"
#include "UISlider.h"
#include "UIMenu.h"

#include <string>
#include <cmath>

class Game
{
public:
	Game();
	~Game();

	void init(int screen_width, int screen_height);
	void shutdown();
	void onUpdate(int time);
	void onRender();
	void onViewportChange(int width, int height);
	void onTouch(UI::UITouchEvent type, float x, float y);
	void onSensor(float *sensor_data);
	void reportMS(int framems);
	void reportF2FMS(int ms);
	void reportFPS(int fps);

	static const int LOGIC_PERIOD_NS = 16666666;

private:
	typedef enum
	{
		GAMESTATE_MENU_INIT,
		GAMESTATE_MENU,
		GAMESTATE_INGAME,
		GAMESTATE_INGAME_LOAD,
	} GameState;

	void changeState(GameState newstate);

	Engine* m_engine;
	GameState m_gamestate;

	GameMenu* m_menu;
	GameMenu::Page m_nextpage;

	Loader* m_loader;

	Shaders::GameShaderContext m_game_shader;
	Shaders::UIShaderContext m_ui_shader;
	Shaders::ParticleShaderContext m_particle_shader;

	//bool m_uiflipped;
	//timeunit m_uiflip_timer;
	//float m_uiflip_angle;
};
