#pragma once

#include "glm/glm.hpp"

#include "Texture2.h"
#include "Shaders.h"

class PolyFrame
{
	friend class PolyAnim;

private:
	int m_num_tris;
	int m_vbo_index;
};

class PolyAnim
{
public:
	// Fixed quad, animated with texture for each frame
	PolyAnim(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, int num_frames, float z, TextureSet* texture);

	// Fixed quad, not animated, texture
	PolyAnim(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4, float z, Texture* texture);

	// Fixed quad, not animated, manual UVs
	PolyAnim(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4,
			 const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3, const glm::vec2& uv4, float z);

	// Polygon, not animated, manual UVs
	PolyAnim(const glm::vec2* points, const glm::vec2* uvs, int num_points, float z);	

	~PolyAnim();

	void render(const Shaders::GameShaderContext* context, int frame);
	void render(const Shaders::GameShaderContext* context);
	int getNumFrames();
private:
	int tesselate(float *vb, const std::vector<glm::vec4>& verts, int index, int num_points, float z);
	void init(const std::vector<glm::vec4>& verts, int num_points, int num_frames, float z);

	GLuint m_vbo;

	PolyFrame* m_frame;
	int m_num_frames;

};