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
	enum TileCategory
	{
		TILE_FLOOR1 = 0,
		TILE_FLOOR2 = 1,
		TILE_FLOOR3 = 2,
		TILE_FLOOR4 = 3,
		TILE_FLOOR5 = 4,
		TILE_FLOOR6 = 5,
		TILE_WALL1 = 6,
		TILE_WALL2 = 7,
		TILE_WALL3 = 8,
		TILE_WALL4 = 9,
		TILE_WALL5 = 10,
		TILE_WALL6 = 11,
		TILE_TOWER1 = 12,
		TILE_TOWER2 = 13,
		TILE_TOWER3 = 14,
		TILE_TOWER4 = 15,
		TILE_TOWER5 = 16,
		TILE_TOWER6 = 17,
		TILE_ENV1 = 18,
		TILE_ENV2 = 19,
		TILE_ENV3 = 20,
		TILE_ENV4 = 21,
		TILE_ENV5 = 22,
		TILE_ENV6 = 23
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
		NUM_SHADING_TYPES,
	};

	struct Tile
	{
		unsigned int color;
		StaticGeometry::TopType top_type;		
		StaticGeometry::ShadingType shading_type;		
		glm::vec2 floor_uv[6];
		glm::vec2 floor_cenuv;
		glm::vec2 wallmid_uv[4];
		glm::vec2 walltop_uv[4];
		glm::vec2 wallbot_uv[4];
		int side_bits;
		float top_height;
		int shading;
	};


	struct Bucket
	{
		int x;
		int y;
		int vbi;
		int num_tris;
		unsigned int map[8 * 8];
	};

	static const unsigned int TILE_MASK = 0xff;
	static const unsigned int TILE_EMPTY = 0xff;
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

	void init();
	void shutdown();
	void get_stats(int* vis_buckets, int* vis_tris);
	void insert(const glm::vec2* p, const glm::vec2* uv, int num_points, float z);
	void process();
	void tesselate();
	int tesselateTile(TileCategory tile, float x, float y, std::vector<float>& vb);
	void update(float x1, float x2, float y1, float y2);
	void render(const Shaders::GameShaderContext* context);

	int insertTile(const StaticGeometry::Tile& tile);
	void insertBucket(const StaticGeometry::Bucket& bucket);

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

	float m_bucket_width;
	float m_bucket_height;

	int m_vis_bucket_xstart;
	int m_vis_bucket_xend;
	int m_vis_bucket_ystart;
	int m_vis_bucket_yend;

	std::vector<GeoObject> m_geo;
	Bucket* m_buckets[(AREA_WIDTH/BUCKET_WIDTH)*(AREA_HEIGHT/BUCKET_WIDTH)];
	std::vector<StaticGeometry::Tile*> m_tiles;

	GLuint m_vbo;
};