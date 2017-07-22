#pragma once

#include "UI.h"
#include "UIItem.h"
#include "UIButton.h"

#include "Shaders.h"
#include "Texture2.h"
#include "Text.h"
#include "QuadMesh2.h"

class UIMessageBox : public UIItem
{
public:
	typedef struct
	{
		const char* corner_tl;
		const char* corner_tr;
		const char* corner_bl;
		const char* corner_br;
		const char* border_l;
		const char* border_t;
		const char* border_r;
		const char* border_b;
		const char* contents;
		const char* titlebar;
		const char* left_button_up;
		const char* left_button_down;
		const char* right_button_up;
		const char* right_button_down;
		Font* smallfont;
		const char* text;
		Font* titlefont;
		const char* title;
	} Definition;

	UIMessageBox(float x, float y, float width, float height, TextureSet* texset,
				 const UIMessageBox::Definition& def);
	~UIMessageBox();

	void setTitle(const char* text);
	void setText(const char* text);

	virtual void onRemove();
	virtual void onUpdate(int time);
	virtual void onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y);
	virtual void onTouch(UI::UITouchEvent event, float x, float y);

private:
	void generateVB();
	void calculateLayout();

	float m_border_left;
	float m_border_right;
	float m_border_top;
	float m_border_bottom;
	float m_titlebar_height;

	TextureSet* m_texset;
	const Texture* m_tex_corner_tl;
	const Texture* m_tex_corner_tr;
	const Texture* m_tex_corner_bl;
	const Texture* m_tex_corner_br;
	const Texture* m_tex_border_l;
	const Texture* m_tex_border_r;
	const Texture* m_tex_border_t;
	const Texture* m_tex_border_b;
	const Texture* m_tex_contents;
	const Texture* m_tex_titlebar;

	Font* m_smallfont;
	Font* m_titlefont;

	Text* m_title;				// titlebar text
	Text* m_text;				// contents
	UIButton* m_left_button;
	UIButton* m_mid_button;
	UIButton* m_right_button;

	QuadMesh2* m_mesh;
};