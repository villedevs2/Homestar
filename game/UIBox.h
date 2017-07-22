#pragma once

#include "UI.h"
#include "UIItem.h"
#include "UIButton.h"

#include "Shaders.h"
#include "Texture2.h"
#include "Text.h"
#include "QuadMesh2.h"

class UIBox : public UIItem
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
	} Definition;

	UIBox(float x, float y, float width, float height, float border_width, float border_height, TextureSet* texset,
	      const UIBox::Definition& def);
	~UIBox();

	virtual void onRemove();
	virtual void onUpdate(int time);
	virtual void onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y);
	virtual void onTouch(UI::UITouchEvent event, float x, float y);

private:
	void generateVB();
	void calculateLayout();

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

	QuadMesh2* m_mesh;

	float m_border_width;
	float m_border_height;
};