#include "QuadMesh2.h"

QuadMesh2::QuadMesh2(int capacity)
{
	// each quad consists of 6 vertices / 2 triangles
	m_vb = new glm::vec2[capacity * 6];
	m_uvb = new glm::vec2[capacity * 6];

	m_capacity = capacity * 6;
	m_num_verts = 0;
}

QuadMesh2::~QuadMesh2()
{
	if (m_vb)
		delete [] m_vb;

	if (m_uvb)
		delete [] m_uvb;
}

void QuadMesh2::reset()
{
	m_num_verts = 0;
}

void QuadMesh2::set_num(int num)
{
	assert(num > 0 && num < (m_capacity * 6));

	m_num_verts = num * 6;
}

void QuadMesh2::insert(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4,
					   const Texture* texture)
{
	assert(m_num_verts < m_capacity);

	int vbi = m_num_verts;
	int uvbi = m_num_verts;

	m_vb[vbi++] = v1;			m_uvb[uvbi++] = glm::vec2(texture->m_left, texture->m_top);
	m_vb[vbi++] = v2;			m_uvb[uvbi++] = glm::vec2(texture->m_right, texture->m_top);
	m_vb[vbi++] = v3;			m_uvb[uvbi++] = glm::vec2(texture->m_left, texture->m_bottom);
	m_vb[vbi++] = v3;			m_uvb[uvbi++] = glm::vec2(texture->m_left, texture->m_bottom);
	m_vb[vbi++] = v2;			m_uvb[uvbi++] = glm::vec2(texture->m_right, texture->m_top);
	m_vb[vbi++] = v4;			m_uvb[uvbi++] = glm::vec2(texture->m_right, texture->m_bottom);

	m_num_verts += 6;
}

void QuadMesh2::insert(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4,
					   const glm::vec2& t1, const glm::vec2& t2, const glm::vec2& t3, const glm::vec2& t4)
{
	assert(m_num_verts < m_capacity);

	int vbi = m_num_verts;
	int uvbi = m_num_verts;

	m_vb[vbi++] = v1;			m_uvb[uvbi++] = t1;
	m_vb[vbi++] = v2;			m_uvb[uvbi++] = t2;
	m_vb[vbi++] = v3;			m_uvb[uvbi++] = t3;
	m_vb[vbi++] = v3;			m_uvb[uvbi++] = t3;
	m_vb[vbi++] = v2;			m_uvb[uvbi++] = t2;
	m_vb[vbi++] = v4;			m_uvb[uvbi++] = t4;

	m_num_verts += 6;
}

void QuadMesh2::set(int index, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4,
					const Texture* texture)
{
	assert(index >= 0 && index < (m_capacity * 6));

	int vbi = index * 6;
	int uvbi = index * 6;

	m_vb[vbi++] = v1;			m_uvb[uvbi++] = glm::vec2(texture->m_left, texture->m_top);
	m_vb[vbi++] = v2;			m_uvb[uvbi++] = glm::vec2(texture->m_right, texture->m_top);
	m_vb[vbi++] = v3;			m_uvb[uvbi++] = glm::vec2(texture->m_left, texture->m_bottom);
	m_vb[vbi++] = v3;			m_uvb[uvbi++] = glm::vec2(texture->m_left, texture->m_bottom);
	m_vb[vbi++] = v2;			m_uvb[uvbi++] = glm::vec2(texture->m_right, texture->m_top);
	m_vb[vbi++] = v4;			m_uvb[uvbi++] = glm::vec2(texture->m_right, texture->m_bottom);
}

void QuadMesh2::set(int index, const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v4,
					const glm::vec2& t1, const glm::vec2& t2, const glm::vec2& t3, const glm::vec2& t4)
{
	assert(index >= 0 && index < (m_capacity * 6));

	int vbi = index * 6;
	int uvbi = index * 6;

	m_vb[vbi++] = v1;			m_uvb[uvbi++] = t1;
	m_vb[vbi++] = v2;			m_uvb[uvbi++] = t2;
	m_vb[vbi++] = v3;			m_uvb[uvbi++] = t3;
	m_vb[vbi++] = v3;			m_uvb[uvbi++] = t3;
	m_vb[vbi++] = v2;			m_uvb[uvbi++] = t2;
	m_vb[vbi++] = v4;			m_uvb[uvbi++] = t4;
}

void QuadMesh2::render(const Shaders::UIShaderContext* context)
{
	glDisable(GL_CULL_FACE);

	glVertexAttribPointer(context->position, 2, GL_FLOAT, false, 8, m_vb);
	glEnableVertexAttribArray(context->position);
	glVertexAttribPointer(context->tex_coord, 2, GL_FLOAT, false, 8, m_uvb);
	glEnableVertexAttribArray(context->tex_coord);

	glDrawArrays(GL_TRIANGLES, 0, m_num_verts);

	glDisableVertexAttribArray(context->position);
	glDisableVertexAttribArray(context->tex_coord);
}
