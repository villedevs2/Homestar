#pragma once

#include "EventUIEffect.h"
#include "Text.h"
#include "Shaders.h"
#include "Screen.h"

#include <string>
#include <vector>
#include <assert.h>

class GameUI
{
public:
	enum GameUIEffect
	{
		UI_SCORE_100,
		UI_SCORE_200,
		UI_SCORE_300,
		UI_SCORE_400,
		UI_SCORE_500,
		UI_SCORE_600,
		UI_SCORE_700,
		UI_SCORE_800,
		UI_SCORE_900,
		UI_SCORE_1000,
		UI_SPEEDZONE,
		UI_CRASH,
		UI_OUT_OF_TRACK,
		UI_BONUS_2X,
		UI_BONUS_3X,
		UI_BONUS_4X,
		UI_BONUS_5X,
		UI_BONUS_6X,
		UI_BONUS_7X,
		UI_BONUS_8X,
		UI_BONUS_9X,
		UI_BONUS_10X,
		UI_COURSE_COMPLETE,
		UI_NUM_EFFECTS,		// for counting
	};

	typedef struct
	{
		const char* text;
		float kerning;
		int duration;
	} UIEffect;

	static const int MAX_POINT_MULTIPLIER = 20;


	GameUI();
	~GameUI();

	void init();
	void free();
	void setScore(int score);
	void setClock(const std::string& clock);
	void addUIEffect(int type);
	void update(int time);
	void render(const Shaders::UIShaderContext* context, float parent_x, float parent_y);

private:
	EventUIEffect* m_ui_effect;
	Text* m_ui_effect_text[UI_NUM_EFFECTS];
	Font* m_ui_effect_font;
	
	int m_score;
	Text* m_score_text;
	Font* m_score_font;

	Text* m_clock_text;
	Font* m_clock_font;
};
