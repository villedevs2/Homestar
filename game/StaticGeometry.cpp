#include "StaticGeometry.h"

#define DEBUG_CLIPPING 0

StaticGeometry::StaticGeometry()
{
	m_bucket_width = 8.0f;
	m_bucket_height = 8.0f;
}

StaticGeometry::~StaticGeometry()
{
}

void StaticGeometry::init(float level_minx, float level_maxx, float level_miny, float level_maxy)
{
	m_bucket_min_x = 0.0f;
	m_bucket_max_x = 0.0f;
	m_bucket_min_y = 0.0f;
	m_bucket_max_y = 0.0f;

	m_num_buckets_x = 0;
	m_num_buckets_y = 0;

	m_vis_bucket_xstart = 0;
	m_vis_bucket_xend = 0;
	m_vis_bucket_ystart = 0;
	m_vis_bucket_yend = 0;


	float bminx = level_minx / m_bucket_width;
	float bmaxx = level_maxx / m_bucket_width;
	float bminy = level_miny / m_bucket_height;
	float bmaxy = level_maxy / m_bucket_height;

	float bucket_minx;
	float bucket_maxx;
	float bucket_miny;
	float bucket_maxy;
	float fraction;

	fraction = modff(bminx, &bucket_minx);
	if (bucket_minx < 0 && fraction != 0.0f)
		bucket_minx--;
		
	fraction = modff(bmaxx, &bucket_maxx);
	if (fraction > 0.0f)
		bucket_maxx++;

	fraction = modff(bminy, &bucket_miny);
	if (bucket_miny < 0 && fraction != 0.0f)
		bucket_miny--;

	fraction = modff(bmaxy, &bucket_maxy);
	if (fraction > 0.0f)
		bucket_maxy++;


	m_num_buckets_x = (int)(bucket_maxx) - (int)(bucket_minx);
	m_num_buckets_y = (int)(bucket_maxy) - (int)(bucket_miny);

	m_bucket_min_x = bucket_minx * m_bucket_width;
	m_bucket_max_x = bucket_maxx * m_bucket_width;
	m_bucket_min_y = bucket_miny * m_bucket_height;
	m_bucket_max_y = bucket_maxy * m_bucket_height;

	m_buckets = new Bucket[m_num_buckets_x * m_num_buckets_y];

	Debug::log(str(fmt::Format("Level MinX: {:f}, MaxX: {:f}, MinY: {:f}, MaxY: {:f}") << level_minx << level_maxx << level_miny << level_maxy));
	Debug::log(str(fmt::Format("Buckets X: {:d}, Y: {:d}") << m_num_buckets_x << m_num_buckets_y));
	Debug::log(str(fmt::Format("Bucket MinX: {:f}, MaxX: {:f}, MinY: {:f}, MaxY: {:f}") << (float)(bucket_minx * m_bucket_width) << (float)(bucket_maxx * m_bucket_width) << (float)(bucket_miny * m_bucket_height) << (float)(bucket_maxy * m_bucket_height)));

}

void StaticGeometry::shutdown()
{
	SAFE_DELETE(m_buckets);
}

void StaticGeometry::insert(const glm::vec2* p, const glm::vec2* uv, int num_points, float z)
{
	GeoObject geo;

	geo.num_points = num_points;
	geo.z = z;

	for (int i=0; i < num_points; i++)
	{
		geo.pos[i] = p[i];
		geo.uv[i] = uv[i];
	}

	m_geo.push_back(geo);
}

bool StaticGeometry::point_side(const glm::vec4& v, int side, float value)
{
	switch (side)
	{
		case 0:
			if (v.x > value) return false;
			break;
		case 1:
			if (v.x < value) return false;
			break;
		case 2:
			if (v.y > value) return false;
			break;
		case 3:
			if (v.y < value) return false;
			break;
	}
	return true;
}

