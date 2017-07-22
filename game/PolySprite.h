#pragma once

#include "glm/glm.hpp"

#include "Texture2.h"
#include "Shaders.h"

class PolySprite
{
public:
	PolySprite(const glm::vec2* points, const glm::vec2* uvs, int num_points, float z);
	PolySprite(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, float z, Texture* texture);
	PolySprite(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4,
			   const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3, const glm::vec2& uv4, float z);
	~PolySprite();

	const glm::vec2* getPoints();
	int getNumPoints();
	void render(const Shaders::GameShaderContext* context);

private:
	int tesselate(int vb_index);

	glm::vec2* m_points;
	glm::vec2* m_uvs;
	int m_num_points;
	float m_z;

	float* m_vb;
	int m_num_tris;
};