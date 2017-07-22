#pragma once

#include "UI.h"
#include "UIBaseButton.h"
#include "UIDrawable.h"

#include "Texture2.h"

class UIButton : public UIBaseButton
{
public:
	UIButton(int id, float x, float y, float width, float height, Texture* up_texture, Texture* down_texture);
	UIButton(float x, float y, float width, float height, Texture* up_texture, Texture* down_texture);
	UIButton(int id, float width, float height, Texture* up_texture, Texture* down_texture);
	UIButton(float width, float height, Texture* up_texture, Texture* down_texture);
	virtual ~UIButton();

	virtual void onRemove();
	virtual void onUpdate(int time);
	virtual void onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y);

private:
	void init(Texture* up_texture, Texture* down_texture);

	Texture* m_texture_up;
	Texture* m_texture_down;
	QuadMesh2* m_mesh_up;
	QuadMesh2* m_mesh_down;
};