float StaticGeometry::line_intersect(const glm::vec4& v1, const glm::vec4& v2, int side, float value)
{
	float t = 0;
	float l;

	switch (side)
	{
		case 0:
			l = v2.x - v1.x;
			if (l != 0.0f)
				t = fabs(value - v1.x) / l;
			break;
		case 1:
			l = v1.x - v2.x;
			if (l != 0.0f)
				t = fabs(value - v1.x) / l;
			break;
		case 2:
			l = v2.y - v1.y;
			if (l != 0.0f)
				t = fabs(value - v1.y) / l;
			break;
		case 3:
			l = v1.y - v2.y;
			if (l != 0.0f)
				t = fabs(value - v1.y) / l;
			break;
	}

	return t;
}

int StaticGeometry::clip(glm::vec4* verts, int num_verts, int side, float value)
{
	glm::vec4 clipv[32];

	int num_clip_verts = 0;
	int previ = num_verts - 1;

	for (int i=0; i < num_verts; i++)
	{
		bool v1_in = point_side(verts[i], side, value);
		bool v2_in = point_side(verts[previ], side, value);

		if (v1_in && v2_in)
		{
			clipv[num_clip_verts] = verts[i];
			num_clip_verts++;
		}
		else if (!v1_in && v2_in)
		{
			float t = line_intersect(verts[previ], verts[i], side, value);
			clipv[num_clip_verts] = verts[previ] + ((verts[i] - verts[previ]) * t);
			num_clip_verts++;
		}
		else if (v1_in && !v2_in)
		{
			float t = line_intersect(verts[i], verts[previ], side, value);
			clipv[num_clip_verts] = verts[i] + ((verts[previ] - verts[i]) * t);
			num_clip_verts++;

			clipv[num_clip_verts] = verts[i];
			num_clip_verts++;
		}

		previ = i;
	}

	memcpy(verts, clipv, sizeof(glm::vec4) * num_clip_verts);
	return num_clip_verts;
}

