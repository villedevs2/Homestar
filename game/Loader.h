#pragma once

#include "Shaders.h"
#include "Texture2.h"
#include "QuadMesh2.h"
#include "Font.h"
#include "Text.h"
#include "Screen.h"
#include "PolyAnim.h"

class Loader
{
public:
	Loader();
	~Loader();

	void render(const Shaders::UIShaderContext* context, const Shaders::GameShaderContext* game_shader);
	void update(int time);
	void init(int numticks);
	void shutdown();
	void setPosition(int pos);

private:
	Texture* m_bartex;
	Font* m_font;
	Text* m_text;
	TextureSet* m_textures;
	TextureSet* m_hamster_tex;

	PolyAnim* m_wheel_anim;
	PolyAnim* m_hamster_anim;

	QuadMesh2* m_barmesh;

	int m_numticks;
	int m_tick;

	timeunit m_timer;
};