#pragma once

#include "glm/glm.hpp"

#include "QuadMesh2.h"
#include "Shaders.h"
#include "Texture2.h"

#include "UIItem.h"

class UISlider : public UIItem
{
public:
	UISlider(float x, float y, float width, float height, int min_value, int max_value,
			 TextureSet* texture, std::string frame_texture, std::string slider_texture);
	UISlider(float width, float height, int min_value, int max_value,
			 TextureSet* texture, std::string frame_texture, std::string slider_texture);
	virtual ~UISlider();

	virtual void setValue(int value);
	virtual int getValue();
	virtual void setRange(int min_value, int max_value);
	virtual void setSize(float width, float height);
	virtual void setTexture(TextureSet* texture, std::string frame_texture, std::string slider_texture);
	virtual void onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y);
	virtual void onTouch(UI::UITouchEvent event, float x, float y);
	virtual void onUpdate(int time);
	virtual void onRemove();

private:
	void init(int min_value, int max_value, TextureSet* texture, std::string frame_tex, std::string slider_tex);
	void generateVB();

	bool m_pushed;

	TextureSet* m_texture;
	Texture* m_frame_tex;
	Texture* m_slider_tex;

	int m_min_value;
	int m_max_value;
	int m_current_value;

	QuadMesh2* m_mesh;
};