int StaticGeometry::processBucket(int index, float minx, float maxx, float miny, float maxy, std::vector<float>& vb)
{
	Bucket* bucket = &m_buckets[index];

	int num_pass = 0;
	int num_needs_clip = 0;
	int num_reject = 0;

	int num_tris = 0;
	int num_clip_tris = 0;
	int num_degen_tris = 0;

	int numgeo = m_geo.size();
	for (int i=0; i < numgeo; i++)
	{
		float poly_minx = m_geo[i].pos[0].x;
		float poly_maxx = m_geo[i].pos[0].x;
		float poly_miny = m_geo[i].pos[0].y;
		float poly_maxy = m_geo[i].pos[0].y;

		for (int j=0; j < m_geo[i].num_points; j++)
		{
			glm::vec2 p = m_geo[i].pos[j];
			if (p.x < poly_minx) poly_minx = p.x;
			if (p.x > poly_maxx) poly_maxx = p.x;
			if (p.y < poly_miny) poly_miny = p.y;
			if (p.y > poly_maxy) poly_maxy = p.y;
		}

		if (poly_maxx < minx || poly_minx > maxx || poly_maxy < miny || poly_miny > maxy)	// fully outside bucket
		{
			num_reject++;
		}
		else if (poly_minx >= minx && poly_maxx <= maxx && poly_miny >= miny && poly_maxy <= maxy)		// fully inside bucket
		{
			num_pass++;
			
			GeoObject& geo = m_geo.at(i);
			for (int j=2; j < geo.num_points; j++)
			{
				vb.push_back(geo.pos[0].x);
				vb.push_back(geo.pos[0].y);
				vb.push_back(geo.z);
				vb.push_back(geo.uv[0].x);
				vb.push_back(geo.uv[0].y);
				vb.push_back(geo.pos[j-1].x);
				vb.push_back(geo.pos[j-1].y);
				vb.push_back(geo.z);
				vb.push_back(geo.uv[j-1].x);
				vb.push_back(geo.uv[j-1].y);
				vb.push_back(geo.pos[j].x);
				vb.push_back(geo.pos[j].y);
				vb.push_back(geo.z);
				vb.push_back(geo.uv[j].x);
				vb.push_back(geo.uv[j].y);

				num_tris++;
			}
			
		}
		else
		{
			GeoObject& geo = m_geo.at(i);
			glm::vec4 clipv[32];

#if DEBUG_CLIPPING
			Debug::log("CLIP VERTEX");
#endif

			for (int j=0; j < geo.num_points; j++)
			{
				clipv[j] = glm::vec4(geo.pos[j].x, geo.pos[j].y, geo.uv[j].x, geo.uv[j].y);

#if DEBUG_CLIPPING
				Debug::log(str(fmt::Format("  In {:d} X: {:f}, Y: {:f}") << j << clipv[j].x << clipv[j].y));
#endif
			}

			int num_verts = geo.num_points;
			num_verts = clip(clipv, num_verts, 0, maxx);
#if DEBUG_CLIPPING
			for (int j=0; j < num_verts; j++)
			{
				Debug::log(str(fmt::Format("  Pass0 {:d} X: {:f}, Y: {:f}") << j << clipv[j].x << clipv[j].y));
			}
#endif
			num_verts = clip(clipv, num_verts, 1, minx);
#if DEBUG_CLIPPING
			for (int j=0; j < num_verts; j++)
			{
				Debug::log(str(fmt::Format("  Pass1 {:d} X: {:f}, Y: {:f}") << j << clipv[j].x << clipv[j].y));
			}
#endif
			num_verts = clip(clipv, num_verts, 2, maxy);
#if DEBUG_CLIPPING
			for (int j=0; j < num_verts; j++)
			{
				Debug::log(str(fmt::Format("  Pass2 {:d} X: {:f}, Y: {:f}") << j << clipv[j].x << clipv[j].y));
			}
#endif
			num_verts = clip(clipv, num_verts, 3, miny);
#if DEBUG_CLIPPING
			for (int j=0; j < num_verts; j++)
			{
				Debug::log(str(fmt::Format("  Out   {:d} X: {:f}, Y: {:f}") << j << clipv[j].x << clipv[j].y));
			}
			for (int j=0; j < num_verts; j++)
			{
				if (clipv[j].x < minx || clipv[j].x > maxx || clipv[j].y < miny || clipv[j].y > maxy)
				{
					Debug::log(str(fmt::Format("   CLIP FAIL vert {:d}") << j));
				}
			}
#endif

			for (int j=2; j < num_verts; j++)
			{
				bool degen = false;
				if (clipv[0].x == clipv[j-1].x && clipv[0].x == clipv[j].x)
				{
					degen = true;
				}
				if (clipv[0].y == clipv[j-1].y && clipv[0].y == clipv[j].y)
				{
					degen = true;
				}

				if (!degen)
				{
					vb.push_back(clipv[0].x);
					vb.push_back(clipv[0].y);
					vb.push_back(geo.z);
					vb.push_back(clipv[0].z);
					vb.push_back(clipv[0].w);
					vb.push_back(clipv[j-1].x);
					vb.push_back(clipv[j-1].y);
					vb.push_back(geo.z);
					vb.push_back(clipv[j-1].z);
					vb.push_back(clipv[j-1].w);
					vb.push_back(clipv[j].x);
					vb.push_back(clipv[j].y);
					vb.push_back(geo.z);
					vb.push_back(clipv[j].z);
					vb.push_back(clipv[j].w);
					
					num_tris++;
					num_clip_tris++;
				}
				else
				{
					num_degen_tris++;
				}
			}

			num_needs_clip++;
		}
	}

	Debug::log(str(fmt::Format("Bucket {:d}: {:f}, {:f}, {:f}, {:f}, pass {:d}, clip {:d}, reject {:d}, numtris {:d}, num clip tris {:d}, num degen {:d}") << index << minx << maxx << miny << maxy << num_pass << num_needs_clip << num_reject << num_tris << num_clip_tris << num_degen_tris));

	return num_tris;
}

