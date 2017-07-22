#pragma once

#include "glm/glm.hpp"

#include "Texture2.h"
#include "Shaders.h"

class QuadMesh2
{
public:
	QuadMesh2(int capacity);
	~QuadMesh2();

	void reset();
	void insert(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4,
				const Texture* texture);
	void insert(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4,
				const glm::vec2& t1, const glm::vec2& t2, const glm::vec2& t3, const glm::vec2& t4);
	void set(int index, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4,
			 const Texture* texture);
	void set(int index, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4,
			 const glm::vec2& t1, const glm::vec2& t2,const glm::vec2& t3, const glm::vec2& t4);
	void set_num(int num);
	void render(const Shaders::UIShaderContext* context);

private:
	glm::vec2* m_vb;
	glm::vec2* m_uvb;

	int m_capacity;
	int m_num_verts;
};