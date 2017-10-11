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
	enum TileType
	{
		TILE_FULL = 0,
		TILE_LEFT = 1,
		TILE_RIGHT = 2,
		TILE_TOP = 3,
		TILE_BOTTOM = 4,
		TILE_MID = 5,
		TILE_CORNER_TL = 6,
		TILE_CORNER_TR = 7,
		TILE_CORNER_BL = 8,
		TILE_CORNER_BR = 9,
	};

	enum TileSide
	{
		SIDE_LEFT = 0x1,
		SIDE_TOP_LEFT = 0x2,
		SIDE_TOP_RIGHT = 0x4,
		SIDE_RIGHT = 0x8,
		SIDE_BOT_RIGHT = 0x10,
		SIDE_BOT_LEFT = 0x20,
		SIDE_MID = 0x40,
	};

	enum TopType
	{
		TOP_FLAT = 0,
		TOP_POINTY = 1
	};

	enum ShadingType
	{
		SHADING_STANDARD = 0,
		SHADING_ENVIRONMENT = 1,
		SHADING_SELFLUMINOUS = 2,
	};

	struct Tile
	{
		unsigned int color;
		StaticGeometry::TileType type;		
		StaticGeometry::TopType top_type;		
		StaticGeometry::ShadingType shading_type;		
		glm::vec2 top_uv[6];
		glm::vec2 side_uv[4];
		int side_bits;
		float top_height;
		int shading;
	};


	struct WallBucket
	{
		int x;
		int y;
		int vbi;
		unsigned int map[8 * 8];
	};

	struct FloorBucket
	{
		int x;
		int y;
		int vbi;
		unsigned short map[8 * 8];
	};

	static const unsigned int TILE_MASK = 0xffff;
	static const unsigned int TILE_EMPTY = 0xffff;
	static const unsigned int Z_MASK = 0xff0000;
	static const unsigned int Z_SHIFT = 16;

	static const int Z_MAX = 100;

	static const int AREA_WIDTH = 8192;
	static const int AREA_HEIGHT = 8192;
	static const int AREA_CENTER_X = AREA_WIDTH / 2;
	static const int AREA_CENTER_Y = AREA_HEIGHT / 2;
	static const int BUCKET_WIDTH = 8;
	static const int BUCKET_HEIGHT = 8;

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

	int insertTile(const StaticGeometry::Tile& tile);
	void insertWallBucket(const StaticGeometry::WallBucket& bucket, int bx, int by);
	void insertFloorBucket(const StaticGeometry::FloorBucket& bucket, int bx, int by);

	glm::vec2 getPoint(int index);

private:
	struct GeoObject
	{
		glm::vec2 pos[8];
		glm::vec2 uv[8];
		int num_points;
		float z;
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
	WallBucket** m_wall_buckets;
	FloorBucket** m_floor_buckets;
	std::vector<StaticGeometry::Tile*> m_tiles;

	GLuint m_vbo;
};