void StaticGeometry::tesselate()
{
	if (glIsBuffer(m_vbo))
		glDeleteBuffers(1, &m_vbo);

	glGenBuffers(1, &m_vbo);

	int numgeo = m_geo.size();

	std::vector<float> tempvb;

	int tri_index = 0;

	for (int j=0; j < m_num_buckets_y; j++)
	{
		for (int i=0; i < m_num_buckets_x; i++)
		{
			float minx = m_bucket_min_x + (i * m_bucket_width);
			float maxx = minx + m_bucket_width;
			float miny = m_bucket_min_y + (j * m_bucket_height);
			float maxy = miny + m_bucket_height;

			int index = j * m_num_buckets_x + i;

			int tris = processBucket(index, minx, maxx, miny, maxy, tempvb);

			m_buckets[index].vbi = tri_index;
			m_buckets[index].num_tris = tris;

			tri_index += tris;
		}
	}


	// calculate total num of tris
	int vb_size = tempvb.size();


	// allocate temps
	float* vb = new float[vb_size];

	int vbi = 0;
	for (int i=0; i < vb_size; i++)
	{
		vb[vbi++] = tempvb[i];
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, (vbi * sizeof(float)), vb, GL_STATIC_DRAW);

	delete [] vb;
	m_geo.clear();

	tempvb.clear();
}

void StaticGeometry::update(float x1, float x2, float y1, float y2)
{
	// calculate visible buckets
	m_vis_bucket_xstart = (x1 - m_bucket_min_x) / m_bucket_width;
	m_vis_bucket_xend = (x2 - m_bucket_min_x) / m_bucket_width;
	m_vis_bucket_ystart = (y1 - m_bucket_min_y) / m_bucket_height;
	m_vis_bucket_yend = (y2 - m_bucket_min_y) / m_bucket_height;
	if (m_vis_bucket_xstart < 0) m_vis_bucket_xstart = 0;
	if (m_vis_bucket_xend >= m_num_buckets_x) m_vis_bucket_xend = m_num_buckets_x - 1;
	if (m_vis_bucket_ystart < 0) m_vis_bucket_ystart = 0;
	if (m_vis_bucket_yend >= m_num_buckets_y) m_vis_bucket_yend = m_num_buckets_y - 1;
}

void StaticGeometry::render(const Shaders::GameShaderContext* context)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		
	glVertexAttribPointer(context->position, 3, GL_FLOAT, false, 20, (void*)0);
	glEnableVertexAttribArray(context->position);
	glVertexAttribPointer(context->tex_coord, 2, GL_FLOAT, false, 20, (void*)12);
	glEnableVertexAttribArray(context->tex_coord);

	for (int j=m_vis_bucket_ystart; j <= m_vis_bucket_yend; j++)
	{
		int li = j * m_num_buckets_x;
		for (int i=m_vis_bucket_xstart; i <= m_vis_bucket_xend; i++)
		{
			const Bucket* bucket = &m_buckets[li + i];
			glDrawArrays(GL_TRIANGLES, bucket->vbi * 3, bucket->num_tris * 3);
		}
	}

	glDisableVertexAttribArray(context->position);
	glDisableVertexAttribArray(context->tex_coord);	

	glBindBuffer(GL_ARRAY_BUFFER, 0);	// TODO REMOVE
}

void StaticGeometry::process()
{
	std::sort(m_geo.begin(), m_geo.end(), [](const GeoObject& a, const GeoObject& b) { return a.z > b.z; });
}

void StaticGeometry::get_stats(int* vis_buckets, int* vis_tris)
{
	*vis_buckets = (m_vis_bucket_yend - m_vis_bucket_ystart) * (m_vis_bucket_xend - m_vis_bucket_xstart);
	*vis_tris = 0;

	for (int j=m_vis_bucket_ystart; j <= m_vis_bucket_yend; j++)
	{
		int li = j * m_num_buckets_x;
		for (int i=m_vis_bucket_xstart; i <= m_vis_bucket_xend; i++)
		{
			*vis_tris += m_buckets[li + i].num_tris;
		}
	}
}