#pragma once

#define _USE_MATH_DEFINES				// this must be defined before *any* includes

#include <Platform.h>

#include <fstream>
#include <cmath>
#include <vector>
#include <list>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shaders.h"
#include "Text.h"
#include "ParticleSys2.h"
#include "Font.h"
#include "GameUI.h"
#include "GenShadow.h"
#include "Clock.h"
#include "Timer.h"
#include "PolyAnim.h"
#include "StaticGeometry.h"
#include "PlayerObjects.h"
#include "DynamicObject.h"

#include "Player.h"
#include "EventUIEffect.h"

#include "UI.h"
#include "UILayer.h"
#include "UIButton.h"
#include "UIText.h"
#include "UIStatic.h"
#include "UIBox.h"
#include "UITextButton.h"

#include "Box2D\Box2D.h"


class Engine : public b2QueryCallback,
			   public b2ContactListener,
			   public UI::UIEventCallback,
			   public TimerSystem::TimerEventCallback
{
public:
	enum Status
	{
		STATUS_RUNNING = 1,
		STATUS_EXIT = 2,
	};

	Engine(void);
	~Engine(void);

	void init(std::string levelname);
	void reset();
	void onUpdate(int time);
	void onRender();
	void onViewportChange(int width, int height);
	void onTouch(UI::UITouchEvent type, float x, float y);
	void setInputAngle(float angle);
	void setShaders(const Shaders::GameShaderContext* game_shader,
					const Shaders::UIShaderContext* ui_shader,
					const Shaders::ParticleShaderContext* particle_shader);
	int load();
	void shutdown();
	Engine::Status getStatus();
	void reportMS(int framems);
	void reportF2FMS(int ms);
	void reportFPS(int fps);

protected:
	bool ReportFixture(b2Fixture* fixture);								// for b2QueryCallback	
	void BeginContact(b2Contact* contact);								// for b2ContactListener

private:
	enum
	{
		GAMEUIID_BACK = 1,
		GAMEUIID_PAUSE_RESUME = 2,
		GAMEUIID_PAUSE_QUIT = 3,
		GAMEUIID_WIN_CONTINUE = 4,
		GAMEUIID_INTRO_START = 5,
		GAMEUIID_FAIL_TRYAGAIN = 6,
		GAMEUIID_FAIL_QUIT = 7,
		GAMEUIID_WIN_QUIT = 8,
	};

	enum
	{
		TIMER_BASIC = 1,
		TIMER_WINSCREEN_IN_ANIM,
		TIMER_WINSCREEN_IN,
		TIMER_WINSCREEN_OUT_ANIM,
		TIMER_WINSCREEN_OUT,
		TIMER_PARTICLE_SPAWN,
		TIMER_PLAYER_RESTORE,
		TIMER_PLAYER_RESTORE_ANIM,
	};

	enum LoadState
	{
		LOAD_BLOB_SHADOW			= 0,
		LOAD_STATIC_SHADOWS			= 10,
		LOAD_TUNNEL_MESHES			= 20,
		LOAD_TUNNEL_TEXTURES		= 30,
		LOAD_PLAYER_MESHES			= 40,
		LOAD_INIT_PARTICLES			= 50,
		LOAD_INIT_UI				= 60,
		LOAD_TRACK					= 70,
		LOAD_PAUSE_SCREEN			= 90,
		LOAD_FINISH					= 99,		// Must be last!
	};

	enum ObjectType
	{
		OBJECT_TYPE_INVALID			= 0,
		OBJECT_TYPE_COLLISION		= 1,
		OBJECT_TYPE_GEOVIS			= 2,
		OBJECT_TYPE_TRIGGER			= 3,
		OBJECT_TYPE_SLIDER			= 4,
		OBJECT_TYPE_DESTRUCTIBLE	= 5,
		OBJECT_TYPE_MOVER			= 6,
	};

	enum CollectableType
	{
		COLLECTABLE_INVALID		= 0,
		COLLECTABLE_COIN		= 1,
	};

	enum TriggerType
	{
		TRIGGER_TYPE_PLAYER_START		= 0,
		TRIGGER_TYPE_LEVEL_END			= 1,
		TRIGGER_TYPE_COIN				= 2,
		TRIGGER_TYPE_PARTICLE_GEN		= 3,
	};

	struct StaticGeo
	{
		glm::vec2 pos[8];
		glm::vec2 uv[8];
		int num_points;
		float z;
	};

	struct StaticGeoBucket
	{
		int vbi;
		int num_tris;
	};

	// TODO REMOVE
	struct ParticleGen
	{
		glm::vec2 position;
		float angle;
	};

	enum MoverState
	{
		MOVER_UP,
		MOVER_DOWN,
		MOVER_GOING_UP,
		MOVER_GOING_DOWN,
	};

	struct MoverObject
	{
		timeunit timer;
		timeunit uptime;
		timeunit downtime;
		timeunit movetime;
		glm::vec2 startpos;
		glm::vec2 endpos;
		b2Body* body;
		MoverState state;
		float length;
		glm::vec2 axis;
	};


	enum LevelAffixTypes
	{
		LEVEL_AFFIX_TIMELIMIT			= 0x00000001,
		LEVEL_AFFIX_COLLECT1			= 0x00000002,
		LEVEL_AFFIX_COLLECT2			= 0x00000004,
		LEVEL_AFFIX_COLLECT3			= 0x00000008,
		LEVEL_AFFIX_DONT_COLLECT		= 0x00000010,
		LEVEL_AFFIX_AVOID_HAZARDS		= 0x00000020,
		LEVEL_AFFIX_FIND_EXIT			= 0x00000040,
		LEVEL_AFFIX_INVERSE_GRAVITY		= 0x00000080,
		LEVEL_AFFIX_SPEEDRUN			= 0x00000100,
	};

	struct LevelAffixes
	{
		timeunit time_limit;
		int collect_item[3];
		int collect_num[3];
		int dont_collect_item;
		unsigned int affixes;
	};

	enum GameState
	{
		GAMESTATE_NORMAL,
		GAMESTATE_INTRO,
		GAMESTATE_FAIL,
		GAMESTATE_WIN,
		GAMESTATE_PAUSE,
		GAMESTATE_PLAYER_RESTORE,
	};


	static const unsigned int BLBX_ID = 0x424c4258;
	static const unsigned int BLOX_ID = 0x424c4f58;
	static const unsigned int LAFX_ID = 0x4c414658;
	static const unsigned int BLBX_VERSION = 0x00010003;

	static const int WINSCREEN_ANIM_DURATION = 1000;

	void onUIEvent(int itemid, UI::UIItemEvent eventid);
	void onTimerEvent(int id, timeunit count, timeunit duration);
	glm::vec2 calculateObjectCenter(const glm::vec2* p, int num_points);
	bool loadLevel(std::string filename);
	void insertStaticCollision(const glm::vec2* p, const glm::vec2* uv, int num_points, float z);
	void insertStaticGeovis(const glm::vec2* p, const glm::vec2* uv, int num_points, float z);
	void tesselateStaticGeo();
	void insertCollectable(const glm::vec2 pos, CollectableType type);
	void insertPlayerObject(DynamicObject* object);
	void insertSlider(const glm::vec2* p, const glm::vec2* uv, int num_points, glm::vec2 axis, float z);
	void setEndZone(glm::vec2 position, const glm::vec2* p, const glm::vec2* uvs, int num_points, Texture* texture, float z);
	void insertDestructible(const glm::vec2* p, const glm::vec2* uv, int num_points, float z);
	void insertMover(const glm::vec2* p, const glm::vec2* uv, int num_points, glm::vec2 axis, float length, float uptime, float downtime, float time, float z);
	void updateMovers(int time);
	int processBucket(int index, float minx, float maxx, float miny, float maxy, std::vector<float>& vb);
	void setPlayerPosition(const glm::vec2& position);
	void setFail(bool enable, const std::string& reason);
	void setGameState(GameState gamestate);


	bool m_render_game;

	GameState m_gamestate;

	std::string m_level_name;

	LevelAffixes m_affixes;


	Clock* m_clock;
	TimerSystem* m_timer;

	Player* m_player;

	std::string m_error_string;

	float m_touch_start_x;
	float m_player_angle;

	UILayer* m_uilayer;
	UIButton* m_uiback;

	Font* m_bigfont;			// large UI displays (short and clear)
	Font* m_smallfont;		// small text for message boxes (longer stuff)
	Font* m_buttonfont;		// font for buttons
	Font* m_messagefont;		// very large messages (win, lose, etc.)

	TextureSet* m_introbox_texture;
	UIBox* m_introbox;
	UITextButton* m_introbox_start;
	Font* m_intro_titlefont;
	UIText* m_intro_title;
	UILayer* m_intro_layer;
	UIText* m_intro_affixes;

	UITextButton* m_failbox_tryagain;
	UITextButton* m_failbox_quit;
	UIText* m_failbox_title;
	UILayer* m_failbox_layer;

	UITextButton* m_win_continue;
	UITextButton* m_win_quit;
	UIText* m_win_title;
	UIText* m_win_time;
	UILayer* m_win_layer;

	Texture* m_clockicon_tex;
	UIStatic* m_clockicon;
	UIText* m_clocktext;

	UIText* m_collect1_text;
	UIText* m_collect2_text;
	UIText* m_collect3_text;

	const Shaders::GameShaderContext* m_game_shader;
	const Shaders::UIShaderContext* m_ui_shader;
	const Shaders::ParticleShaderContext* m_particle_shader;

	glm::vec2 m_player_restore_start;
	glm::vec2 m_player_restore_end;
	glm::vec2 m_player_restore_pos;

	glm::vec2 m_player_pos;

	Texture* m_vignette;
	TextureSet* m_dustcloud_tex;
	TextureSet* m_sparkle_tex;
	TextureSet* m_gameui_texture;
	ParticleSys2* m_sparkles;
	ParticleSys2* m_dustcloud;

	QuadMesh2* m_testbox;

	int m_load_progress;

	Status m_status;

	int m_winscreen_timer;

	Font* m_pausefont;
	QuadMesh2* m_pause_bg;

	UIStatic* m_pause_bgg;
	UITextButton* m_pause_resume;
	UITextButton* m_pause_quit;
	UIText* m_pause_title;
	UILayer* m_pause_layer;

	Font* m_sensor_font;
	UIText* m_sensors;
	

	b2World* m_world;	

	Texture* m_staticgeo_tex;

	TextureSet* m_soccer_set;
	TextureSet* m_bountyhunter_set;
	TextureSet* m_tire_set;
	TextureSet* m_bh_effect_set;
	TextureSet* m_reactor_set;
	TextureSet* m_hamsterwheel_set;
	TextureSet* m_hamsterleft_set;
	TextureSet* m_hamsterright_set;

	b2Body* m_player_body;

	PolyAnim* m_coin_sprite;
	Texture* m_coin_tex;
	Texture* m_goal_tex;

	b2Body* m_level_container_body;

	PolyAnim* m_levelbg;
	Texture* m_levelbgtex;

	Texture* m_level_texture;
	glm::vec2 m_player_start_pos;

	float m_scrollpos_x;
	float m_scrollpos_y;

	float m_level_min_x;
	float m_level_max_x;
	float m_level_min_y;
	float m_level_max_y;

	std::list<DynamicObject*> m_vis_dynobjs;
	std::list<DynamicObject*> m_dynobjs;
	std::list<PolyAnim*> m_sprites;
	std::list<MoverObject*> m_movers;

	std::list<ParticleGen*> m_particle_gen;		// TODO REMOVE

	// collision body removal list
	std::list<b2Body*> m_destroy_list;

	StaticGeometry* m_geometry;

	int m_last_ms;
	int m_last_f2fms;
	int m_last_fps;

	float m_bg_width;
	float m_bg_height;
};
