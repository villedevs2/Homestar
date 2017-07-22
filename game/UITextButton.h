#pragma once

#include "UIBaseButton.h"
#include "UI.h"

#include "Texture2.h"
#include "Font.h"
#include "Text.h"

class UITextButton : public UIBaseButton
{
public:
	typedef struct
	{
		const char* tex_up;
		const char* tex_up_left;
		const char* tex_up_right;
		const char* tex_down;
		const char* tex_down_left;
		const char* tex_down_right;
	} Def;

	UITextButton(int id, float x, float y, float width, float height, std::string text, Font* font, Texture* texture);
	UITextButton(int id, float x, float y, float width, float height, std::string text, Font* font, Texture* texup, Texture* texdown);
	UITextButton(int id, float x, float y, float width, float height, float left, float right, std::string text, Font* font, TextureSet* texset, const UITextButton::Def& def);
	virtual ~UITextButton();
	
	virtual void onRemove();
	virtual void onUpdate(int time);
	virtual void onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y);

private:
	void init(std::string text, Font* font, Texture* texup, Texture* texdown);
	void initDef(std::string text, float left, float right, Font* font, TextureSet* texset, const UITextButton::Def& def);

	Font* m_font;
	Text* m_text;

	Texture* m_texture_up;
	Texture* m_texture_down;
	QuadMesh2* m_mesh_up;
	QuadMesh2* m_mesh_down;

	Texture* m_texup_left;
	Texture* m_texup_right;
	Texture* m_texdown_left;
	Texture* m_texdown_right;

	TextureSet* m_texset;

	float m_left_size;
	float m_right_size;
};