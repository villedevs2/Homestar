#pragma once

#include "glm/glm.hpp"

#include "QuadMesh2.h"
#include "Shaders.h"
#include "Texture2.h"

#include "UI.h"
#include "UIItem.h"

class UIDrawable : public UIItem
{
public:
	UIDrawable(float x, float y, float width, float height, Texture* texture);
	UIDrawable(float width, float height, Texture* texture);
	~UIDrawable();

	void setSize(float width, float height);
	void setTexture(Texture* texture);
	void onRender(const Shaders::UIShaderContext* context, float parent_x, float parent_y);

protected:
	void init(Texture* texture);

	Texture* m_texture;

private:
	void generateVB();

	QuadMesh2* m_mesh;
};