#pragma once

#include <vector>
#include <string>

#include "format\format.h"
#include "glm/glm.hpp"

#include "Shaders.h"
#include "Debug.h"

class StaticGeometry
{
public:
	StaticGeometry();
	~StaticGeometry();

	void init(float level_minx, float level_maxx, float level_miny, float level_maxy);
	void shutdown();
	void get_stats(int* vis_buckets, int* vis_tris);
	void insert(const glm::vec2* p, const glm::vec2* uv, int num_points, float z);
	void process();
	void tesselate();
	void update(float x1, float x2, float y1, float y2);
	void render(const Shaders::GameShaderContext* context);

private:
	struct GeoObject
	{
		glm::vec2 pos[8];
		glm::vec2 uv[8];
		int num_points;
		float z;
	};

	struct Bucket
	{
		int vbi;
		int num_tris;
	};

	int processBucket(int index, float minx, float maxx, float miny, float maxy, std::vector<float>& vb);
	int clip(glm::vec4* verts, int num_verts, int side, float value);
	bool point_side(const glm::vec4& v, int side, float value);
	float line_intersect(const glm::vec4& v1, const glm::vec4& v2, int side, float value);

	float m_bucket_min_x;
	float m_bucket_max_x;
	float m_bucket_min_y;
	float m_bucket_max_y;
	
	int m_num_buckets_x;
	int m_num_buckets_y;

	float m_bucket_width;
	float m_bucket_height;

	int m_vis_bucket_xstart;
	int m_vis_bucket_xend;
	int m_vis_bucket_ystart;
	int m_vis_bucket_yend;

	std::vector<GeoObject> m_geo;
	Bucket* m_buckets;

	GLuint m_vbo;
};