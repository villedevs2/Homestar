#include "GameUI.h"

static const GameUI::UIEffect UI_EFFECTS[] =
{
	{ "100",					15,		1000,		},
	{ "200",					15,		1000,		},
	{ "300",					15,		1000,		},
	{ "400",					15,		1000,		},
	{ "500",					15,		1000,		},
	{ "600",					15,		1000,		},
	{ "700",					15,		1000,		},
	{ "800",					15,		1000,		},
	{ "900",					15,		1000,		},
	{ "1000",					15,		1000,		},
	{ "SPEEDZONE!",				0,		1500,		},
	{ "CRASHED!",				9,		2000,		},
	{ "OUT OF TRACK!",			0,		4000,		},
	{ "SCORE BONUS 2X",			-6,		2000,		},
	{ "SCORE BONUS 3X",			-6,		2000,		},
	{ "SCORE BONUS 4X",			-6,		2000,		},
	{ "SCORE BONUS 5X",			-6,		2000,		},
	{ "SCORE BONUS 6X",			-6,		2000,		},
	{ "SCORE BONUS 7X",			-6,		2000,		},
	{ "SCORE BONUS 8X",			-6,		2000,		},
	{ "SCORE BONUS 9X",			-6,		2000,		},
	{ "SCORE BONUS 10X",		-6,		2000,		},
	{ "COURSE COMPLETE!",		0,      5000,		},
};

GameUI::GameUI()
{
	m_score_font = new Font();
	m_ui_effect_font = new Font();

	m_clock_font = new Font();

	for (int i=0; i < UI_NUM_EFFECTS; i++)
	{
		m_ui_effect_text[i] = new Text(20);
	}
	m_ui_effect = new EventUIEffect(4, m_ui_effect_text);
	
	m_score_text = new Text(64);
	m_clock_text = new Text(64);

	m_score = 0;
}

GameUI::~GameUI()
{
	SAFE_DELETE(m_ui_effect);
	SAFE_DELETE(m_score_font);
	SAFE_DELETE(m_clock_font);
	SAFE_DELETE(m_ui_effect_font);

	for (int i=0; i < UI_NUM_EFFECTS; i++)
	{
		SAFE_DELETE(m_ui_effect_text[i]);
	}

	SAFE_DELETE(m_score_text);
	SAFE_DELETE(m_clock_text);
}

void GameUI::init()
{
	// ui effect
	m_ui_effect_font->load("200.font.xml", Screen::getHeightSlice(12));

	for (int i=0; i < UI_NUM_EFFECTS; i++)
	{
		m_ui_effect_text[i]->setFont(m_ui_effect_font);
		m_ui_effect_text[i]->setAnchor(Text::ANCHOR_X_CENTER | Text::ANCHOR_Y_CENTER);
		
		m_ui_effect_text[i]->set(UI_EFFECTS[i].text);
		m_ui_effect_text[i]->setKerning(UI_EFFECTS[i].kerning);
	}

	float clock_x = Screen::getWidthSlice(10);
	float clock_y = Screen::getHeightSlice(24);

	// score
	m_score_font->load("newfont1.font.xml", Screen::getHeightSlice(12));
	m_score_text->setAnchor(Text::ANCHOR_X_LEFT | Text::ANCHOR_Y_TOP);
	m_score_text->setFont(m_score_font);
	m_score_text->setPosition(40, 40);
	m_score_text->set("0000000000");

	// clock
	m_clock_font->load("newfont1.font.xml", Screen::getHeightSlice(12));
	m_clock_text->setAnchor(Text::ANCHOR_X_LEFT | Text::ANCHOR_Y_TOP);
	m_clock_text->setFont(m_clock_font);
	m_clock_text->setPosition(clock_x, clock_y);
	m_clock_text->set("");
}



void GameUI::setScore(int score)
{
	/*
	if (score != m_score)
	{
		m_score_text->set(score, 10, true);
		m_score = score;
	}
	*/
}

void GameUI::setClock(const std::string& clock)
{
	m_clock_text->set(clock.c_str());
}



void GameUI::addUIEffect(int type)
{
	int duration = UI_EFFECTS[type].duration;

	m_ui_effect->addEvent(type, duration);
}

void GameUI::update(int time)
{
	m_ui_effect->update(time);
}

void GameUI::render(const Shaders::UIShaderContext* context, float parent_x, float parent_y)
{
	//m_score_text->render(context);
	m_clock_text->render(context, parent_x, parent_y);
	m_ui_effect->render(context);
}
