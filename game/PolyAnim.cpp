#include "PolyAnim.h"

PolyAnim::PolyAnim(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, int num_frames, float z, TextureSet* textures)
{
	std::vector<glm::vec4> verts;

	for (int frame=0; frame < num_frames; frame++)
	{
		Texture* tex = textures->getTexture(frame);

		verts.push_back(glm::vec4(p1.x, p1.y, tex->m_left, tex->m_top));
		verts.push_back(glm::vec4(p2.x, p2.y, tex->m_left, tex->m_bottom));
		verts.push_back(glm::vec4(p3.x, p3.y, tex->m_right, tex->m_bottom));
		verts.push_back(glm::vec4(p4.x, p4.y, tex->m_right, tex->m_top));
	}

	init(verts, 4, num_frames, z);

	/*
	float *vb = new float[vbsize];

	std::vector<glm::vec4> polys;

	int vbi = 0;

	for (int frame=0; frame < num_frames; frame++)
	{
		PolyFrame* f = &m_frame[frame];

		f->m_tex = textures->getTexture(frame);
		f->m_num_tris = num_tris;
		f->m_vbo_index = vbi;

		points[0] = p1;
		points[1] = p2;
		points[2] = p3;
		points[3] = p4;

		uvs[0] = glm::vec2(f->m_tex->m_left, f->m_tex->m_top);
		uvs[1] = glm::vec2(f->m_tex->m_left, f->m_tex->m_bottom);
		uvs[2] = glm::vec2(f->m_tex->m_right, f->m_tex->m_bottom);
		uvs[3] = glm::vec2(f->m_tex->m_right, f->m_tex->m_top);

		vbi += tesselate(&vb[vbi], points, uvs, 4, z);
	}

	if (glIsBuffer(m_vbo))
		glDeleteBuffers(1, &m_vbo);

	glGenBuffers(1, &m_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, (vbi * sizeof(float)), vb, GL_STATIC_DRAW);

	delete [] vb;
	*/
}

PolyAnim::PolyAnim(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, float z, Texture* texture)
{
	std::vector<glm::vec4> verts;

	verts.push_back(glm::vec4(p1.x, p1.y, texture->m_left, texture->m_top));
	verts.push_back(glm::vec4(p2.x, p2.y, texture->m_left, texture->m_bottom));
	verts.push_back(glm::vec4(p3.x, p3.y, texture->m_right, texture->m_bottom));
	verts.push_back(glm::vec4(p4.x, p4.y, texture->m_right, texture->m_top));

	init(verts, 4, 1, z);
}

PolyAnim::PolyAnim(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4,
				   const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3, const glm::vec2& uv4, float z)
{
	std::vector<glm::vec4> verts;

	verts.push_back(glm::vec4(p1.x, p1.y, uv1.x, uv1.y));
	verts.push_back(glm::vec4(p2.x, p2.y, uv2.x, uv2.y));
	verts.push_back(glm::vec4(p3.x, p3.y, uv3.x, uv3.y));
	verts.push_back(glm::vec4(p4.x, p4.y, uv4.x, uv4.y));
	
	init(verts, 4, 1, z);
}

PolyAnim::PolyAnim(const glm::vec2* points, const glm::vec2* uvs, int num_points, float z)
{
	std::vector<glm::vec4> verts;

	for (int i=0; i < num_points; i++)
	{
		verts.push_back(glm::vec4(points[i].x, points[i].y, uvs[i].x, uvs[i].y));
	}

	init(verts, num_points, 1, z);
}

PolyAnim::~PolyAnim()
{
	delete [] m_frame;
	
	if (glIsBuffer(m_vbo))
		glDeleteBuffers(1, &m_vbo);
}

void PolyAnim::init(const std::vector<glm::vec4>& verts, int num_points, int num_frames, float z)
{
	m_frame = new PolyFrame[num_frames];
	m_num_frames = num_frames;

	int num_tris = num_points - 2;
	int vbsize = num_frames * (num_tris * 15);
	float *vb = new float[vbsize];

	int vbi = 0;
	int vin = 0;

	for (int frame=0; frame < num_frames; frame++)
	{
		PolyFrame* f = &m_frame[frame];

		f->m_num_tris = num_tris;
		f->m_vbo_index = vbi/5;

		vbi += tesselate(&vb[vbi], verts, frame*4, num_points, z);

		vin += num_points;
	}

	if (glIsBuffer(m_vbo))
		glDeleteBuffers(1, &m_vbo);

	glGenBuffers(1, &m_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, (vbi * sizeof(float)), vb, GL_STATIC_DRAW);

	delete [] vb;
}

int PolyAnim::tesselate(float *vb, const std::vector<glm::vec4>& verts, int index, int num_points, float z)
{
	int vbi = 0;

	for (int i=2; i < num_points; i++)
	{
		vb[vbi++] = verts[index].x;
		vb[vbi++] = verts[index].y;
		vb[vbi++] = z;
		vb[vbi++] = verts[index].z;
		vb[vbi++] = verts[index].w;

		vb[vbi++] = verts[index+i-1].x;
		vb[vbi++] = verts[index+i-1].y;
		vb[vbi++] = z;
		vb[vbi++] = verts[index+i-1].z;
		vb[vbi++] = verts[index+i-1].w;

		vb[vbi++] = verts[index+i].x;
		vb[vbi++] = verts[index+i].y;
		vb[vbi++] = z;
		vb[vbi++] = verts[index+i].z;
		vb[vbi++] = verts[index+i].w;
	}

	return vbi;
}

int PolyAnim::getNumFrames()
{
	return m_num_frames;
}

void PolyAnim::render(const Shaders::GameShaderContext* context, int frame)
{
	assert(frame >= 0 && frame < m_num_frames);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		
	glVertexAttribPointer(context->position, 3, GL_FLOAT, false, 20, (void*)0);
	glEnableVertexAttribArray(context->position);
	glVertexAttribPointer(context->tex_coord, 2, GL_FLOAT, false, 20, (void*)12);
	glEnableVertexAttribArray(context->tex_coord);

	glDrawArrays(GL_TRIANGLES, m_frame[frame].m_vbo_index, m_frame[frame].m_num_tris * 3);

	glDisableVertexAttribArray(context->position);
	glDisableVertexAttribArray(context->tex_coord);	

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// TODO REMOVE
}

void PolyAnim::render(const Shaders::GameShaderContext* context)
{
	render(context, 0);
}