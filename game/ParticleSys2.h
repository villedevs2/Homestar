#pragma once

#define _USE_MATH_DEFINES				// this must be defined before *any* includes

#include "glm/glm.hpp"

#include "Shaders.h"
#include "Texture2.h"

#include <cmath>

class ParticleSys2
{
public:
	struct Emitter
	{
		glm::vec2 position;
		float angle;
		float angle_range;
		int time;
		int time_range;
		float width;
		float height;
		float particle_speed;
		float particle_speed_range;
	};

	ParticleSys2(int num, TextureSet* texset);
	~ParticleSys2();

	void render(const Shaders::ParticleShaderContext* context);
	void update(int time);
	void setEmitter(const ParticleSys2::Emitter& emitter);
	void setTexture(TextureSet* texset);
	void emit(int num);
	void emitArea(const glm::vec2* p, int num_points, int num_particles);
	int getNumActive();
	void setGravity(glm::vec2 gravity);
	void setGravity(float gx, float gy);
	void enableGravity(bool enable);
	void setParticleSize(float size);

private:
	struct Particle
	{
		glm::vec2 pos;
		glm::vec2 dir;
		glm::vec4 texture;
		timeunit timer;
	};

	void copy(int from, int to);

	Particle* m_particle;
	Emitter m_emitter;

	TextureSet* m_texset;

	int m_num_active;
	int m_num_max;

	glm::vec2 m_gravity;
	bool m_grav_enable;

	float m_particle_size;
};