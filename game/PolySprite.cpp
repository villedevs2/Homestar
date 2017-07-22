#include "PolySprite.h"

PolySprite::PolySprite(const glm::vec2* points, const glm::vec2* uvs, int num_points, float z)
{
	m_points = new glm::vec2[num_points];
	m_uvs = new glm::vec2[num_points];
	m_num_points = num_points;
	m_z = z;

	for (int i=0; i < num_points; i++)
	{
		m_points[i] = points[i];
		m_uvs[i] = uvs[i];
	}

	m_vb = new float[((num_points - 2) * 3) * 5];
	tesselate(0);
}

PolySprite::PolySprite(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, float z, Texture* texture)
{
	m_points = new glm::vec2[4];
	m_uvs = new glm::vec2[4];
	m_num_points = 4;
	m_z = z;

	m_points[0] = p1;
	m_points[1] = p2;
	m_points[2] = p3;
	m_points[3] = p4;

	m_uvs[0] = glm::vec2(texture->m_left, texture->m_top);
	m_uvs[1] = glm::vec2(texture->m_left, texture->m_bottom);
	m_uvs[2] = glm::vec2(texture->m_right, texture->m_bottom);
	m_uvs[3] = glm::vec2(texture->m_right, texture->m_top);

	m_vb = new float[6 * 5];
	tesselate(0);
}

PolySprite::PolySprite(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4,
					   const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3, const glm::vec2& uv4, float z)
{
	m_points = new glm::vec2[4];
	m_uvs = new glm::vec2[4];
	m_num_points = 4;
	m_z = z;

	m_points[0] = p1;
	m_points[1] = p2;
	m_points[2] = p3;
	m_points[3] = p4;

	m_uvs[0] = uv1;
	m_uvs[1] = uv2;
	m_uvs[2] = uv3;
	m_uvs[3] = uv4;

	m_vb = new float[6 * 5];
	tesselate(0);
}

PolySprite::~PolySprite()
{
	delete [] m_points;
	delete [] m_uvs;
	delete [] m_vb;
}

const glm::vec2* PolySprite::getPoints()
{
	return m_points;
}

int PolySprite::getNumPoints()
{
	return m_num_points;
}

int PolySprite::tesselate(int vb_index)
{
	int vbi = vb_index;

	for (int i=2; i < m_num_points; i++)
	{
		m_vb[vbi++] = m_points[0].x;
		m_vb[vbi++] = m_points[0].y;
		m_vb[vbi++] = m_z;
		m_vb[vbi++] = m_uvs[0].x;
		m_vb[vbi++] = m_uvs[0].y;

		m_vb[vbi++] = m_points[i-1].x;
		m_vb[vbi++] = m_points[i-1].y;
		m_vb[vbi++] = m_z;
		m_vb[vbi++] = m_uvs[i-1].x;
		m_vb[vbi++] = m_uvs[i-1].y;

		m_vb[vbi++] = m_points[i].x;
		m_vb[vbi++] = m_points[i].y;
		m_vb[vbi++] = m_z;
		m_vb[vbi++] = m_uvs[i].x;
		m_vb[vbi++] = m_uvs[i].y;
	}

	m_num_tris = m_num_points - 2;

	return vbi - vb_index;
}

void PolySprite::render(const Shaders::GameShaderContext* context)
{
	glVertexAttribPointer(context->position, 3, GL_FLOAT, false, 20, (GLfloat*)m_vb);
	glEnableVertexAttribArray(context->position);
	glVertexAttribPointer(context->tex_coord, 2, GL_FLOAT, false, 20, (GLfloat*)(m_vb)+3);
	glEnableVertexAttribArray(context->tex_coord);

	glDrawArrays(GL_TRIANGLES, 0, m_num_tris * 3);

	glDisableVertexAttribArray(context->position);
	glDisableVertexAttribArray(context->tex_coord);
}